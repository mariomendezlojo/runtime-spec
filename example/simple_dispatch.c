#include "stdio.h"

#define KERNEL_ADDRESS (hello_world)

void hello_world() {
	printf("Hello World!\n");
}


/**
 * Dispatch a kernel in the command queue of an HSA component.
 *
 * The source code has been simplified for readibility. We do not check the status code returned by invocations of the HSA
 * API, and we assume that no asynchronous errors are generated by the runtime while executing the kernel.
 */
#include "string.h"

#include "hsa.h"
// DO NOT MOVE THE FOLLOWING LINE - IT SHOULD BE AT LINE 20
void initialize_packet(hsa_kernel_dispatch_packet_t* dispatch_packet) {
    // Contents are zeroed:
    //    -Reserved fields must be 0
    //    -Type is set to HSA_PACKET_TYPE_ALWAYS_RESERVED, so the packet cannot be consumed by the packet processor
    memset(dispatch_packet, 0, sizeof(hsa_kernel_dispatch_packet_t));

    dispatch_packet->header.acquire_fence_scope = HSA_FENCE_SCOPE_COMPONENT;
    dispatch_packet->header.release_fence_scope = HSA_FENCE_SCOPE_COMPONENT;

    dispatch_packet->dimensions = 1;
    dispatch_packet->workgroup_size_x = 256;
    dispatch_packet->workgroup_size_y = 1;
    dispatch_packet->workgroup_size_z = 1;
    dispatch_packet->grid_size_x = 256;
    dispatch_packet->grid_size_y = 1;
    dispatch_packet->grid_size_z = 1;

    // Indicate which ISA to run. The application is expected to have finalized a kernel (for example, using the finalization API).
    // We will assume that the kernel object location is stored in KERNEL_ADDRESS
    dispatch_packet->kernel_object_address = (uint64_t) KERNEL_ADDRESS;

    // Assume our kernel receives no arguments
    dispatch_packet->kernarg_address = 0;
}




// Find HSA agent that can process Kernel Dispatch packets.
// DO NOT MOVE THE FOLLOWING LINE - IT SHOULD BE AT LINE 50
hsa_status_t get_component(hsa_agent_t agent, void* data) {
    uint32_t features = 0;
    hsa_agent_get_info(agent, HSA_AGENT_INFO_FEATURE, &features);
    if (features & HSA_AGENT_FEATURE_KERNEL_DISPATCH) {
        // Store HSA component in the application-provided buffer and return
        hsa_agent_t* ret = (hsa_agent_t*) data;
        *ret = agent;
        return HSA_STATUS_INFO_BREAK;
    }
    // Keep iterating
    return HSA_STATUS_SUCCESS;
}

void packet_type_store_release(hsa_packet_header_t* header, hsa_packet_type_t type) {
    __atomic_store_n((uint8_t*) header, (uint8_t) type, __ATOMIC_RELEASE);
}

int main() {
    // Initialize the runtime
    hsa_init();

    // Retrieve the HSA component
    hsa_agent_t component;
    hsa_iterate_agents(get_component, &component);

    // Create a queue in the HSA component. The queue can hold 4 packets, and has no callback or service queue associated with it
    hsa_queue_t *queue;
    hsa_queue_create(component, 4, HSA_QUEUE_TYPE_SINGLE, NULL, NULL, &queue);

    // Request a packet ID from the queue. Since no packets have been enqueued yet, the expected ID is zero
    uint64_t packet_id = hsa_queue_add_write_index_relaxed(queue, 1);

    // Calculate the virtual address where to place the packet
    hsa_kernel_dispatch_packet_t* dispatch_packet = (hsa_kernel_dispatch_packet_t*) queue->base_address + packet_id;

    // Populate fields in Kernel Dispatch packet, except for the completion signal and the header type
    initialize_packet(dispatch_packet);

    // Create a signal with an initial value of one to monitor the task completion
    hsa_signal_t signal;
    hsa_signal_create(1, 0, NULL, &signal);
    dispatch_packet->completion_signal = signal;

    // Notify the queue that the packet is ready to be processed
    packet_type_store_release(&dispatch_packet->header, HSA_PACKET_TYPE_KERNEL_DISPATCH);
    hsa_signal_store_release(queue->doorbell_signal, packet_id);

    // Wait for the task to finish, which is the same as waiting for the value of the completion signal to be zero
    while (hsa_signal_wait_acquire(signal, HSA_EQ, 0, UINT64_MAX, HSA_WAIT_EXPECTANCY_UNKNOWN) != 0);

    // Done! The kernel has completed. Time to cleanup resources and leave
    hsa_signal_destroy(signal);
    hsa_queue_destroy(queue);
    hsa_shut_down();
    return 0;
}
