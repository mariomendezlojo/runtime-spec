/**
 * Dispatch a kernel in the command queue of a component.
 *
 * The source code has been simplified for readibility. For instance, we do not check the status code
 * returned by invocations of the HSA API, and we assume that no asynchronous errors are generated by the
 * runtime while executing the kernel.
 */
#include "assert.h"
#include "stdio.h"
#include "string.h"

#include "hsa.h"

// Find component that can process Dispatch packets.
hsa_status_t get_dispatch_component(hsa_agent_t agent, void* data) {
  uint32_t features = 0;
  hsa_agent_get_info(agent, HSA_AGENT_INFO_FEATURE, &features);
  if (features & HSA_AGENT_FEATURE_DISPATCH) {
    // Store component in user-provided buffer and return
    hsa_agent_t* ret = (hsa_agent_t*) data;
    *ret = agent;
    return HSA_STATUS_INFO_BREAK;
  }
  // Keep iterating
  return HSA_STATUS_SUCCESS;
}


int main() {
  // Initialize the runtime
  hsa_init();

  // Retrieve the component
  hsa_agent_t component;
  hsa_iterate_agents(get_dispatch_component, &component);

  // Create a queue in the selected component. The queue can hold up to four
  // packets, and has no callback or service queue associated with it.
  hsa_queue_t *queue;
  hsa_queue_create(component, 4, HSA_QUEUE_TYPE_SINGLE, NULL, NULL, &queue);

  // Setup the packet encoding the task to execute
  hsa_dispatch_packet_t dispatch_packet;
  const size_t packet_size = sizeof(dispatch_packet);
  memset(&dispatch_packet, 0, packet_size); // reserved fields are zeroed
  dispatch_packet.header.acquire_fence_scope = HSA_FENCE_SCOPE_COMPONENT;
  dispatch_packet.header.release_fence_scope = HSA_FENCE_SCOPE_COMPONENT;
  dispatch_packet.dimensions = 1;
  dispatch_packet.workgroup_size_x = 256;
  dispatch_packet.grid_size_x = 256;

  // Indicate which ISA to run. The application is expected to have finalized a
  // kernel (for example, using the finalization API). We will assume the object is
  // located at address 0xDEADBEEF
  dispatch_packet.kernel_object_address = 0xDEADBEEF;

  // Assume our kernel receives no arguments, so no need to set the kernarg_address
  // field in the packet.

  // Create a signal with an initial value of one to monitor the task completion
  hsa_signal_t signal;
  hsa_signal_create(1, 0, NULL, &signal);
  dispatch_packet.completion_signal = signal;

  // Request a packet ID from the queue
  // Since no packets have been enqueued yet, the expected ID is zero.
  uint64_t write_index = hsa_queue_add_write_index_relaxed(queue, 1);

  // Calculate the virtual address where to place the packet.
  // No need to check if the queue is full or the index might wrap around...
  void* dst = (void*)(queue->base_address + write_index * packet_size);
  memcpy(dst, &dispatch_packet, packet_size);

  // Notify the queue that the packet is ready to be processed
  dispatch_packet.header.type = HSA_PACKET_TYPE_DISPATCH;
  hsa_signal_store_release(queue->doorbell_signal, write_index);

  // Wait for the task to finish, which is the same as waiting for the value of the
  // completion signal to be zero.
  while (hsa_signal_wait_acquire(signal, HSA_EQ, 0, UINT64_MAX, HSA_WAIT_EXPECTANCY_UNKNOWN) != 0);

  // Done! The kernel has completed. Time to cleanup resources and leave.
  hsa_signal_destroy(signal);
  hsa_queue_destroy(queue);
  hsa_shut_down();
  return 0;
}