#ifndef HSACOREBASE_H
#define HSACOREBASE_H

#include <stdint.h> // uintXX_t
#include <stddef.h> // size_t

// placeholder for calling convention - check macro naming convention
#define HSA_API

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/** \defgroup status Errors and warnings.
 *  @{
 */

/**
 * @brief A Doxygen macro test
 */
#define TEST 4

/**
 * TODO
 */
typedef enum {
    /**
     * The function has been executed successfully.
     */
    HSA_STATUS_SUCCESS = 0,
    /**
     * Indicates that initialization attempt failed due to prior initialization.
     */
    HSA_EXT_STATUS_INFO_ALREADY_INITIALIZED,
    /**
     * TODO.
     */
    HSA_EXT_STATUS_INFO_UNRECOGNIZED_OPTIONS,

    /**
     * A signal wait has been abandoned before the condition associated with the
     * signal value and the wait is met.
     */
    HSA_STATUS_ERROR_WAIT_ABANDONED,

    /**
     * One of the actual arguments does not meet a precondition stated in the
     * documentation of the corresponding formal argument.
     */
    HSA_STATUS_ERROR_INVALID_ARGUMENT,
    /**
     * The component is invalid.
     */
    HSA_STATUS_ERROR_INVALID_COMPONENT,
    /**
     * The signal is invalid.
     */
    HSA_STATUS_ERROR_INVALID_SIGNAL,
    /**
     * The queue is invalid.
     */
    HSA_STATUS_ERROR_INVALID_QUEUE,
    /**
     * The runtime failed to allocate the necessary resources. This error
     * may also occur when the core runtime library needs to spawn threads or
     * create internal OS-specific events.
     */
    HSA_STATUS_ERROR_OUT_OF_RESOURCES,
    /**
     * Indicates that the AQL packet is malformed.
     */
    HSA_STATUS_ERROR_INVALID_PACKET_FORMAT,
    /**
     * Indicates that a signal we depend on has a negative value.
     */
    HSA_STATUS_ERROR_SIGNAL_DEPENDENCY,
    /**
     * An error has been detected while releasing a resource.
     */
    HSA_STATUS_ERROR_RESOURCE_FREE,
    /**
     * The pointer is not currently registered.
     */
    HSA_STATUS_ERROR_NOT_REGISTERED,
    /**
     * An API other than ::hsa_init has been invoked while the reference count
     * of the HSA runtime is zero.
     */
    HSA_STATUS_ERROR_NOT_INITIALIZED,
    /**
     * The maximum reference count for the object has been reached.
     */
    HSA_STATUS_ERROR_REFCOUNT_OVERFLOW,
    /**
     * TODO.
     */
    HSA_EXT_STATUS_ERROR_DIRECTIVE_MISMATCH,
    /**
     * Image format is not supported.
     */
    HSA_EXT_STATUS_ERROR_IMAGE_FORMAT_UNSUPPORTED,
    /**
     * Image size is not supported.
     */
    HSA_EXT_STATUS_ERROR_IMAGE_SIZE_UNSUPPORTED,


} hsa_status_t;

/**
 * @brief Query additional information about a status code.
 *
 * @param[in] status Status code that the user is seeking more information on.
 *
 * @param[out] status_string A ISO/IEC 646 encoded English language string that
 * potentially describes the error status. The string terminates in a NUL
 * character.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a status_string is NULL or
 * @a status is not a valid status code.
 */
hsa_status_t HSA_API hsa_status_string(hsa_status_t status,
      char * const * status_string);

/**
 * @brief Event object. Used to pass information from the HSA runtime to the
 * application.
 *
 * @details The event object communicates to the application what has happened
 * (@a status field), and might contain event-specific details that can be
 * parsed by the application to further understand the event.
 *
 */
typedef struct hsa_event_s {
    /**
     * Status code associated with the event.
     */
    hsa_status_t status;

    /**
     * Additional information about the event to be interpreted based on @a
     * status.
     */
    uint64_t data[4];

   /**
    * A string containing further information. ISO/IEC 646 character encoding
    * must be used. The string should be NUL terminated.
    */
    char info[64];

    /**
     * System timestamp to indicate when the event was discovered. If the
     * implementation chooses not to return the current timestamp, then
     * @a timestamp must be zero.
     */
    uint64_t timestamp;

} hsa_event_t;

/**
 * @brief Callback for events.
 */
typedef void (*hsa_event_callback_t)(const hsa_event_t *event);
/** @} */

/** \defgroup RuntimeCommon Runtime Common
 *  @{
 */

/**
 * @brief Value expressed as a power of two.
 */
typedef uint8_t hsa_powertwo8_t;

/**
 * @brief Power of two between 1 and 256.
 */
typedef enum {
  HSA_POWERTWO_1 = 0,
  HSA_POWERTWO_2 = 1,
  HSA_POWERTWO_4 = 2,
  HSA_POWERTWO_8 = 3,
  HSA_POWERTWO_16 = 4,
  HSA_POWERTWO_32 = 5,
  HSA_POWERTWO_64 = 6,
  HSA_POWERTWO_128 = 7,
  HSA_POWERTWO_256 = 8
} hsa_powertwo_t;

/**
 * @brief Three-dimensional coordinate.
 */
typedef struct hsa_dim3_s {
  /**
   * X dimension.
   */
   uint32_t x;

  /**
   * Y dimension.
   */
   uint32_t y;

   /**
    * Z dimension.
    */
   uint32_t z;
} hsa_dim3_t;

/**
 * @brief Dimensions in a 3D space.
 */
typedef enum {
  /**
   * X dimension.
   */
  HSA_DIM_X = 0,

  /**
   * Y dimension.
   */
  HSA_DIM_Y = 1,

  /**
   * Z dimension.
   */
  HSA_DIM_Z = 2,
} hsa_dim_t;

/**
* @brief Opaque pointer that is passed to all runtime functions that use
* callbacks. The runtime passes this pointer as the first argument to all 
* callbacks made by the function.
*/
typedef struct hsa_runtime_caller_s {
  /**
   * Opaque pointer that is passed as the first argument to callback
   * functions invoked by a runtime function.
   */
  uint64_t caller;
} hsa_runtime_caller_t;

/**
 * @brief Call back function for allocating data.
 */
typedef hsa_status_t (*hsa_runtime_alloc_data_callback_t)(
  hsa_runtime_caller_t caller,
  size_t byte_size,
  void **address);

/** @} **/


/** \defgroup initshutdown TODO
 *  @{
 */

/**
 * @brief Initialize the HSA runtime.
 *
 * @details Initializes the HSA runtime if it is not already initialized, and
 * increases the reference counter associated with the HSA runtime for the
 * current process. Invocation of any HSA function other than ::hsa_init results
 * in undefined behavior if the current HSA runtime reference counter is less
 * than one.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is failure to allocate
 * the resources required by the implementation.
 *
 * @retval ::HSA_STATUS_ERROR_REFCOUNT_OVERFLOW If the runtime reference
 * count reaches INT32_MAX.
 */
hsa_status_t HSA_API hsa_init();

/**
 * @brief Shut down the HSA runtime.
 *
 * @details Decreases the reference count of the runtime instance. When the
 * reference count reaches zero, the runtime is no longer considered valid but
 * the user might call ::hsa_init to initialize the HSA runtime again.
 *
 * Once the reference count of the runtime reaches zero, all the resources
 * associated with it (queues, signals, topology information, etc.) are
 * considered invalid and any attempt to reference them in subsequent API calls
 * results in undefined behavior. When the reference count reaches zero, the HSA
 * runtime might release resources associated with it.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 */
hsa_status_t HSA_API hsa_shut_down();

/** @} */

/** \defgroup topology TODO
 *  @{
 */

/**
 * @brief Agent type.
 */
typedef enum {
    /**
     * Host agent (CPU).
     */
    HSA_AGENT_TYPE_HOST = 1,

    /**
     * HSA component.
     */
    HSA_AGENT_TYPE_COMPONENT = 2,

    /**
     * The agent is capable of agent dispatches, and can serve as a target for
     * them.
     */
    HSA_AGENT_TYPE_AGENT_DISPATCH = 4
} hsa_agent_type_t;

/**
 * @brief HSA agent.
 */
typedef struct hsa_agent_s {
    /**
     * ID of the node this agent/component belongs to.
     */
    uint32_t node_id;

    /**
     * Unique identifier for an HSA agent.
     */
    uint32_t id;

    /**
     * Agent type, bit-field.
     */
    hsa_agent_type_t agent_type;

    /**
     * The vendor of the agent/component. ISO/IEC 646 character encoding must be
     * used. If the name is less than 16 characters then remaining characters
     * must be set to 0.
     */
    char vendor[16];

    /**
     * The name of this agent/component. ISO/IEC 646 character encoding must be
     * used. If the name is less than 16 characters then remaining characters
     * must be set to 0.
     */
    char name[16];

    /**
     * Array of memory descriptor offsets.  Number of elements in array equals
     * @a number_memory_descriptors.
    */
    uint32_t *memory_descriptors;

    /**
     * Number of the different types of memories available to this agent. Zero
     * indicates that no information is available.
     */
    uint32_t number_memory_descriptors;

    /**
     * Array of cache descriptor offsets.  Number of elements in array equals @a
     * number_cache_descriptors.
    */
    uint32_t  *cache_descriptors;

    /**
     * Number of caches available to this agent/component. Zero indicates that
     * no information is available.
     */
    uint32_t number_cache_descriptors;

    /**
     * Subagent list of offsets, points to the offsets in the topology table.
     */
    uint32_t *subagent_offset_list;

    /**
     * Number of subagents.
     */
    uint32_t number_subagents;

    /**
     * Wave front size, i.e. number of work-items in a wavefront.
     */
    uint32_t wavefront_size;

    /**
     * Maximum size of the user queue in bytes allocatable via the runtime.
    */
    uint32_t queue_size;

    /**
     * Size (in bytes) of group memory available to a single work-group.
    */
    uint32_t group_memory_size_bytes;

    /**
     * Max number of fbarrier that can be used in any kernel and functions it
     * invokes.
     */
    uint32_t fbarrier_max_count;
    /**
     * Indicates if the agent supports position-independent code (the value is
     * not zero). Only applicable when the agent is a component.
     */
    uint8_t is_pic_supported;

} hsa_agent_t;

/**
 * @brief Memory segment.
 */
typedef struct hsa_segment_s {
    /**
     * Global segment.
     */
    uint8_t global:1;

    /**
     * Private segment.
     */
    uint8_t privat:1;

    /**
     * Group segment.
     */
    uint8_t group:1;

    /**
     * Kernarg segment.
     */
    uint8_t kernarg:1;

    /**
     * Readonly segment.
     */
    uint8_t readonly:1;

    /**
     * Reserved.
     */
    uint8_t reserved:1;
} hsa_segment_t;

/**
 * @brief Memory descriptor.
 *
 * @details Representation of a physical memory block or region. Implementations
 * may choose not to provide memory bandwidth or latency information, which case
 * zero is returned.
 */
typedef struct hsa_memory_descriptor_s {
    /**
     * ID of the node this memory belongs to.
     */
    uint32_t node_id;

    /**
     * Unique ID for this memory within the system.
     */
    uint32_t id;

    /**
     * Information on segments that can use this memory.
     */
    hsa_segment_t supported_segment_type_mask;

    /**
     * Base of the virtual address for this memory, if applicable.
     */
    uint64_t virtual_address_base;

    /**
     * Size.
     */
    uint64_t size_in_bytes;

    /**
     * Theoretical peak bandwidth in mega-bits per second to access this memory
     * from the agent/component.
     */
    uint64_t peak_bandwidth_mbps;
} hsa_memory_descriptor_t;

/**
 * @brief Cache descriptor.
 */
typedef struct hsa_cache_descriptor_s {
    /**
     * ID of the node this memory belongs to.
     */
    uint32_t node_id;
    /**
     * Unique ID for this cache within the system.
     */
    uint32_t id;

    /**
     * Number of levels of cache (for a multi-level cache).
     */
    uint8_t levels;

    /**
     * Associativity of this cache. The array has size @a levels. Associativity
     * is expressed as a power of two, where 1 means 'direct mapped', and 255
     * means 'full associative'. Zero is reserved.
     */
    uint8_t *associativity;

    /**
     * Size at each level. The array has size @a levels.
     */
    uint64_t *cache_size;

    /**
     * Cache line size at each level. The array has size @a levels.
     */
    uint64_t *cache_line_size;

    /**
     * Cache inclusivity with respect to the level above. The array has size @a
     * levels, where @a is_inclusive[@a levels - 1] is always zero.
     */
    uint8_t *is_inclusive;

} hsa_cache_descriptor_t;

/**
 * @brief Topology object type.
 */
typedef enum {
  /**
   * Agent object.
   */
  HSA_TOPOLOGY_OBJECT_AGENT = 1,
  /**
   * Memory object.
   */
  HSA_TOPOLOGY_OBJECT_MEMORY = 2,
  /**
   * Cache object.
   */
  HSA_TOPOLOGY_OBJECT_CACHE = 4
} hsa_topology_object_t;

/**
 * @brief Retrieve the identifiers of all the topology objects.
 *
 * @param[in] type Type of object affected by the query.
 *
 * @param[out] ids Pointer to a list containing the identifiers of all the
 * topology objects of type @a type.
 *
 * @param[out] num_ids Pointer to a memory location where the number of elements
 * in @a ids is to be stored.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is failure to allocate
 * the resources required by the implementation.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a ids is NULL or @a num_ids
 * is NULL.
 */
hsa_status_t hsa_topology_object_ids(hsa_topology_object_t type,
                                     uint32_t** ids,
                                     int* num_ids);

/**
 * @brief Retrieve the topology descriptor associated with a topology  object.
 *
 * @param[in] id Identifier of the topology object being queried.
 *
 * @param[in,out] object_descriptor User-allocated buffer where the descriptor
 * of the object will be copied to. The buffer pointed by @a object_descriptor
 * must be large enough to hold the descriptor for the object.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a object_descriptor is NULL.
 */
hsa_status_t hsa_topology_object_descriptor(uint32_t id,
                                     void* object_descriptor);

/** @} */


/** \defgroup signals TODO
 *  @{
 */

/**
 * @brief Signal handle.
 */
typedef uint64_t hsa_signal_handle_t;

/**
 * @brief Signal value. The value occupies 32 bits in small machine mode, and 64
 * bits in large machine mode.
 */
typedef uintptr_t hsa_signal_value_t;

/**
 * @brief Create a signal.
 *
 * @param[in] initial_value Initial value of the signal.
 *
 * @param[out] signal_handle Signal handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is failure to allocate
 * the resources required by the implementation.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a signal_handle is NULL.
 */
hsa_status_t HSA_API hsa_signal_create(hsa_signal_value_t initial_value,
                          hsa_signal_handle_t *signal_handle);

/**
 * @brief Destroy a signal previous created by ::hsa_signal_create.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_SIGNAL If @a signal_handle is invalid.
 */
hsa_status_t HSA_API hsa_signal_destroy(hsa_signal_handle_t signal_handle);

/**
 * @brief Read the current signal value.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[out] value Pointer to memory location where to store the signal value.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_SIGNAL If @a signal_handle is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a value is NULL.
 *
*/
hsa_status_t HSA_API hsa_signal_load_acquire(hsa_signal_handle_t signal_handle,
                          hsa_signal_value_t *value);

/**
 * @copydoc hsa_signal_load_acquire
 */
hsa_status_t HSA_API hsa_signal_load_relaxed(hsa_signal_handle_t signal_handle,
                          hsa_signal_value_t *value);
/**
 * @brief Set the value of a signal.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] value Value to be assigned to the signal handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_SIGNAL If @a signal_handle is invalid.
 */
hsa_status_t HSA_API hsa_signal_store_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_store_relaxed
 */
hsa_status_t HSA_API hsa_signal_store_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @brief Set the value of a signal and return its previous value.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[out] value Value to be placed at the signal
 *
 * @param[out] prev_value Pointer to the value of the signal prior to the
 * exchange.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_SIGNAL If @a signal_handle is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a prev_value is NULL.
 */
hsa_status_t HSA_API hsa_signal_exchange_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value,
                hsa_signal_value_t *prev_value);

/**
 * @copydoc hsa_signal_exchange_release
 */
hsa_status_t HSA_API hsa_signal_exchange_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value,
                hsa_signal_value_t *prev_value);

/**
 * @brief Perform a compare and swap on the value of a signal.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] expected The value to compare the handle's value with.
 *
 * @param[in] value The new value of the signal.
 *
 * @param[out] observed Pointer to memory location where to store the observed
 * value of the signal.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_SIGNAL If @a signal_handle is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a observed is NULL.
 */
hsa_status_t HSA_API hsa_signal_cas_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t expected,
                hsa_signal_value_t value,
                hsa_signal_value_t *observed);

/**
 * @brief Increment the value of a signal by a given amount. The
 * addition is atomic.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] value Value to add to the value of the signal handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_SIGNAL If @a signal_handle is invalid.
 */
hsa_status_t HSA_API hsa_signal_add_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_add_release
 */
hsa_status_t HSA_API hsa_signal_add_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @brief Decrement the value of a signal by a given amount.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] value Value to subtract from the value of the signal
 * handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_SIGNAL If @a signal_handle is invalid.
 */
hsa_status_t HSA_API hsa_signal_subtract_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_subtract_release
 */
hsa_status_t HSA_API hsa_signal_subtract_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @brief Perform a logical AND of the value of a signal and a given value.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] value Value to AND with the value of the signal handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_SIGNAL If @a signal_handle is invalid.
 */
hsa_status_t HSA_API hsa_signal_and_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_and_release
 */
hsa_status_t HSA_API hsa_signal_and_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @brief Perform a logical OR of the value of a signal and a given value.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] value Value to OR with the value of the signal handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_SIGNAL If @a signal_handle is invalid.
 */
hsa_status_t HSA_API hsa_signal_or_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_or_release
 */
hsa_status_t HSA_API hsa_signal_or_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @brief Perform a logical XOR of the value of a signal and a given value.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] value Value to XOR with the value of the signal handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_SIGNAL If @a signal_handle is invalid.
 */
hsa_status_t HSA_API hsa_signal_xor_release(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @copydoc hsa_signal_xor_release
 */
hsa_status_t HSA_API hsa_signal_xor_relaxed(hsa_signal_handle_t signal_handle,
                hsa_signal_value_t value);

/**
 * @brief Wait condition operator.
 */
typedef enum {
    /**
     * The two operands are equal.
     */
    HSA_EQ,
    /**
     * The two operands are not equal.
     */
    HSA_NE,
    /**
     * The first operand is less than the second operand.
     */
    HSA_LT,
    /**
     * The first operand is greater than or equal to the second operand.
     */
    HSA_GTE
} hsa_signal_condition_t;

/**
 * @brief Wait until the value of a signal satisfies a given condition.
 *
 * @details The wait may return before the condition is satisfied due to
 * multiple reasons. It is the user's burden to check the returned status before
 * consuming @a return_value.
 *
 * @param[in] signal_handle Signal handle.
 *
 * @param[in] condition Condition used to compare the signal value with @a
 * compare_value.
 *
 * @param[in] compare_value Value to compare with.
 *
 * @param[out] return_value Pointer to a memory location where the observed
 * value of @a signal_handle is written. If the function returns success, the
 * returned value must satisfy the passed condition. If the function returns any
 * other status, the implementation is not required to populate this value.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_SIGNAL If @a signal_handle is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_WAIT_ABANDONED If the wait has been abandoned (for
 * example, a spurious wakeup has occurred) before the condition is met.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a condition is not a valid
 * condition value, or @a return_value is NULL.
 *
*/
hsa_status_t HSA_API hsa_signal_wait_acquire(hsa_signal_handle_t signal_handle,
                          hsa_signal_condition_t condition,
                          hsa_signal_value_t compare_value,
                          hsa_signal_value_t *return_value);

/**
 * @copydoc hsa_signal_wait_acquire
 */
hsa_status_t HSA_API hsa_signal_wait_relaxed(hsa_signal_handle_t signal_handle,
                          hsa_signal_condition_t condition,
                          hsa_signal_value_t compare_value,
                          hsa_signal_value_t *return_value);

/**
 * @brief Wait until the value of a signal satisfies a given condition.
 *
 * @details The wait may return before the condition is satisfied due to
 * multiple reasons. It is the user's burden to check the returned status before
 * consuming @a return_value.
 *
 * The application might indicate a preference about the maximum wait duration,
 * which implementations can ignore.

 * @param[in] signal_handle Signal handle.
 *
 * @param[in] timeout Maximum wait duration hint. The operation might block for
 * a shorter or longer time even if the condition is not met. Specified in the
 * same unit as the system timestamp. A value of UINT64_MAX indicates no
 * maximum.
 *
 * @param[in] condition Condition used to compare the signal value with @a
 * compare_value.
 *
 * @param[in] compare_value Value to compare with.
 *
 * @param[out] return_value Pointer to a memory location where the observed
 * value of @a signal_handle is written. If the function returns success, the
 * returned value must satisfy the passed condition. If the function returns any
 * other status, the implementation is not required to populate this value.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_SIGNAL If @a signal_handle is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_WAIT_ABANDONED If the wait has been abandoned (for
 * example, it timed out or a spurious wakeup has occurred) before the condition
 * is met.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a condition is not a valid
 * condition value, or @a return_value is NULL.
 *
*/
hsa_status_t HSA_API hsa_signal_wait_timeout_acquire(hsa_signal_handle_t signal_handle,
                          uint64_t timeout,
                          hsa_signal_condition_t condition,
                          hsa_signal_value_t compare_value,
                          hsa_signal_value_t *return_value);

/**
 * @copydoc hsa_signal_wait_timeout_acquire
 */
hsa_status_t HSA_API hsa_signal_wait_timeout_relaxed(hsa_signal_handle_t signal_handle,
                          uint64_t timeout,
                          hsa_signal_condition_t condition,
                          hsa_signal_value_t compare_value,
                          hsa_signal_value_t *return_value);

/** @} */

/** \defgroup queue TODO
 *  @{
 */

/**
 * @brief Queue type. Intended to be used for dynamic queue protocol
 * determination.
 */
typedef enum {
  /**
   * Multiple producers are supported.
   */
  HSA_QUEUE_TYPE_MULTI = 0,
  /**
   * Only a single producer is supported.
   */
  HSA_QUEUE_TYPE_SINGLE = 1,
} hsa_queue_type_t;

/**
 * @brief Queue features.
 */
typedef enum {
  /**
   * Queue supports dispatch packets.
   */
  HSA_QUEUE_FEATURE_DISPATCH = 1,

  /**
   * Queue supports agent dispatch packets.
   */
  HSA_QUEUE_FEATURE_AGENT_DISPATCH = 2,
} hsa_queue_feature_t;

/**
 * @brief User mode queue.
 *
 * @details Queues are read-only, but HSA agents can directly modify the
 * contents of the buffer pointed by @a base_address, or use runtime APIs to
 * access the doorbell signal or the service queue.
 *
 */
typedef struct hsa_queue_s {
  /**
   * Queue type.
   */
  hsa_queue_type_t type;

  /**
   * Queue features mask. Applications should ignore any unknown set bits.
   */
  uint32_t features;

  /**
   * Starting address of the runtime-allocated buffer which is used to store the
   * AQL packets. Aligned to the size of an AQL packet.
   */
  uint64_t base_address;

  /**
   * Signal object used by the application to indicate the ID of a packet that
   * is ready to be processed.
   *
   * The HSA runtime is responsible for the life cycle of the doorbell signal:
   * replacing it with another signal or destroying it is not allowed and
   * results in undefined behavior.
   *
   * If @a type is ::HSA_QUEUE_TYPE_SINGLE, it is the application's
   * responsibility to update the doorbell signal value with monotonically
   * increasing indexes.
   */
  hsa_signal_handle_t doorbell_signal;

  /**
   * Maximum number of packets the queue can hold. Must be a power of two.
   */
  uint32_t size;

  /**
   * Queue identifier which is unique per process.
   */
  uint32_t id;

  /**
   * A pointer to another user mode queue that can be used by the HSAIL kernel
   * to request system services.
   */
  uint64_t service_queue;

} hsa_queue_t;

/**
 * @brief Create a user mode queue.
 *
 * @details When a queue is created, the runtime also allocates the packet
 * buffer and the completion signal. The application should only rely on the
 * error code returned to determine if the queue is valid.
 *
 * @param[in] component Pointer to the component on which this queue is to be
 * created.
 *
 * @param[in] size Number of packets the queue is expected to hold. Must be a
 * power of two.
 *
 * @param[in] type Type of the queue.
 *
 * @param[in] event_callback Callback to be invoked for events related with this
 * queue. Can be NULL.
 *
 * @param[in] service_queue Pointer to service queue to be associated with the
 * newly created queue. It might be NULL, or another previously created queue
 * that supports agent dispatch.
 *
 * @param[out] queue The queue structure, filled up and returned by the runtime.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is failure to allocate
 * the resources required by the implementation.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_COMPONENT If the component is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a component is NULL, @a size
 * is not a power of two, @a type is not a valid queue type, or @a queue is
 * NULL.
 *
 */
hsa_status_t HSA_API hsa_queue_create(const hsa_agent_t *component,
                              size_t size,
                              hsa_queue_type_t type,
                              hsa_event_callback_t event_callback,
                              hsa_queue_t* service_queue,
                              hsa_queue_t **queue);

/**
 * @brief Destroy a user mode queue.
 *
 * @details A destroyed queue might not be accessed after being destroyed. When
 * a queue is destroyed, the state of the AQL packets that have not been yet
 * fully processed becomes undefined.
 *
 * @param[in] queue Pointer to a queue.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_QUEUE If the queue is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a queue is NULL.
 */
hsa_status_t HSA_API hsa_queue_destroy(hsa_queue_t *queue);

/**
 * @brief Inactivate a queue.
 *
 * @details Inactivating the queue aborts any pending executions and prevent any
 * new packets from being processed. Any more packets written to the queue once
 * it is inactivated will be ignored by the packet processor.
 *
 * @param[in] queue Pointer to a queue.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_QUEUE If the queue is invalid.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a queue is NULL.
 */
hsa_status_t HSA_API hsa_queue_inactivate(hsa_queue_t *queue);

/**
 * @brief Retrieve the read index of a queue.
 *
 * @param[in] queue Pointer to a queue.
 *
 * @return Read index of the queue pointed by @a queue.
 */
uint64_t hsa_queue_load_read_index_relaxed(hsa_queue_t *queue);

/**
 * @copydoc hsa_queue_load_read_index_relaxed
 */
uint64_t hsa_queue_load_read_index_acquire(hsa_queue_t *queue);

/**
 * @brief Retrieve the write index of a queue.
 *
 * @param[in] queue Pointer to a queue.
 *
 * @return Write index of the queue pointed by @a queue.
 */
uint64_t hsa_queue_load_write_index_relaxed(hsa_queue_t *queue);

/**
 * @copydoc hsa_queue_load_write_index_relaxed
 */
uint64_t hsa_queue_load_write_index_acquire(hsa_queue_t *queue);

/**
 * @brief Set the write index of a queue.
 *
 * @param[in] queue Pointer to a queue.
 *
 * @param[in] value Value to assign to the write index.
 *
 */
void hsa_queue_store_write_index_relaxed(hsa_queue_t *queue, uint64_t value);

/**
 * @copydoc hsa_queue_store_write_index_relaxed
 */
void hsa_queue_store_write_index_release(hsa_queue_t *queue, uint64_t value);

/**
 * @brief Atomically compare and set the write index of a queue.
 *
 * @param[in] queue Pointer to a queue.
 *
 * @param[in] expected The expected index value.
 *
 * @param[in] value Value to assign to the write index if @a expected matches the
 * observed write index.
 *
 * @return Previous value of the write index.
 */
uint64_t hsa_queue_cas_write_index_relaxed(hsa_queue_t *queue,
    uint64_t expected,
    uint64_t value);

/**
 * @copydoc hsa_queue_cas_write_index_relaxed
 */
uint64_t hsa_queue_cas_write_index_release(hsa_queue_t *queue,
    uint64_t expected,
    uint64_t value);

/**
 * @copydoc hsa_queue_cas_write_index_relaxed
 */
uint64_t hsa_queue_cas_write_index_acquire(hsa_queue_t *queue,
    uint64_t expected,
    uint64_t value);

/**
 * @copydoc hsa_queue_cas_write_index_relaxed
 */
uint64_t hsa_queue_cas_write_index_acquire_release(hsa_queue_t *queue,
    uint64_t expected,
    uint64_t value);

/**
 * @brief Increment the write index of a queue by an offset.
 *
 * @param[in] queue Pointer to a queue.
 *
 * @param[in] value Value to add to the write index.
 *
 * @return Previous value of the write index.
 */
uint64_t hsa_queue_add_write_index_relaxed(hsa_queue_t *queue, uint64_t value);

/**
 * @copydoc hsa_queue_add_write_index_relaxed
 */
uint64_t hsa_queue_add_write_index_acquire(hsa_queue_t *queue, uint64_t value);

/**
 * @copydoc hsa_queue_add_write_index_relaxed
 */
uint64_t hsa_queue_add_write_index_release(hsa_queue_t *queue, uint64_t value);

/**
 * @copydoc hsa_queue_add_write_index_relaxed
 */
uint64_t hsa_queue_add_write_index_acquire_release(hsa_queue_t *queue,
  uint64_t value);

/**
 * @brief Set the read index of a queue.
 *
 * @param[in] queue Pointer to a queue.
 *
 * @param[in] value Value to assign to the read index.
 *
 */
void hsa_queue_store_read_index_relaxed(hsa_queue_t *queue, uint64_t value);

/**
 * @copydoc hsa_queue_store_read_index_relaxed
 */
void hsa_queue_store_read_index_release(hsa_queue_t *queue, uint64_t value);
/** @} */


/** \defgroup aql TODO
 *  @{
 */

/**
 * @brief Packet type.
 */
typedef enum {
  /**
   * Initial format of a packets when the queue is created. Always reserved
   * packet have never been assigned to the packet processor. From a functional
   * view always reserved packets are equivalent to invalid packets. All queues
   * support this packet format.
   */
  HSA_AQL_PACKET_FORMAT_ALWAYS_RESERVED = 0,
  /**
   * The packet slot has been processed in the past, and has not been reassigned
   * to the packet processor (is available). All queues support this packet
   * format.
   */
  HSA_AQL_PACKET_FORMAT_INVALID = 1,
  /**
   * Packet used by HSA agents for dispatching jobs to HSA components. Not all
   * queues support packets of this type (see ::hsa_queue_feature_t).
   */
  HSA_AQL_PACKET_FORMAT_DISPATCH = 2,
  /**
   * Packet used by HSA agents to delay processing of subsequent packets, and to
   * express complex dependencies between multiple packets. All queues support
   * this packet format.
   */
  HSA_AQL_PACKET_FORMAT_BARRIER = 3,
  /**
   * Packet used by HSA agents for dispatching jobs to HSA agents.  Not all
   * queues support packets of this type (see ::hsa_queue_feature_t).
   */
  HSA_AQL_PACKET_FORMAT_AGENT_DISPATCH = 4,
} hsa_aql_packet_format_t;

/**
 * @brief Scope of the memory fence operation associated with a packet.
 */
typedef enum {
  /**
   * No scope. Only valid for barrier packets.
   */
  HSA_FENCE_SCOPE_NONE = 0,
  /**
   * The fence is applied with component scope for the global segment.
   */
  HSA_FENCE_SCOPE_COMPONENT = 1,
  /**
   * The fence is applied with system scope for the global segment.
   */
  HSA_FENCE_SCOPE_SYSTEM = 2,
} hsa_fence_scope_t;

/**
 * @brief AQL packet header.
 */
typedef struct hsa_aql_packet_header_s {
  /**
   * Packet type.
   */
  hsa_aql_packet_format_t format:8;

  /**
   * If set, the processing of the current packet only launches when all 
   * preceding packets (within the same queue) are complete.
   */
  uint16_t barrier:1;

  /**
   * Determines the scope and type of the memory fence operation applied before
   * the packet enters the active phase.
   */
  hsa_fence_scope_t acquire_fence_scope:2;

  /**
   * Determines the scope and type of the memory fence operation applied after
   * kernel completion but before the packet is completed.
   */
  hsa_fence_scope_t release_fence_scope:2;

  /**
   * Must be a value of 0.
   */
  uint16_t reserved:3;

} hsa_aql_packet_header_t;

/**
 * @brief AQL dispatch packet
 */
typedef struct hsa_aql_dispatch_packet_s {
  /**
   *  Packet header.
   */
  hsa_aql_packet_header_t header;

  /**
   * Number of dimensions specified in the grid size. Valid values are 1, 2, or
   * 3.
   */
  uint16_t dimensions:2;

  /**
   * Reserved, must be a value of 0.
   */
  uint16_t reserved:14;

  /**
   * X dimension of work-group (measured in work-items).
   */
  uint16_t workgroup_size_x;

  /**
   * Y dimension of work-group (measured in work-items).
   */
  uint16_t workgroup_size_y;

  /**
   * Z dimension of work-group (measured in work-items).
   */
  uint16_t workgroup_size_z;

  /**
   * Reserved. Must be a value of 0.
   */
  uint16_t reserved2;

  /**
   * X dimension of grid (measured in work-items).
   */
  uint32_t grid_size_x;

  /**
   * Y dimension of grid (measured in work-items).
   */
  uint32_t grid_size_y;

  /**
   * Z dimension of grid (measured in work-items).
   */
  uint32_t grid_size_z;

  /**
   * Size (in bytes) of private memory allocation request (per work-item).
   */
  uint32_t private_segment_size_bytes;

  /**
   *  Size (in bytes) of group memory allocation request (per work-group).
   */
  uint32_t group_segment_size_bytes;

  /**
   *  Address of an object in memory that includes an implementation-defined
   *  executable ISA image for the kernel.
   */
  uint64_t kernel_object_address;

  /**
   * Address of memory containing kernel arguments.
   */
  uint64_t kernarg_address;

  /**
   * Reserved. Must be a value of 0.
   */
  uint64_t reserved3;

  /**
   * Signaling object handle used to indicate completion of the job.
   */
  hsa_signal_handle_t completion_signal;

} hsa_aql_dispatch_packet_t;

/**
 * @brief Agent dispatch packet.
 */
typedef struct hsa_aql_agent_dispatch_packet_s {
  /**
   *  Packet header.
   */
  hsa_aql_packet_header_t header;

  /**
   * The function to be performed by the destination HSA Agent. The type value
   * is split into the following ranges: 0x0000:0x3FFF (vendor specific),
   * 0x4000:0x7FFF (HSA runtime) 0x8000:0xFFFF (user registered function).
   */
  uint16_t type;

  /**
   * Reserved. Must be a value of 0.
   */
  uint32_t reserved2;

  /**
   * Pointer to location to store the function return value(s) in.
   */
  uint64_t return_location;

  /**
   * 64-bit direct or indirect arguments.
   */
  uint64_t arg[4];

  /**
   * Reserved. Must be a value of 0.
   */
  uint64_t reserved3;

  /**
   * Signaling object handle used to indicate completion of the job.
   */
  hsa_signal_handle_t completion_signal;

} hsa_aql_agent_dispatch_packet_t;

/**
 * @brief Barrier packet.
 */
typedef struct hsa_aql_barrier_packet_s {
  /**
   * Packet header.
   */
  hsa_aql_packet_header_t header;

  /**
   * Reserved. Must be a value of 0.
   */
  uint16_t reserved2;

  /**
   * Reserved. Must be a value of 0.
   */
  uint32_t reserved3;

  /**
   * Array of dependent signal objects.
   */
  hsa_signal_handle_t dep_signal[5];

  /**
   * Reserved. Must be a value of 0.
   */
  uint64_t reserved4;

  /**
   * Signaling object handle used to indicate completion of the job.
   */
  hsa_signal_handle_t completion_signal;

} hsa_aql_barrier_packet_t;

/** @} */

/** \defgroup memory TODO
 *  @{
 */
/**
 *
 * @brief Register memory.
 *
 * @details Registering a system memory region for use with all the available
 * devices This is an optional interface that is solely provided as a
 * performance optimization hint to the underlying implementation so it may
 * prepare for the future use of the memory by the devices. The interface is
 * only beneficial for system memory that will be directly accessed by a device.
 *
 * Overlapping registrations are allowed. This is neither detrimental nor
 * beneficial.
 *
 * @param[in] address A pointer to the base of the memory region to be
 * registered. If a null pointer is passed, no operation is performed.
 *
 * @param[in] size Requested registration size in bytes. If a size of zero is
 * passed, no operation is performed.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is a failure in
 * allocating the necessary resources.
 *
 */
hsa_status_t HSA_API hsa_memory_register(void *address,
    size_t size);

/**
 *
 * @brief Deregister memory.
 *
 * @details Used for deregistering a memory region previously registered.
 *
 * Deregistration must be performed using an address that was previously
 * registered.  In the event that deregistration is performed on an address that
 * has been used in multiple registrations, the smallest of the registrations is
 * deregistered.
 *
 * @param[in] address A pointer to the base of the memory region to be
 * deregistered. If a NULL pointer is passed, no operation is performed.
 *
 *  @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 *  @retval ::HSA_STATUS_ERROR_NOT_REGISTERED If the pointer has not been
 *  registered before.
 *
 */
hsa_status_t HSA_API hsa_memory_deregister(void *address);

/**
 * @brief Allocate system memory.
 *
 * @details The returned buffer is already registered. Allocation of size 0 is
 * allowed and returns a NULL pointer.

 * @param[in] size_bytes Allocation size.
 *
 * @param[in] address Address pointer allocated by the user. Dereferenced and
 * assigned to the pointer to the memory allocated for this request.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is a failure in
 * allocation. This error may also occur when the core runtime library needs to
 * spawn threads or create internal OS-specific events.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If the passed address is NULL.
 */
hsa_status_t HSA_API hsa_memory_allocate(size_t size_bytes, void **address);

/**
 * @brief Free system memory.
 *
 * @param[in] ptr Pointer to be released. If NULL, no action is performed
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 */
hsa_status_t HSA_API hsa_memory_free(void* ptr);

/**
 * @brief Allocate kernarg memory.
 *
 * @param[in] component A valid pointer to the component for which the specified
 * amount of kernarg memory is to be allocated.
 *
 * @param[in] size Requested allocation size in bytes. If size is 0, NULL is
 * returned.
 *
 * @param[out] address A valid pointer to the location of where to return the
 * pointer to the base of the allocated region of memory.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If the passed address is NULL.
 */
hsa_status_t HSA_API hsa_memory_allocate_kernarg( const hsa_agent_t *component,
    size_t size,
    void **address);

/**
 * @brief Free kernarg memory.
 *
 * @param[in] ptr Pointer to be released. If NULL, no action is performed
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 */
hsa_status_t HSA_API hsa_memory_free_kernarg(void* ptr);

/**
 * @brief Copy between the system and kernarg segments.
 *
 * @param[out] dst A valid pointer to the destination array where the content is
 *      to be copied.
 *
 * @param[in] src A valid pointer to the source of data to be copied.
 *
 * @param[in] size Number of bytes to copy.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If the source or destination
 * pointers are invalid.
 */
hsa_status_t HSA_API hsa_memory_copy_kernarg_to_system(void *dst,
    const void *src,
    size_t size);

/**
  * @copydoc hsa_memory_copy_kernarg_to_system
  */
hsa_status_t HSA_API hsa_memory_copy_system_to_kernarg(void *dst,
    const void *src,
    size_t size);

/**
 * @brief Allocate memory on HSA Device.
 *
 * @details Allocate global device memory associated with specified
 * device.
 *
 * @param[in] component A valid pointer to the HSA device for which the
 * specified amount of global memory is to be allocated.
 *
 * @param[in] size Requested allocation size in bytes. If size is 0, NULL is
 * returned.
 *
 * @param[out] address A valid pointer to the location of where to return the
 * pointer to the base of the allocated region of memory.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is a failure in
 * allocation of an internal structure required by the core runtime
 * library. This error may also occur when the core runtime library needs to
 * spawn threads or create internal OS-specific events.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If the passed component is NULL
 * or invalid, or if the passed pointer is NULL.
 */
hsa_status_t HSA_API hsa_memory_allocate_component_local( const hsa_agent_t *component,
    size_t size,
    void **address);

/**
 *
 * @brief Deallocate memory on HSA component.
 *
 * @details Deallocate component memory that was allocated with
 * ::hsa_memory_allocate_component_local.
 *
 * @param[in] address A pointer to the address to be deallocated. If the pointer
 * is NULL, no operation is performed.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 */
hsa_status_t HSA_API hsa_memory_free_component_local(void *address);

/**
 * @brief Copy between the system and local heaps.
 *
 * @param[out] dst A valid pointer to the destination array where the content is
 * to be copied.
 *
 * @param[in] src A valid pointer to the source of data to be copied.
 *
 * @param[in] size Number of bytes to copy.
 *
 * @param[in] signal The signal that will be incremented by the runtime when the
 * copy is complete.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If there is a failure in
 * allocation of an internal structure required by the core runtime
 * library. This error may also occur when the core runtime library needs to
 * spawn threads or create internal OS-specific events.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If any argument is invalid.
 */
hsa_status_t HSA_API hsa_memory_copy_component_local_to_system(void *dst,
    const void *src,
    size_t size,
    hsa_signal_handle_t signal);
/** @} */

/** \defgroup FinalizerCoreApi Finalizer Core API
 *  @{
 */

/**
 * @brief BrigProfile is used to specify the kind of profile. This controls what
 * features of HSAIL are supported. For more information see HSA Programmer's
 * Reference Manual.
 */
typedef uint8_t hsa_ext_brig_profile8_t;

/**
 * @brief BRIG profile values.
 */
typedef enum {
  /**
   * Base profile.
   */
  HSA_EXT_BRIG_PROFILE_BASE = 0,

  /**
   * Full profile.
   */
  HSA_EXT_BRIG_PROFILE_FULL = 1
} hsa_ext_brig_profile_t;

/**
 * @brief Machine model type. This controls the size of addresses used for
 * segment and flat addresses. For more information see HSA Programmer's
 * Reference Manual.
 */
typedef uint8_t hsa_ext_brig_machine_model8_t;

/**
 * @brief BRIG machine model.
 */
typedef enum {
  /**
   * Use 32 bit addresses for global segment and flat addresses.
   */
  HSA_EXT_BRIG_MACHINE_SMALL = 0,

  /**
   * Use 64 bit addresses for global segment and flat addresses.
   */
  HSA_EXT_BRIG_MACHINE_LARGE = 1
} hsa_ext_brig_machine_model_t;

/**
 * @brief BRIG section id. The index into the array of sections in a BRIG
 * module.
 */
typedef uint32_t hsa_ext_brig_section_id32_t;

/**
* @brief The fixed BRIG sections ID of the predefined BRIG sections.
*/
typedef enum {
  /**
   * Data section, containing all character strings and byte data used in the
   * finalization unit.
   */
  HSA_EXT_BRIG_SECTION_DATA = 0,

  /**
   * All of the executable operations. Most operations contain offsets to the
   * .operand section.
   */
  HSA_EXT_BRIG_SECTION_CODE = 1,

  /**
   * The operands, such as immediate constants, registers, and address
   * expressions, that appear in the operations.
   */
  HSA_EXT_BRIG_SECTION_OPERAND = 2
} hsa_ext_brig_section_id_t;

/**
 * @brief BRIG section header. The first entry in every section must be this
 * ::hsa_ext_brig_section_header_t structure.
 */
typedef struct hsa_ext_brig_section_header_s {
  /**
   * Size in bytes of the section.
   */
  uint32_t byte_count;

  /**
   * Size of the header in bytes.
   */
  uint32_t header_byte_count;

  /**
   * Length of @a name
   */
  uint32_t name_length;

  /**
   * Dynamically sized section name.
   */
  uint8_t name[1];
} hsa_ext_brig_section_header_t;

/**
 * @brief Top level BRIG module.
 */
typedef struct hsa_ext_brig_module_s {
  /**
   * Number of sections in this BRIG module.
   */
  uint32_t section_count;

  /**
   * Sections in this BRIG module.
   */
  hsa_ext_brig_section_header_t *section[1];
} hsa_ext_brig_module_t;

/**
 * @brief An opaque handle to the BRIG module.
 */
typedef struct hsa_ext_brig_module_handle_s {
  /**
   * HSA component specific handle to the brig module.
   */
  uint64_t handle;
} hsa_ext_brig_module_handle_t;

/**
 * @brief BRIG code section offset.
 */
typedef uint32_t hsa_ext_brig_code_section_offset32_t;

/**
 * @brief The set of exceptions supported by HSAIL. This is represented as a
 * bit set.
 */
typedef uint16_t hsa_ext_exception_kind16_t;

/**
 * @brief HSAIL exceptions.
 */
typedef enum {
  /**
   * Operations are performed on values for which the results are not defined.
   * These are:
   *   - Operations on signaling NaN (sNaN) floating-point values.
   *   - Signalling comparisons: comparisons on quiet NaN (qNaN)
   *     floating-point values.
   *   - Multiplication: mul(0.0, infinity) or mul(infinity, 0.0).
   *   - Fused multiply add: fma(0.0, infinity, c) or fma(infinity, 0.0, c)
   *     unless c is a quiet NaN, in which case it is implementation-defined
   *     if an exception is generated.
   *   - Addition, subtraction, or fused multiply add: magnitude subtraction
   *     of infinities, such as: add(positive infinity, negative infinity),
   *     sub(positive infinity, positive infinity).
   *   - Division: div(0.0, 0.0) or div(infinity, infinity).
   *   - Square root: sqrt(negative).
   *   - Conversion: A cvt with a floating-point source type, an integer
   *     destination type, and a nonsaturating rounding mode, when the source
   *     value is a NaN, infinity, or the rounded value, after any flush to
   *     zero, cannot be represented precisely in the integer type of the
   *     destination.
   */
  HSA_EXT_EXCEPTION_INVALID_OPERATION = 1,

  /**
   * A finite non-zero floating-point value is divided by zero. It is
   * implementation defined if integer div or rem operations with a divisor of
   * zero will generate a divide by zero exception.
   */
  HSA_EXT_EXCEPTION_DIVIDE_BY_ZERO = 2,

  /**
   * The floating-point exponent of a value is too large to be represented.
   */
  HSA_EXT_EXCEPTION_OVERFLOW = 4,

  /**
   * A non-zero tiny floating-point value is computed and either the ftz
   * modifier is specified, or the ftz modifier was not specified and the value
   * cannot be represented exactly.
   */
  HSA_EXT_EXCEPTION_UNDERFLOW = 8,

  /**
   * A computed floating-point value is not represented exactly in the
   * destination. This can occur due to rounding. In addition, it is
   * implementation defined if operations with the ftz modifier that cause a
   * value to be flushed to zero generate the inexact exception.
   */
  HSA_EXT_EXCEPTION_INEXACT = 16
} hsa_ext_exception_kind_t;

/**
 * @brief Bit set of control directives supported in HSAIL. See HSA Programmer's
 * Reference Manual description of control directives with the same name for
 * more information. For control directives that have an associated value, the
 * value is given by the field in hsa_ext_control_directives_t. For control
 * directives that are only present of absent (such as
 * requirenopartialworkgroups) they have no corresponding field as the presence
 * of the bit in this mask is sufficient.
 */
typedef uint64_t hsa_ext_control_directive_present64_t;

/**
 * @brief HSAIL control directives.
 */
typedef enum {
  /**
   * If not enabled then must be 0, otherwise must be non-0 and specifies the
   * set of HSAIL exceptions that must have the BREAK policy enabled.  If this
   * set is not empty then the generated code may have lower performance than if
   * the set is empty. If the kernel being finalized has any
   * enablebreakexceptions control directives, then the values specified by this
   * argument are unioned with the values in these control directives.  If any
   * of the functions the kernel calls have an enablebreakexceptions control
   * directive, then they must be equal or a subset of, this union.
   */
  HSA_EXT_CONTROL_DIRECTIVE_ENABLE_BREAK_EXCEPTIONS = 0,

  /**
   * If not enabled then must be 0, otherwise must be non-0 and specifies the
   * set of HSAIL exceptions that must have the DETECT policy enabled.  If this
   * set is not empty then the generated code may have lower performance than if
   * the set is empty. However, an implementation should endeavour to make the
   * performance impact small. If the kernel being finalized has any
   * enabledetectexceptions control directives, then the values specified by
   * this argument are unioned with the values in these control directives. If
   * any of the functions the kernel calls have an enabledetectexceptions
   * control directive, then they must be equal or a subset of, this union.
   */
  HSA_EXT_CONTROL_DIRECTIVE_ENABLE_DETECT_EXCEPTIONS = 1,

  /**
   * If not enabled then must be 0, and any amount of dynamic group segment can
   * be allocated for a dispatch, otherwise the value specifies the maximum
   * number of bytes of dynamic group segment that can be allocated for a
   * dispatch. If the kernel being finalized has any maxdynamicsize control
   * directives, then the values must be the same, and must be the same as this
   * argument if it is enabled. This value can be used by the finalizer to
   * determine the maximum number of bytes of group memory used by each
   * work-group by adding this value to the group memory required for all group
   * segment variables used by the kernel and all functions it calls, and group
   * memory used to implement other HSAIL features such as fbarriers and the
   * detect exception operations. This can allow the finalizer to determine the
   * expected number of work-groups that can be executed by a compute unit and
   * allow more resources to be allocated to the work-items if it is known that
   * fewer work-groups can be executed due to group memory limitations.
   */
  HSA_EXT_CONTROL_DIRECTIVE_MAX_DYNAMIC_GROUP_SIZE = 2,

  /**
   * If not enabled then must be 0, otherwise must be greater than 0. Specifies
   * the maximum number of work-items that will be in the grid when the kernel
   * is dispatched. For more information see HSA Programmer's Reference Manual.
   */
  HSA_EXT_CONTROL_DIRECTIVE_MAX_FLAT_GRID_SIZE = 4,

  /**
   * If not enabled then must be 0, otherwise must be greater than 0. Specifies
   * the maximum number of work-items that will be in the work-group when the
   * kernel is dispatched. For more information see HSA Programmer's Reference
   * Manual.
   */
  HSA_EXT_CONTROL_DIRECTIVE_MAX_FLAT_WORKGROUP_SIZE = 8,

  /**
   * If not enabled then must be 0, and the finalizer is free to generate ISA
   * that may result in any number of work-groups executing on a single compute
   * unit. Otherwise, the finalizer should attempt to generate ISA that will
   * allow the specified number of work-groups to execute on a single compute
   * unit. This is only a hint and can be ignored by the finalizer. If the
   * kernel being finalized, or any of the functions it calls, has a requested
   * control directive, then the values must be the same. This can be used to
   * determine the number of resources that should be allocated to a single
   * work-group and work-item. For example, a low value may allow more resources
   * to be allocated, resulting in higher per work-item performance, as it is
   * known there will never be more than the specified number of work-groups
   * actually executing on the compute unit.  Conversely, a high value may
   * allocate fewer resources, resulting in lower per work-item performance,
   * which is offset by the fact it allows more work-groups to actually execute
   * on the compute unit.
   */
  HSA_EXT_CONTROL_DIRECTIVE_REQUESTED_WORKGROUPS_PER_CU = 16,

  /**
   * If not enabled then all elements for Dim3 must be 0, otherwise every
   * element must be greater than 0. Specifies the grid size that will be used
   * when the kernel is dispatched. For more information see HSA Programmer's
   * Reference Manual.
   */
  HSA_EXT_CONTROL_DIRECTIVE_REQUIRED_GRID_SIZE = 32,

  /**
   * If not enabled then all elements for Dim3 must be 0, and the produced code
   * can be dispatched with any legal work-group range consistent with the
   * dispatch dimensions. Otherwise, the code produced must always be dispatched
   * with the specified work-group range. No element of the specified range must
   * be 0. It must be consistent with required_dimensions and
   * max_flat_workgroup_size. If the kernel being finalized, or any of the
   * functions it calls, has a requiredworkgroupsize control directive, then the
   * values must be the same. Specifying a value can allow the finalizer to
   * optimize work-group id operations, and if the number of work-items in the
   * work-group is less tha the WAVESIZE then barrier operations can be
   * optimized to just a memory fence.
   */
  HSA_EXT_CONTROL_DIRECTIVE_REQUIRED_WORKGROUP_SIZE = 64,

  /**
   * If not enabled then must be 0 and the produced kernel code can be
   * dispatched with 1, 2 or 3 dimensions. If enabled then the value is 1..3 and
   * the code produced must only be dispatched with a dimension that
   * matches. Other values are illegal. If the kernel being finalized, or any of
   * the functions it calls, has a requireddimsize control directive, then the
   * values must be the same. This can be used to optimize the code generated to
   * compute the absolute and flat work-group and work-item id, and the dim
   * HSAIL operations.
   */
  HSA_EXT_CONTROL_DIRECTIVE_REQUIRED_DIM = 128,

  /**
   * Specifies that the kernel must be dispatched with no partial work-groups.
   * It can be placed in either a kernel or a function code block. This is only
   * a hint and can be ignored by the finalizer.
   *
   * It is undefined if the kernel is dispatched with any dimension of the grid
   * size not being an exact multiple of the corresponding dimension of the
   * work-group size.
   *
   * A finalizer might be able to generate better code for currentworkgroupsize
   * if it knows there are no partial work-groups, because the result becomes
   * the same as the workgroupsize operation. An HSA component might be able to
   * dispatch a kernel more efficiently if it knows there are no partial
   * work-groups.
   *
   * The control directive applies to the whole kernel and all functions it
   * calls. It can appear multiple times in a kernel or function. If it appears
   * in a function (including external functions), then it must also appear in
   * all kernels that call that function (or have been specified when the
   * finalizer was invoked), either directly or indirectly.
   *
   * If require no partial work-groups is specified when the finalizer is
   * invoked, the kernel behaves as if the requirenopartialworkgroups control
   * directive has been specified.
   *
   * \note require_no_partial_work_groups does not have a field since having the
   * bit set in enabledControlDirectives indicates that the cpntrol directive is
   * present.
   */
  HSA_EXT_CONTROL_DIRECTIVE_REQUIRE_NO_PARTIAL_WORKGROUPS = 256
} hsa_ext_control_directive_present_t;

/**
 * @brief The hsa_ext_control_directives_t specifies the values for the HSAIL
 * control directives. These control how the finalizer generates code. This
 * struct is used both as an argument to hsaFinalizeKernel to specify values for
 * the control directives, and is used in HsaKernelCode to record the values of
 * the control directives that the finalize used when generating the code which
 * either came from the finalizer argument or explicit HSAIL control
 * directives. See the definition of the control directives in HSA Programmer's
 * Reference Manual which also defines how the values specified as finalizer
 * arguments have to agree with the control directives in the HSAIL code.
 */
typedef struct hsa_ext_control_directives_s {
  /**
   * This is a bit set indicating which control directives have been
   * specified. If the value is 0 then there are no control directives specified
   * and the rest of the fields can be ignored. The bits are accessed using the
   * hsa_ext_control_directives_present_mask_t. Any control directive that is not
   * enabled in this bit set must have the value of all 0s.
   */
  hsa_ext_control_directive_present64_t enabled_control_directives;

  /**
   * If enableBreakExceptions is not enabled then must be 0, otherwise must be
   * non-0 and specifies the set of HSAIL exceptions that must have the BREAK
   * policy enabled. If this set is not empty then the generated code may have
   * lower performance than if the set is empty. If the kernel being finalized
   * has any enablebreakexceptions control directives, then the values specified
   * by this argument are unioned with the values in these control
   * directives. If any of the functions the kernel calls have an
   * enablebreakexceptions control directive, then they must be equal or a
   * subset of, this union.
   */
  hsa_ext_exception_kind16_t enable_break_exceptions;

  /**
   * If enableDetectExceptions is not enabled then must be 0, otherwise must be
   * non-0 and specifies the set of HSAIL exceptions that must have the DETECT
   * policy enabled. If this set is not empty then the generated code may have
   * lower performance than if the set is empty. However, an implementation
   * should endeavour to make the performance impact small. If the kernel being
   * finalized has any enabledetectexceptions control directives, then the
   * values specified by this argument are unioned with the values in these
   * control directives. If any of the functions the kernel calls have an
   * enabledetectexceptions control directive, then they must be equal or a
   * subset of, this union.
   */
  hsa_ext_exception_kind16_t enable_detect_exceptions;

  /**
   * If maxDynamicGroupSize is not enabled then must be 0, and any amount of
   * dynamic group segment can be allocated for a dispatch, otherwise the value
   * specifies the maximum number of bytes of dynamic group segment that can be
   * allocated for a dispatch. If the kernel being finalized has any
   * maxdynamicsize control directives, then the values must be the same, and
   * must be the same as this argument if it is enabled. This value can be used
   * by the finalizer to determine the maximum number of bytes of group memory
   * used by each work-group by adding this value to the group memory required
   * for all group segment variables used by the kernel and all functions it
   * calls, and group memory used to implement other HSAIL features such as
   * fbarriers and the detect exception operations. This can allow the finalizer
   * to determine the expected number of work-groups that can be executed by a
   * compute unit and allow more resources to be allocated to the work-items if
   * it is known that fewer work-groups can be executed due to group memory
   * limitations.
   */
  uint32_t max_dynamic_group_size;

  /**
   * If maxFlatGridSize is not enabled then must be 0, otherwise must be greater
   * than 0. See HSA Programmer's Reference Manual description of
   * maxflatgridsize control directive.
   */
  uint32_t max_flat_grid_size;

  /**
   * If maxFlatWorkgroupSize is not enabled then must be 0, otherwise must be
   * greater than 0. See HSA Programmer's Reference Manual description of
   * maxflatworkgroupsize control directive.
   */
  uint32_t max_flat_workgroup_size;

  /**
   * If requestedWorkgroupsPerCu is not enabled then must be 0, and the
   * finalizer is free to generate ISA that may result in any number of
   * work-groups executing on a single compute unit. Otherwise, the finalizer
   * should attempt to generate ISA that will allow the specified number of
   * work-groups to execute on a single compute unit. This is only a hint and
   * can be ignored by the finalizer. If the kernel being finalized, or any of
   * the functions it calls, has a requested control directive, then the values
   * must be the same. This can be used to determine the number of resources
   * that should be allocated to a single work-group and work-item. For example,
   * a low value may allow more resources to be allocated, resulting in higher
   * per work-item performance, as it is known there will never be more than the
   * specified number of work-groups actually executing on the compute
   * unit. Conversely, a high value may allocate fewer resources, resulting in
   * lower per work-item performance, which is offset by the fact it allows more
   * work-groups to actually execute on the compute unit.
   */
  uint32_t requested_workgroups_per_cu;

  /**
   * If not enabled then all elements for Dim3 must be 0, otherwise every
   * element must be greater than 0. See HSA Programmer's Reference Manual
   * description of requiredgridsize control directive.
   */
  hsa_dim3_t required_grid_size;

  /**
   * If requiredWorkgroupSize is not enabled then all elements for Dim3 must be
   * 0, and the produced code can be dispatched with any legal work-group range
   * consistent with the dispatch dimensions. Otherwise, the code produced must
   * always be dispatched with the specified work-group range. No element of the
   * specified range must be 0. It must be consistent with required_dimensions
   * and max_flat_workgroup_size. If the kernel being finalized, or any of the
   * functions it calls, has a requiredworkgroupsize control directive, then the
   * values must be the same. Specifying a value can allow the finalizer to
   * optimize work-group id operations, and if the number of work-items in the
   * work-group is less than the WAVESIZE then barrier operations can be
   * optimized to just a memory fence.
   */
  hsa_dim3_t required_workgroup_size;

  /**
   * If requiredDim is not enabled then must be 0 and the produced kernel code
   * can be dispatched with 1, 2 or 3 dimensions. If enabled then the value is
   * 1..3 and the code produced must only be dispatched with a dimension that
   * matches. Other values are illegal. If the kernel being finalized, or any of
   * the functions it calls, has a requireddimsize control directive, then the
   * values must be the same. This can be used to optimize the code generated to
   * compute the absolute and flat work-group and work-item id, and the dim
   * HSAIL operations.
   */
  uint8_t required_dim;

  /**
   * Reserved. Must be 0.
   */
  uint8_t reserved[75];
} hsa_ext_control_directives_t;

/**
 * @brief The kinds of code objects that can be contained in
 * ::hsa_ext_code_descriptor_t.
 */
typedef uint32_t hsa_ext_code_kind32_t;

/**
 * @brief Type of code object.
 */
typedef enum {
  /**
   * Not a code object.
   */
  HSA_EXT_CODE_NONE = 0,

  /**
   * HSAIL kernel that can be used with an AQL dispatch packet.
   */
  HSA_EXT_CODE_KERNEL = 1,

  /**
   * HSAIL indirect function.
   */
  HSA_EXT_CODE_INDIRECT_FUNCTION = 2,

  /**
   * HSA runtime code objects. For example, partially linked code objects.
   */
  HSA_EXT_CODE_RUNTIME_FIRST = 0x40000000,
  HSA_EXT_CODE_RUNTIME_LAST = 0x7fffffff,

  /**
   * Vendor specific code objects.
   */
  HSA_EXT_CODE_VENDOR_FIRST = 0x80000000,
  HSA_EXT_CODE_VENDOR_LAST = 0xffffffff,
} hsa_ext_code_kind_t;

/**
 * @brief Program call convention.
 */
typedef uint32_t hsa_ext_program_call_convention_id32_t;

/**
 * @brief Types of program call conventions.
 */
typedef enum {
  HSA_EXT_PROGRAM_CALL_CONVENTION_FINALIZER_DETERMINED = -1
} hsa_ext_program_call_convention_id_t;

/**
 * @brief An opaque handle to the code object.
 */
typedef struct hsa_ext_code_handle_s {
  /**
   * HSA component specific handle to the code.
   */
  uint64_t handle;
} hsa_ext_code_handle_t;

/**
 * @brief An opaque handle to the debug information.
 */
typedef struct hsa_ext_debug_information_handle_s {
  /**
   * HSA component specific handle to the debug information.
   */
  uint64_t handle;
} hsa_ext_debug_information_handle_t;

/**
 * @brief ::hsa_ext_code_descriptor_t is the descriptor for the code object
 * produced by the Finalizer and contains information that applies to all code
 * entities in the program.
 */
typedef struct hsa_ext_code_descriptor_s {
  /**
   * Type of code object.
   */
  hsa_ext_code_kind32_t code_type;

  /**
   * The amount of group segment memory required by a work-group in bytes. This
   * does not include any dynamically allocated group segment memory that may be
   * added when the kernel is dispatched.
   */
  uint32_t workgroup_group_segment_byte_size;

  /**
   * The size in bytes of the kernarg segment that holds the values of the
   * arguments to the kernel.
   */
  uint64_t kernarg_segment_byte_size;

  /**
   * The amount of memory required for the combined private, spill and arg
   * segments for a work-item in bytes.
   */
  uint32_t workitem_private_segment_byte_size;

  /**
   * Number of fbarrier's used in the kernel and all functions it calls. If the
   * implementation uses group memory to allocate the fbarriers then that amount
   * must already be included in the workgroupGroupSegmentByteSize total.
   */
  uint32_t workgroup_fbarrier_count;

  /**
   * Opaque handle to code object.
   */
  hsa_ext_code_handle_t code;

  /**
   * The maximum byte alignment of variables used by the kernel in the kernarg
   * memory segment. Expressed as a power of two. Must be at least
   * HSA_POWERTWO_16
   */
  hsa_powertwo8_t kernarg_segment_alignment;

  /**
   * The maximum byte alignment of variables used by the kernel in the group
   * memory segment. Expressed as a power of two. Must be at least
   * HSA_POWERTWO_16
   */
  hsa_powertwo8_t group_segment_alignment;

  /**
   * The maximum byte alignment of variables used by the kernel in the private
   * memory segment. Expressed as a power of two. Must be at least
   * HSA_POWERTWO_16
   */
  hsa_powertwo8_t private_segment_alignment;

  /**
   * Wavefront size expressed as a power of two. Must be a power of 2 in range
   * 1..64 inclusive. Used to support runtime query that obtains wavefront size,
   * which may be used by application to allocated dynamic group memory and set
   * the dispatch work-group size.
   */
  hsa_powertwo8_t wavefront_size;

  /**
   * Program call convention id this code descriptor holds.
   */
  hsa_ext_program_call_convention_id32_t program_call_convention;

  /**
   * BRIG module handle this code descriptor associated with.
   */
  hsa_ext_brig_module_handle_t module;

  /**
   * BRIG directive offset this code descriptor associated with.
   */
  hsa_ext_brig_code_section_offset32_t symbol;

  /**
   * The HSAIL profile defines which features are used. This information is from
   * the HSAIL version directive. If this ::hsa_ext_code_descriptor_t is not
   * generated from an ::hsa_ext_finalize then must still indicate what
   * profile is being used.
   */
  hsa_ext_brig_profile8_t hsail_profile;

  /**
   * The HSAIL machine model gives the address sizes used by the code. This
   * information is from the HSAIL version directive. If this
   * ::hsa_ext_code_descriptor_t is not generated from an ::hsa_ext_finalize
   * then must still indicate for what machine mode the code is generated.
   */
  hsa_ext_brig_machine_model8_t hsail_machine_model;

  /**
   * Reserved for BRIG target options if any are defined in the future. Must
   * be 0.
   */
  uint16_t reserved1;

  /**
   * Opaque handle to debug information.
   */
  hsa_ext_debug_information_handle_t debug_information;

  /**
   * The vendor of the HSA Component on which this Kernel Code object can
   * execute. ISO/IEC 624 character encoding must be used. If the
   * name is less than 24 characters then remaining characters must
   * be set to 0.
   */
  char agent_vendor[24];

  /**
   * The vendor's name of the HSA Component on which this Kernel Code object can
   * execute. ISO/IEC 624 character encoding must be used. If the name is less
   * than 24 characters then remaining characters must be set to 0.
   */
  char agent_name[24];

  /**
   * The HSAIL major version. This information is from the HSAIL version
   * directive. If this ::hsa_ext_code_descriptor_t is not generated from an
   * ::hsa_ext_finalize then must be 0.
   */
  uint32_t hsail_version_major;

  /**
   * The HSAIL minor version. This information is from the HSAIL version
   * directive. If this ::hsa_ext_code_descriptor_t is not generated from an
   * ::hsa_ext_finalize then must be 0.
   */
  uint32_t hsail_version_minor;

  /**
   * Reserved. Must be 0.
   */
  uint64_t reserved2;

  /**
   * The values should be the actually values used by the finalizer in
   * generating the code. This may be the union of values specified as finalizer
   * arguments and explicit HSAIL control directives. If the finalizer chooses
   * to ignore a control directive, and not generate constrained code, then the
   * control directive should not be marked as enabled even though it was
   * present in the HSAIL or finalizer argument. The values are intended to
   * reflect the constraints that the code actually requires to correctly
   * execute, not the values that were actually specified at finalize time.
   */
  hsa_ext_control_directives_t control_directive;
} hsa_ext_code_descriptor_t;

/**
 * @brief Finalization request. Contains ::hsa_ext_brig_module_handle_t which
 * points to the ::hsa_ext_brig_module_t to be finalized, as well as the desired
 * call convention to use when finalizing given BRIG module.
 */
typedef struct hsa_ext_finalization_request_s {
  /**
   * Handle to the ::hsa_ext_brig_module_t, which needs to be finalized.
   */
  hsa_ext_brig_module_handle_t module;
  /**
   * BRIG code section offset.
   */
  hsa_ext_brig_code_section_offset32_t symbol;
  /**
   * Desired program call convention.
   */
  hsa_ext_program_call_convention_id32_t program_call_convention;
} hsa_ext_finalization_request_t;

/**
 * @brief Finalization descriptor is the descriptor for the code object
 * produced by the Finalizer and contains information that applies to all code
 * entities in the program.
 */
typedef struct hsa_ext_finalization_descriptor_s {
  /**
   * Number of code descriptors produced.
   */
  uint32_t code_descriptor_count;
  /**
   * Reserved. Must be 0.
   */
  uint32_t reserved1;
  /**
   * Dynamically sized array of code descriptors.
   */
  hsa_ext_code_descriptor_t code_descriptors[1];
} hsa_ext_finalization_descriptor_t;

/**
 * @brief Call back function to get the definition of a module scope
 * variable/fbarrier or kernel/function.
 */
typedef hsa_status_t (*hsa_ext_symbol_definition_callback_t)(
  hsa_runtime_caller_t caller,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  hsa_ext_brig_module_handle_t *definition_module,
  hsa_ext_brig_module_t *definition_module_brig,
  hsa_ext_brig_code_section_offset32_t *definition_symbol);

/**
 * @brief Call back function to get the address of global segment variables,
 * kernel table variable, indirect function table variable.
 */
typedef hsa_status_t (*hsa_ext_symbol_address_callback_t)(
  hsa_runtime_caller_t caller,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  uint64_t *symbol_address);

/**
 * @brief Call back function to get the string representation of the error
 * message.
 */
typedef hsa_status_t (*hsa_ext_error_message_callback_t)(
  hsa_runtime_caller_t caller,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t statement,
  uint32_t indent_level,
  const char *message);

/**
 * @brief Finalizes provided BRIG modules.
 *
 * @details Invokes the finalizer on the provided list of kernels and indirect
 * functions that are in HSAIL modules in HSAIL program.
 *
 * @param[in] caller Opaque pointer which is passed to all call back functions
 * made by this call of the finalizer.
 *
 * @param[in] agent The HSA agent for which code must be produced.
 *
 * @param[in] program_agent_id Program agent id.
 *
 * @param[in] program_agent_count Number of program agents.
 *
 * @param[in] finalization_request_count The number of kernels and indirect
 * functions that are in HSAIL modules in HSAIL program.
 *
 * @param[in] finalization_request_list List of kernels and indirect functions
 * that are in HSAIL modules in HSAIL program.
 *
 * @param[in] control_directives The control directives that can be specified to
 * influence how the finalizer generates code. If NULL then no control
 * directives are used. If this call is successful and control_directives is not
 * NULL, then the resulting hsa_ext_code_descriptor_t object will have control
 * directives which were used by the finalizer.
 *
 * @param[in] symbol_definition_callback Call back function to get the
 * definition of a module scope variable/fbarrier or kernel/function.
 *
 * @param[in] symbol_address_callback Call back function to get the address of
 * global segment variables, kernel table variables, indirect function table
 * variable.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message.
 *
 * @param[in] optimization_level An implementation defined value that control
 * the level of optimization performed by the finalizer.
 *
 * @param[in] options Implementation defined options that can be specified to
 * the finalizer.
 *
 * @param[in] debug_information The flag for including/excluding the debug
 * information for @a finalization_descriptor. 0 - exclude debug information,
 * 1 - include debug information.
 *
 * @param[out] finalization_descriptor the descriptor for the code object
 * produced by the Finalizer and contains information that applies to all code
 * entities in the program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_DIRECTIVE_MISMATCH If the directive in the control
 * directive structure and in the HSAIL kernel mismatch or if the same directive
 * is used with a different value in one of the functions used by this kernel.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a finalization_request_list
 * is NULL or invalid.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If the finalize API cannot
 * allocate memory for @a finalization_descriptor.
 *
 * @retval ::HSA_EXT_STATUS_INFO_UNRECOGNIZED_OPTIONS If the options are not
 * recognized, no error is returned, just an info status is used to indicate
 * invalid options.
 */
hsa_status_t HSA_API hsa_ext_finalize(
  hsa_runtime_caller_t caller,
  hsa_agent_t *agent,
  uint32_t program_agent_id,
  uint32_t program_agent_count,
  size_t finalization_request_count,
  hsa_ext_finalization_request_t *finalization_request_list,
  hsa_ext_control_directives_t *control_directives,
  hsa_ext_symbol_definition_callback_t symbol_definition_callback,
  hsa_ext_symbol_address_callback_t symbol_address_callback,
  hsa_ext_error_message_callback_t error_message_callback,
  uint8_t optimization_level,
  const char *options,
  int debug_information,
  hsa_ext_finalization_descriptor_t **finalization_descriptor);

/**
 * @brief Destroys the finalization descriptor.
 *
 *
 * @param[in] finalization_descriptor A pointer to the finalization descriptor
 * that needs to be destroyed.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a finalization_descriptor is
 * NULL or does not point to a valid finalization descriptor object.
 *
 * @retval ::HSA_STATUS_ERROR_RESOURCE_FREE If some of the resources consumed
 * during initialization by the runtime could not be freed.
 */
hsa_status_t HSA_API hsa_ext_destroy_finalization_descriptor(
  hsa_ext_finalization_descriptor_t *finalization_descriptor);

/**
 * @brief Serializes the finalization descriptor.
 *
 * @details Serializes finalization descriptor for specified @a agent. The
 * caller can set @a debug_information to 1 in order to include debug
 * information of this finalization descriptor in the serialized object.
 *
 * @param[in] caller Opaque pointer and will be passed to all call back
 * functions made by this call.
 *
 * @param[in] agent The HSA agent for which @a finalization_descriptor must be
 * serialized.
 *
 * @param[in] finalization_descriptor Finalization descriptor to serialize.
 *
 * @param[in] alloc_serialize_data_callback Call back function for allocation.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message.
 *
 * @param[in] debug_information The flag for including/excluding the debug
 * information for @a finalization_descriptor. 0 - exclude debug information,
 * 1 - include debug information.
 *
 * @param[out] serialized_object Pointer to the serialized object.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a finalization_descriptor
 * is either NULL or does not point to a valid finalization descriptor object.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If no memory can be allocated
 * for @a serialized_object.
 */
hsa_status_t HSA_API hsa_ext_serialize_finalization_descriptor(
  hsa_runtime_caller_t caller,
  hsa_agent_t *agent,
  hsa_ext_finalization_descriptor_t *finalization_descriptor,
  hsa_runtime_alloc_data_callback_t alloc_serialize_data_callback,
  hsa_ext_error_message_callback_t error_message_callback,
  int debug_information,
  void *serialized_object);

/**
 * @brief Deserializes the finalization descriptor.
 *
 * @details Deserializes finalization descriptor for specified @a agent. The
 * caller can set @a debug_information to 1 in order to include debug
 * information of this finalization descriptor from the serialized object.
 *
 * @param[in] caller Opaque pointer and will be passed to all call back
 * functions made by this call.
 *
 * @param[in] serialized_object Serialized object to be deserialized.
 *
 * @param[in] agent The HSA agent for which @a finalization_descriptor must be
 * serialized.
 *
 * @param[in] program_agent_id TODO.
 *
 * @param[in] program_agent_count TODO.
 *
 * @param[in] symbol_address_callback Call back function to get the address of
 * global segment variables, kernel table variables, indirect function table
 * variable.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message.
 *
 * @param[in] debug_information The flag for including/excluding the debug
 * information for @a finalization_descriptor. 0 - exclude debug information,
 * 1 - include debug information.
 *
 * @param[out] finalization_descriptor Deserialized finalization descriptor.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a serialized_object is
 * either NULL, or is not valid, or the size is 0.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If no memory can be allocated
 * for @a finalization_descriptor.
 */
hsa_status_t HSA_API hsa_ext_deserialize_finalization_descriptor(
  hsa_runtime_caller_t caller,
  void *serialized_object,
  hsa_agent_t *agent,
  uint32_t program_agent_id,
  uint32_t program_agent_count,
  hsa_ext_symbol_address_callback_t symbol_address_callback,
  hsa_ext_error_message_callback_t error_message_callback,
  int debug_information,
  hsa_ext_finalization_descriptor_t **finalization_descriptor);
/** @} */


/** \defgroup HsailLinkerServiceLayer HSAIL Linker Service Layer
 *  @{
 */

/**
 * @brief An opaque handle to the HSAIL program. Created by
 * ::hsa_ext_program_create, and destroyed by ::hsa_ext_program_destroy.
 */
typedef struct hsa_ext_program_handle_s {
  /**
   * HSA component specific handle to the program.
   */
  uint64_t handle;
} hsa_ext_program_handle_t;

/**
 * @brief Creates an HSAIL program.
 *
 * @details Creates an HSAIL program for specified @a agent_count of @a agents,
 * with specified BRIG machine model @a machine_model and BRIG profile @a
 * profile. Returns a handle to the created HSAIL program, and ::hsa_status_t,
 * which describes the status of execution of this function.  There should be at
 * least one agent specified, and @a machine_model and @a profile have to be
 * valid ::hsa_ext_brig_profile_t and ::hsa_ext_brig_machine_model_t, otherwise
 * returns ::HSA_STATUS_ERROR_INVALID_ARGUMENT. If the program handle @a program
 * is already a valid program, ::HSA_EXT_STATUS_INFO_ALREADY_INITIALIZED is
 * returned.
 *
 * @param[in] agents One or more HSA agent for which this HSAIL program is
 * created.
 *
 * @param[in] agent_count Number of HSA agents for which this HSAIL program is
 * created.
 *
 * @param[in] machine_model The kind of machine model this HSAIL program is
 * created for.
 *
 * @param[in] profile The kind of profile this HSAIL program is created for.
 *
 * @param[out] program A valid pointer to a program handle for the HSAIL
 * program created.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a agent is NULL, or not
 * valid. If @a agent_count is 0. If @a machine_model is not valid. If
 * @a profile is not valid.
 *
 * @retval ::HSA_EXT_STATUS_INFO_ALREADY_INITIALIZED If @a program is already a
 * valid program.
 */
hsa_status_t HSA_API hsa_ext_program_create(
  hsa_agent_t *agents,
  uint32_t agent_count,
  hsa_ext_brig_machine_model8_t machine_model,
  hsa_ext_brig_profile8_t profile,
  hsa_ext_program_handle_t *program);

/**
 * @brief Destroys an HSAIL program.
 *
 * @details Destroys an HSAIL program pointed to by program handle @a program.
 * Returns ::hsa_status_t, which describes the status of execution of this
 * function. HSAIL program handle @a program has to be a valid
 * ::hsa_ext_program_handle_t object, otherwise ::HSA_STATUS_ERROR_INVALID_ARGUMENT
 * is returned. If the program handle @a program is already destroyed or has
 * never been created ::HSA_STATUS_ERROR_RESOURCE_FREE is returned.
 *
 * @param[in] program Program handle for the HSAIL program to be destroyed.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a program is not a valid
 * ::hsa_ext_program_handle_t object.
 *
 * @retval ::HSA_STATUS_ERROR_RESOURCE_FREE If @a program is already destroyed
 * or has never been created.
 */
hsa_status_t HSA_API hsa_ext_program_destroy(
  hsa_ext_program_handle_t program);

/**
 * @brief Adds an existing BRIG module to an existing HSAIL program.
 *
 * @param[in] program Program handle for the HSAIL program.
 *
 * @param[in] brig_module BRIG module to add to the HSAIL program.
 *
 * @param[out] module The handle for the @a brig_module.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 */
hsa_status_t HSA_API hsa_ext_add_module(
  hsa_ext_program_handle_t program,
  hsa_ext_brig_module_t *brig_module,
  hsa_ext_brig_module_handle_t *module);

/**
 * @brief Finalizes provided BRIG modules.
 *
 * @details Provides and services call backs to core Finalizer to manage looking
 * up global segment variable allocation and variable/function/fbarrier
 * definitions. Takes the result of core Finalizer and updates kernel and
 * indirect function table variables. Done as atomic store release to system
 * scope so ldi_acq and ldk_acq can synchronize with the update. Other query
 * operations must be used to get code address of kernels/indirect functions
 * finalized.
 *
 * @param[in] program Handle to the program.
 *
 * @param[in] agent The HSA agent for which code must be produced.
 *
 * @param[in] finalization_request_count The number of kernels and indirect
 * functions that are in HSAIL modules in HSAIL program.
 *
 * @param[in] finalization_request_list List of kernels and indirect functions
 * that are in HSAIL modules in HSAIL program.
 *
 * @param[in] control_directives The control directives that can be specified to
 * influence how the finalizer generates code. If NULL then no control
 * directives are used. If this call is successful and control_directives is not
 * NULL, then the resulting hsa_ext_code_descriptor_t object will have control
 * directives which were used by the finalizer.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message.
 *
 * @param[in] optimization_level An implementation defined value that control
 * the level of optimization performed by the finalizer.
 *
 * @param[in] options Implementation defined options that can be specified to
 * the finalizer.
 *
 * @param[in] debug_information The flag for including/excluding the debug
 * information for @a finalization_descriptor. 0 - exclude debug information,
 * 1 - include debug information.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_DIRECTIVE_MISMATCH If the directive in the control
 * directive structure and in the HSAIL kernel mismatch or if the same directive
 * is used with a different value in one of the functions used by this kernel.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a finalization_request_list
 * is NULL or invalid.
 *
 * @retval ::HSA_EXT_STATUS_INFO_UNRECOGNIZED_OPTIONS If the options are not
 * recognized, no error is returned, just an info status is used to indicate
 * invalid options.
 */
hsa_status_t HSA_API hsa_ext_finalize_program(
  hsa_ext_program_handle_t program,
  hsa_agent_t *agent,
  size_t finalization_request_count,
  hsa_ext_finalization_request_t *finalization_request_list,
  hsa_ext_control_directives_t *control_directives,
  hsa_ext_error_message_callback_t error_message_callback,
  uint8_t optimization_level,
  const char *options,
  int debug_information);

/**
 * @brief Queries program agent's id.
 *
 * @param[in] program Program to query agent's id from.
 *
 * @param[in] agent Agent to query agent's id from.
 *
 * @param[out] program_agent_id Program agent's id.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program or
 * @a agent is invalid.
 */
hsa_status_t HSA_API hsa_ext_query_program_agent_id(
  hsa_ext_program_handle_t program,
  hsa_agent_t *agent,
  uint32_t *program_agent_id);

/**
 * @brief Queries program agent count.
 *
 * @param[in] program Program to query agent count from.
 *
 * @param[out] program_agent_count Number of agents in the program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * invalid.
 */
hsa_status_t HSA_API hsa_ext_query_program_agent_count(
  hsa_ext_program_handle_t program,
  uint32_t *program_agent_count);

/**
 * @brief Queries program agents.
 *
 * @details Queries @a program_agent_count number of agents.
 *
 * @param[in] program Program to query agents from.
 *
 * @param[in] program_agent_count Number of agents to query.
 *
 * @param[out] agents HSA program agents.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * invalid.
 */
hsa_status_t HSA_API hsa_ext_query_program_agents(
  hsa_ext_program_handle_t program,
  uint32_t program_agent_count,
  hsa_agent_t *agents);

/**
 * @brief Queries program module count.
 *
 * @param[in] program Program to query module count from.
 *
 * @param[out] program_module_count Number of modules in the program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * invalid.
 */
hsa_status_t HSA_API hsa_ext_query_program_module_count(
  hsa_ext_program_handle_t program,
  uint32_t *program_module_count);

/**
 * @brief Queries program modules.
 *
 * @details Queries @a program_module_count number of modules.
 *
 * @param[in] program Program to query modules from.
 *
 * @param[in] program_module_count Number of module to query.
 *
 * @param[out] modules Queried modules.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * invalid.
 */
hsa_status_t HSA_API hsa_ext_query_program_modules(
  hsa_ext_program_handle_t program,
  uint32_t program_module_count,
  hsa_ext_brig_module_handle_t *modules);

/**
 * @brief Queries program brig modules.
 *
 * @details Query a program brig module with specified module handle.
 *
 * @param[in] program Program to query module from.
 *
 * @param[in] module Module handle.
 *
 * @param[out] brig_module Queried module.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * invalid, or @a module is invalid.
 */
hsa_status_t HSA_API hsa_ext_query_program_brig_module(
  hsa_ext_program_handle_t program,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_module_t *brig_module);

/**
 * @brief Queries call convention.
 *
 * @param[in] program program Program to query module for.
 *
 * @param[in] agent HSA Agent to query call convention for.
 *
 * @param[out] first_call_convention_id First call convention.
 *
 * @param[out] call_convention_count Number of call conventions in the program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * invalid, or @a agent is invalid.
 */
hsa_status_t HSA_API hsa_ext_query_call_convention(
  hsa_ext_program_handle_t program,
  hsa_agent_t *agent,
  hsa_ext_program_call_convention_id32_t *first_call_convention_id,
  uint32_t *call_convention_count);

/**
 * @brief Defines program's global variable address.
 *
 * @param[in] program Program to define global variable address for.
 *
 * @param[in] module Module to define global variable address for.
 *
 * @param[in] symbol Offset.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message.
 *
 * @param[in] address Specified address.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * invalid, or @a module is invalid.
 */
hsa_status_t HSA_API hsa_ext_define_program_allocation_global_variable_address(
  hsa_ext_program_handle_t program,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  hsa_ext_error_message_callback_t error_message_callback,
  void *address);

/**
 * @brief Queries program's global variable address.
 *
 * @param[in] program Program to query global variable address for.
 *
 * @param[in] module Module to query global variable address for.
 *
 * @param[in] symbol Offset.
 *
 * @param[out] address Queried address.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * invalid, or @a module is invalid.
 */
hsa_status_t HSA_API hsa_ext_query_program_allocation_global_variable_address(
  hsa_ext_program_handle_t program,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  void** address);

/**
 * @brief Defines agent's global variable address.
 *
 * @param[in] program Program to define global variable address for.
 *
 * @param[in] agent HSA Agent to define global variable address for.
 *
 * @param[in] module Module to define global variable address for.
 *
 * @param[in] symbol Offset.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message.
 *
 * @param[in] address Specified address.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * invalid, or @a module is invalid, or @a agent is invalid.
 */
hsa_status_t HSA_API hsa_ext_define_agent_allocation_global_variable_address(
  hsa_ext_program_handle_t program,
  hsa_agent_t *agent,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  hsa_ext_error_message_callback_t error_message_callback,
  void *address);

/**
 * @brief Queries agent's global variable address.
 *
 * @param[in] program Program to query global variable address for.
 *
 * @param[in] agent HSA Agent to query global variable address for.
 *
 * @param[in] module Module to query global variable address for.
 *
 * @param[in] symbol Offset.
 *
 * @param[out] address Queried address.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * invalid, or @a module is invalid, or @a agent is invalid.
 */
hsa_status_t HSA_API hsa_ext_query_agent_global_variable_address(
  hsa_ext_program_handle_t program,
  hsa_agent_t *agent,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  void** address);

/**
 * @brief Defines agent's read-only variable address.
 *
 * @param[in] program Program to define read-only variable address for.
 *
 * @param[in] agent HSA Agent to define read-only variable address for.
 *
 * @param[in] module Module to define read-only variable address for.
 *
 * @param[in] symbol Offset.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message.
 *
 * @param[in] address Specified address.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * invalid, or @a module is invalid, or @a agent is invalid.
 */
hsa_status_t HSA_API hsa_ext_define_readonly_variable_address(
  hsa_ext_program_handle_t program,
  hsa_agent_t * agent,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  hsa_ext_error_message_callback_t error_message_callback,
  void* address);

/**
 * @brief Queries agent's read-only variable address.
 *
 * @param[in] program Program to query read-only variable address for.
 *
 * @param[in] agent HSA Agent to query read-only variable address for.
 *
 * @param[in] module Module to query read-only variable address for.
 *
 * @param[in] symbol Offset.
 *
 * @param[out] address Queried address.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If provided @a program is
 * invalid, or @a module is invalid, or @a agent is invalid.
 */
hsa_status_t HSA_API hsa_ext_query_readonly_variable_address(
  hsa_ext_program_handle_t program,
  hsa_agent_t * agent,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  void** address);

/**
 * @brief Queries kernel descriptor address.
 *
 * @details Queries kernel descriptor address. Needed to create the dispatch
 * packet.
 *
 * @param[in] program Program to query kernel descriptor address from.
 *
 * @param[in] module BRIG module handle.
 *
 * @param[in] symbol Offset.
 *
 * @param[out] address The address of kernel descriptor.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a program or @a module are
 * not valid HSAIL program or BRIG module respectively.
 */
hsa_status_t HSA_API hsa_ext_query_kernel_descriptor_address(
  hsa_ext_program_handle_t program,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  void** address);

/**
 * @brief Queries indirect function descriptor address.
 *
 * @details Queries indirect function descriptor address, which allows host
 * program to perform indirect function table variable initialization.
 *
 * @param[in] program Program to query indirect function descriptor address
 * from.
 *
 * @param[in] module BRIG module handle.
 *
 * @param[in] symbol Offset.
 *
 * @param[out] address The address of indirect function descriptor.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a program or @a module are
 * not valid HSAIL program or BRIG module respectively.
 */
hsa_status_t HSA_API hsa_ext_query_indirect_function_descriptor_address(
  hsa_ext_program_handle_t program,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_brig_code_section_offset32_t symbol,
  void** address);

/**
 * @brief Validates HSAIL program.
 *
 * @details Validates HSAIL program with specified program handle. Returns
 * either ::HSA_STATUS_SUCCESS or ::HSA_STATUS_ERROR if the validation is
 * successful or not. Refer to the @a error_message call back to get the string
 * representation of the failure.
 *
 * @param[in] program Handle to the HSAIL program to validate.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR If the program is not valid, refer to the error
 * call back function to get string representation of the failure.
 */
hsa_status_t HSA_API hsa_ext_validate_program(
  hsa_ext_program_handle_t program,
  hsa_ext_error_message_callback_t error_message_callback);

/**
 * @brief Validates program module.
 *
 * @details Validates program module with specified module handle. Returns
 * either ::HSA_STATUS_SUCCESS or ::HSA_STATUS_ERROR if the validation is
 * successful or not. Refer to the @a error_message_callback call back to get
 * the string representation of the failure.
 *
 * @param[in] program Handle to the HSAIL program.
 *
 * @param[in] module Handle to the module to validate.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR If the module is not valid, refer to the error
 * call back function to get string representation of the failure.
 */
hsa_status_t HSA_API hsa_ext_validate_program_module(
  hsa_ext_program_handle_t program,
  hsa_ext_brig_module_handle_t module,
  hsa_ext_error_message_callback_t error_message_callback);

/**
 * @brief Serializes the HSAIL program.
 *
 * @details Serializes the @a program to @a serialized_object. Used for
 * offline compilation.
 *
 * @param[in] caller Opaque pointer and will be passed to all call back
 * functions made by this call.
 *
 * @param[in] program HSAIL program to be serialized.
 *
 * @param[in] alloc_serialize_data_callback Call back function for allocation.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message.
 *
 * @param[in] debug_information The flag for including/excluding the debug
 * information for @a finalization_descriptor. 0 - exclude debug information, 1
 * - include debug information.
 *
 * @param[in] serialized_object Pointer to the serialized object.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a program is not a valid
 * program.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If no memory can be allocated
 * for @a serialized_object.
 */
hsa_status_t HSA_API hsa_ext_serialize_program(
  hsa_runtime_caller_t caller,
  hsa_ext_program_handle_t program,
  hsa_runtime_alloc_data_callback_t alloc_serialize_data_callback,
  hsa_ext_error_message_callback_t error_message_callback,
  int debug_information,
  void *serialized_object);

/**
 * @brief Call back function to get program's address of global segment
 * variables, kernel table variable, indirect function table variable based
 * on the symbolic name.
 */
typedef hsa_status_t (*hsa_ext_program_allocation_symbol_address_t)(
  hsa_runtime_caller_t caller,
  const char *name,
  uint64_t *symbol_adress);

/**
 * @brief Call back function to get agents's address of global segment
 * variables, kernel table variable, indirect function table variable based
 * on the symbolic name.
 */
typedef hsa_status_t (*hsa_ext_agent_allocation_symbol_address_t)(
  hsa_runtime_caller_t caller,
  hsa_agent_t *agent,
  const char *name,
  uint64_t *symbol_adress);

/**
 * @brief Deserializes the HSAIL program.
 *
 * @details Deserializes the program from @a serialized_object. Used for
 * offline compilation. Includes call back functions
 * hsa_{program, agent}_allocation_symbol_address_t, where call back functions
 * take symbolic name, this allows symbols defined by application to be
 * relocated.
 *
 * @param[in] caller Opaque pointer and will be passed to all call back
 * functions made by this call.
 *
 * @param[in] serialized_object Serialized object to be deserialized.
 *
 * @param[in] program_allocation_symbol_address Call back function to get
 * program's address of global segment variables, kernel table variable,
 * indirect function table variable based on the symbolic name. Allows symbols
 * defined by application to be relocated.
 *
 * @param[in] agent_allocation_symbol_address Call back function to get agent's
 * address of global segment variables, kernel table variable, indirect
 * function table variable based on the symbolic name. Allows symbols defined
 * by application to be relocated.
 *
 * @param[in] error_message_callback Call back function to get the string
 * representation of the error message.
 *
 * @param[in] debug_information The flag for including/excluding the debug
 * information for @a finalization_descriptor. 0 - exclude debug information,
 * 1 - include debug information.
 *
 * @param[out] program Deserialized program.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a serialized_object is
 * either NULL, or is not valid, or the size is 0.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If no memory can be allocated
 * for @a finalization_descriptor.
 */
hsa_status_t HSA_API hsa_ext_deserialize_program(
  hsa_runtime_caller_t caller,
  void *serialized_object,
  hsa_ext_program_allocation_symbol_address_t program_allocation_symbol_address,
  hsa_ext_agent_allocation_symbol_address_t agent_allocation_symbol_address,
  hsa_ext_error_message_callback_t error_message_callback,
  int debug_information,
  hsa_ext_program_handle_t **program);
/** @} */


/** \defgroup images Images API
 *  @{
 */

/**
 * @brief Image handle, populated by ::hsa_ext_image_create_handle. Images handles
 * are only unique within an agent, not across agents.
 *
 */
typedef struct hsa_ext_image_handle_s {
   /**
    * HSA component specific handle to the image.
    */
    uint64_t handle;

} hsa_ext_image_handle_t;

/**
 * @brief Image format capability returned by ::hsa_ext_image_get_format_capability
 */
typedef enum  {
   /**
    * Images of this format are not supported.
    */
    HSA_EXT_IMAGE_FORMAT_NOT_SUPPORTED = 0x0,

   /**
    * Images of this format can be accessed for read operations.
    */
    HSA_EXT_IMAGE_FORMAT_READ_ONLY = 0x1,

   /**
    * Images of this format can be accessed for write operations.
    */
    HSA_EXT_IMAGE_FORMAT_WRITE_ONLY = 0x2,

    /**
    * Images of this format can be accessed for read and write operations.
    */
    HSA_EXT_IMAGE_FORMAT_READ_WRITE = 0x4,

    /**
    * Images of this format can be accessed for read-modify-write operations.
    */
    HSA_EXT_IMAGE_FORMAT_READ_MODIFY_WRITE = 0x8,
    /**
    * Images of this format are guaranteed to have consistent data layout
    * regardless of the how it is accessed by the HSA agent.
    */
    HSA_EXT_IMAGE_FORMAT_ACCESS_INVARIANT_IMAGE_DATA = 0x10

} hsa_ext_image_format_capability_t;

/**
 * @brief Agent-specific image size and alignment requirements. This structure
 * stores the agent-dependent image data sizes and alignment, and populated by
 * ::hsa_ext_image_get_info.
 */
typedef struct hsa_ext_image_info_s {
  /**
   * Component specific image data size in bytes.
   */
  size_t image_size;

  /**
   * Component specific image data alignment in bytes.
   */
  size_t image_alignment;

} hsa_ext_image_info_t;

/**
 * @brief Defines how the HSA device expects to access the image. The access
 * pattern used by the HSA agent specified in ::hsa_ext_image_create_handle.
 *
 */
typedef enum {
  /**
   * Image handle is to be used by the HSA agent as read-only using an HSAIL
   * roimg type.
   */
  HSA_EXT_IMAGE_ACCESS_PERMISSION_READ_ONLY,

  /**
   * Image handle is to be used by the HSA agent as write-only using an HSAIL
   * woimg type.
   */
  HSA_EXT_IMAGE_ACCESS_PERMISSION_WRITE_ONLY,

  /**
   * Image handle is to be used by the HSA agent as read and/or write using an
   * HSAIL rwimg type.
   */
  HSA_EXT_IMAGE_ACCESS_PERMISSION_READ_WRITE

} hsa_ext_image_access_permission_t;

/**
 * @brief Geometry associated with the HSA image (image dimensions allowed in
 * HSA). The enumeration values match the HSAIL BRIG type BrigImageGeometry.
 */
typedef enum {
/**
   * One-dimensional image addressed by width coordinate.
   */
  HSA_EXT_IMAGE_GEOMETRY_1D = 0,

  /**
   * Two-dimensional image addressed by width and height coordinates.
   */
  HSA_EXT_IMAGE_GEOMETRY_2D = 1,

  /**
   * Three-dimensional image addressed by width, height, and depth coordinates.
   */
  HSA_EXT_IMAGE_GEOMETRY_3D = 2,

  /**
   * Array of one-dimensional images with the same size and format. 1D arrays
   * are addressed by index and width coordinate.
   */
  HSA_EXT_IMAGE_GEOMETRY_1DA = 3,

  /**
   * Array of two-dimensional images with the same size and format. 2D arrays
   * are addressed by index and width and height coordinates.
   */
  HSA_EXT_IMAGE_GEOMETRY_2DA = 4,

  /**
   * One-dimensional image interpreted as a buffer with specific restrictions.
   */
  HSA_EXT_IMAGE_GEOMETRY_1DB = 5,

  /**
   * Two-dimensional depth image addressed by width and height coordinates.
   */
  HSA_EXT_IMAGE_GEOMETRY_2DDEPTH = 6,

  /**
   * Array of two-dimensional depth images with the same size and format.  2D
   * arrays are addressed by index and width and height coordinates.
   */
  HSA_EXT_IMAGE_GEOMETRY_2DADEPTH = 7
} hsa_ext_image_geometry_t;

/**
* @brief Component type associated with the image. See Image section in HSA
* Programming Reference Manual for definitions on each component type. The
* enumeration values match the HSAIL BRIG type BrigImageChannelType.
*/
typedef enum
{
    HSA_EXT_IMAGE_CHANNEL_TYPE_SNORM_INT8 = 0,
    HSA_EXT_IMAGE_CHANNEL_TYPE_SNORM_INT16 = 1,
    HSA_EXT_IMAGE_CHANNEL_TYPE_UNORM_INT8 = 2,
    HSA_EXT_IMAGE_CHANNEL_TYPE_UNORM_INT16 = 3,
    HSA_EXT_IMAGE_CHANNEL_TYPE_UNORM_INT24 = 4,
    HSA_EXT_IMAGE_CHANNEL_TYPE_UNORM_SHORT_555 = 5,
    HSA_EXT_IMAGE_CHANNEL_TYPE_UNORM_SHORT_565 = 6,
    HSA_EXT_IMAGE_CHANNEL_TYPE_UNORM_SHORT_101010 = 7,
    HSA_EXT_IMAGE_CHANNEL_TYPE_SIGNED_INT8 = 8,
    HSA_EXT_IMAGE_CHANNEL_TYPE_SIGNED_INT16 = 9,
    HSA_EXT_IMAGE_CHANNEL_TYPE_SIGNED_INT32 = 10,
    HSA_EXT_IMAGE_CHANNEL_TYPE_UNSIGNED_INT8 = 11,
    HSA_EXT_IMAGE_CHANNEL_TYPE_UNSIGNED_INT16 = 12,
    HSA_EXT_IMAGE_CHANNEL_TYPE_UNSIGNED_INT32 = 13,
    HSA_EXT_IMAGE_CHANNEL_TYPE_HALF_FLOAT = 14,
    HSA_EXT_IMAGE_CHANNEL_TYPE_FLOAT = 15

} hsa_ext_image_channel_type_t;

/**
 *
* @brief Image component order associated with the image. See Image section in
* HSA Programming Reference Manual for definitions on each component order. The
* enumeration values match the HSAIL BRIG type BrigImageChannelOrder.
*
*/
typedef enum {

    HSA_EXT_IMAGE_CHANNEL_ORDER_A = 0,
    HSA_EXT_IMAGE_CHANNEL_ORDER_R = 1,
    HSA_EXT_IMAGE_CHANNEL_ORDER_RX = 2,
    HSA_EXT_IMAGE_CHANNEL_ORDER_RG = 3,
    HSA_EXT_IMAGE_CHANNEL_ORDER_RGX = 4,
    HSA_EXT_IMAGE_CHANNEL_ORDER_RA = 5,
    HSA_EXT_IMAGE_CHANNEL_ORDER_RGB = 6,
    HSA_EXT_IMAGE_CHANNEL_ORDER_RGBX = 7,
    HSA_EXT_IMAGE_CHANNEL_ORDER_RGBA = 8,
    HSA_EXT_IMAGE_CHANNEL_ORDER_BGRA = 9,
    HSA_EXT_IMAGE_CHANNEL_ORDER_ARGB = 10,
    HSA_EXT_IMAGE_CHANNEL_ORDER_ABGR = 11,
    HSA_EXT_IMAGE_CHANNEL_ORDER_SRGB = 12,
    HSA_EXT_IMAGE_CHANNEL_ORDER_SRGBX = 13,
    HSA_EXT_IMAGE_CHANNEL_ORDER_SRGBA = 14,
    HSA_EXT_IMAGE_CHANNEL_ORDER_SBGRA = 15,
    HSA_EXT_IMAGE_CHANNEL_ORDER_INTENSITY = 16,
    HSA_EXT_IMAGE_CHANNEL_ORDER_LUMINANCE = 17,
    HSA_EXT_IMAGE_CHANNEL_ORDER_DEPTH = 18,
    HSA_EXT_IMAGE_CHANNEL_ORDER_DEPTH_STENCIL = 19

} hsa_ext_image_channel_order_t;

/**
 * @brief Image format descriptor (attributes of the image format).
 */
typedef struct hsa_ext_image_format_s {
/**
    * Channel type of the image.
    */
    hsa_ext_image_channel_type_t channel_type;

   /**
    * Channel order of the image.
    */
    hsa_ext_image_channel_order_t channel_order;
} hsa_ext_image_format_t;

/**
 * @brief Implementation-independent HSA Image descriptor.
 */
typedef struct hsa_ext_image_descriptor_s {
  /**
   * Geometry of the image.
   */
  hsa_ext_image_geometry_t geometry;

  /**
   * Width of the image in components.
   */
  size_t width;

  /**
   * Height of the image in components, only used if geometry is 2D or higher.
   */
  size_t height;

  /**
   * Depth of the image in slices, only used if geometry is 3D depth = 0 is same
   * as depth = 1.
   */
  size_t depth;

  /**
   * Number of images in the image array, only used if geometry is 1DArray and
   * 2DArray.
   */
  size_t array_size;

  /**
   * Format of the image.
   */
  hsa_ext_image_format_t format;


} hsa_ext_image_descriptor_t;

/**
 * @brief Three-dimensional image range description.
 */
typedef struct hsa_ext_image_range_s {
   /**
    * The width for an image range (in coordinates).
    */
    uint32_t width;

   /**
    * The height for an image range (in coordinates).
    */
    uint32_t height;

   /**
    * The depth for an image range (in coordinates).
    */
    uint32_t depth;

} hsa_ext_image_range_t;

/**
 * @brief Image region description. Used by image operations such as import,
 * export, copy, and clear.
 */
typedef struct hsa_ext_image_region_s {
   /**
    * Offset in the image (in coordinates).
    */
    hsa_dim3_t image_offset;

   /**
    * Dimensions of the image range (in coordinates).
    */
    hsa_ext_image_range_t image_range;

} hsa_ext_image_region_t;

/**
 * @brief Sampler handle. Samplers are populated by
 * ::hsa_ext_sampler_create_handle. Sampler handles are only unique within an
 * agent, not across agents.
 */
typedef struct hsa_ext_sampler_handle_s {
   /**
    * Component-specific HSA sampler.
    */
    uint64_t handle;

} hsa_ext_sampler_handle_t;

/**
 * @brief Sampler address modes. The sampler address mode describes the
 * processing of out-of-range image coordinates. The values match the HSAIL BRIG
 * type BrigSamplerAddressing.
 */
typedef enum {

  /**
   * Out-of-range coordinates are not handled.
   */
  HSA_EXT_SAMPLER_ADDRESSING_UNDEFINED = 0,

  /**
   * Clamp out-of-range coordinates to the image edge.
   */
  HSA_EXT_SAMPLER_ADDRESSING_CLAMP_TO_EDGE = 1,

  /**
   * Clamp out-of-range coordinates to the image border.
   */
  HSA_EXT_SAMPLER_ADDRESSING_CLAMP_TO_BORDER = 2,

  /**
   * Wrap out-of-range coordinates back into the valid coordinate range.
   */
  HSA_EXT_SAMPLER_ADDRESSING_REPEAT = 3,

  /**
   * Mirror out-of-range coordinates back into the valid coordinate range.
   */
  HSA_EXT_SAMPLER_ADDRESSING_MIRRORED_REPEAT = 4

} hsa_ext_sampler_addressing_mode_t;

/**
 * @brief Sampler coordinate modes. The enumeration values match the HSAIL BRIG
 * BRIG_SAMPLER_COORD bit in the type BrigSamplerModifier.
 */
typedef enum {
  /**
   * Coordinates are all in the range of 0.0 to 1.0.
   */
  HSA_EXT_SAMPLER_COORD_NORMALIZED = 0,

  /**
   * Coordinates are all in the range of 0 to (dimension-1).
   */
  HSA_EXT_SAMPLER_COORD_UNNORMALIZED = 1

} hsa_ext_sampler_coordinate_mode_t;

/**
 * @brief Sampler filter modes. The enumeration values match the HSAIL BRIG type
 * BrigSamplerFilter.
 */
typedef enum {
  /**
   * Filter to the image element nearest (in Manhattan distance) to the
   * specified coordinate.
   */
  HSA_EXT_SAMPLER_FILTER_NEAREST = 0,

  /**
   * Filter to the image element calculated by combining the elements in a 2x2
   * square block or 2x2x2 cube block around the specified coordinate. The
   * elements are combined using linear interpolation.
   */
  HSA_EXT_SAMPLER_FILTER_LINEAR = 1

} hsa_ext_sampler_filter_mode_t;

/**
 * @brief Implementation-independent sampler descriptor.
 */
typedef struct hsa_ext_sampler_descriptor_s {
  /**
   * Sampler coordinate mode describes the normalization of image coordinates.
   */
  hsa_ext_sampler_coordinate_mode_t coordinate_mode;

  /**
   * Sampler filter type describes the type of sampling performed.
   */
  hsa_ext_sampler_filter_mode_t filter_mode;

  /**
   * Sampler address mode describes the processing of out-of-range image
   * coordinates.
   */
  hsa_ext_sampler_addressing_mode_t address_mode;

} hsa_ext_sampler_descriptor_t;

/**
 * @brief Retrieve image format capabilities for the specified image format on
 * the specified HSA component.
 *
 * @details If successful, the queried image format's capabilities bit-mask is
 * written to the location specified by @a capability_mask. See
 * ::hsa_ext_image_format_capability_t to determine all possible capabilities that
 * can be reported in the bit mask.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] image_format Image format.
 *
 * @param[in] image_geometry Geometry of the image.
 *
 * @param[out] capability_mask Image format capability bit-mask.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a agent, @a image_format, or
 *       @a capability_mask are NULL.
 */
hsa_status_t HSA_API hsa_ext_image_get_format_capability(const hsa_agent_t *agent,
                         const hsa_ext_image_format_t *image_format,
                         hsa_ext_image_geometry_t image_geometry,
                         uint32_t *capability_mask);
/**
 *
 * @brief Inquires the required HSA component-specific image data details from a
 * implementation independent image descriptor.
 *
 * @details If successful, the queried HSA agent-specific image data info is
 * written to the location specified by @a image_info. Based on the
 * implementation the optimal image data size and alignment requirements could
 * vary depending on the image attributes specified in @a image_descriptor.
 *
 * The implementation must return the same image info requirements for different
 * access permissions with exactly the same image descriptor as long as
 * ::hsa_ext_image_get_format_capability reports
 * ::HSA_EXT_IMAGE_FORMAT_ACCESS_INVARIANT_IMAGE_DATA for the image format
 * specified in the image descriptor.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] image_descriptor Implementation-independent image descriptor
 * describing the image.
 *
 * @param[in] access_permission Access permission of the image by the HSA agent.
 *
 * @param[out] image_info Image info size and alignment requirements that the
 * HSA agent requires.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If any of the arguments is NULL.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_IMAGE_FORMAT_UNSUPPORTED If the HSA agent does
 * not support the image format specified by the descriptor.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_IMAGE_SIZE_UNSUPPORTED If the HSA agent does
 * not support the image dimensions specified by the format descriptor.
 */
hsa_status_t HSA_API hsa_ext_image_get_info(
                         const hsa_agent_t *agent,
                         const hsa_ext_image_descriptor_t *image_descriptor,
                         hsa_ext_image_access_permission_t access_permission,
                         hsa_ext_image_info_t *image_info);

/**
 * @brief Creates a agent-defined image handle from an
 * implementation-independent image descriptor and a agent-specific image
 * data. The image access defines how the HSA agent expects to use the image and
 * must match the HSAIL image handle type used by the agent.
 *
 * @details If successful, the image handle is written to the location specified
 * by @a image_handle. The image data memory must be allocated using the
 * previously queried ::hsa_ext_image_get_info memory requirements with the same
 * HSA agent and implementation-independent image descriptor.
 *
 * The image data is not initialized and any previous memory contents is
 * preserved. The memory management of image data is the application's
 * responsibility and can only be freed until the memory is no longer needed and
 * any image handles using it are destroyed.
 *
 * @a access_permission defines how the HSA agent expects to use the image
 * handle. The image format specified in the image descriptor must be capable by
 * the HSA agent for the intended permission.
 *
 * Image handles with different permissions can be created using the same image
 * data with exactly the same image descriptor as long as
 * ::HSA_EXT_IMAGE_FORMAT_ACCESS_INVARIANT_IMAGE_DATA is reported by
 * ::hsa_ext_image_get_format_capability for the image format specified in the
 * image descriptor. Images of non-linear s-form channel order can share the
 * same image data with its equivalent linear non-s form channel order, provided
 * the rest of the image descriptor parameters are identical.
 *
 * If necessary, an application can use image operations (import, export, copy,
 * clear) to prepare the image for the intended use regardless of the access
 * permissions.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] image_descriptor Implementation-independent image descriptor
 * describing the image.
 *
 * @param[in] image_data Address of the component-specific image data.
 *
 * @param[in] access_permission Access permission of the image by the HSA agent.
 *
 * @param[out] image_handle Agent-specific image handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If any of the arguments is NULL.
 *
 * @retval ::HSA_EXT_STATUS_ERROR_IMAGE_FORMAT_UNSUPPORTED If the HSA agent does
 * not have the capability to support the image format using the specified @a
 * agent_access.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If the HSA agent cannot create
 * the specified handle because it is out of resources.
 *
 */
hsa_status_t HSA_API hsa_ext_image_create_handle(
                         const hsa_agent_t *agent,
                         const hsa_ext_image_descriptor_t *image_descriptor,
                         const void *image_data,
                         hsa_ext_image_access_permission_t access_permission,
                         hsa_ext_image_handle_t *image_handle);

/**
 * @brief Imports a linearly organized image data from memory directly to an
 * image handle.
 *
 * @details This operation updates the image data referenced by the image handle
 * from the source memory. The size of the data imported from memory is
 * implicitly derived from the image region.
 *
 * If @a completion_signal is NULL, the operation occurs
 * synchronously. Otherwise the function returns immediately and the
 * completion signal is signaled when the operation completes.
 *
 * If @a src_row_pitch is smaller than the destination region width (in bytes),
 * then @a src_row_pitch = region width.
 *
 * If @a src_slice_pitch is smaller than the destination region width * region
 * height (in bytes), then @a src_slice_pitch = region width * region height.
 *
 * It is the application's responsibility to avoid out of bounds memory access.
 *
 * None of the source memory or image data memory in the previously created
 * ::hsa_ext_image_create_handle image handle can overlap.  Overlapping of any
 * of the source and destination memory within the import operation produces
 * undefined results.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] src_memory Source memory.
 *
 * @param[in] src_row_pitch Number of bytes in one row of the source memory.
 *
 * @param[in] src_slice_pitch Number of bytes in one slice of the source memory.
 *
 * @param[in] dst_image_handle Destination Image handle.
 *
 * @param[in] image_region Image region to be updated.
 *
 * @param[in] completion_signal Signal to set when the operation is completed.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a agent, @a src_memory or @a
 * image_region are NULL.
 *
 */
hsa_status_t HSA_API hsa_ext_image_import (
                         const hsa_agent_t *agent,
                         const void *src_memory,
                         size_t src_row_pitch,
                         size_t src_slice_pitch,
                         hsa_ext_image_handle_t dst_image_handle,
                         const hsa_ext_image_region_t *image_region,
                         const hsa_signal_handle_t *completion_signal);

/**
 * @brief Export image data from the image handle directly to memory organized
 * linearly.
 *
 * @details The operation updates the destination memory with the image data in
 * the image handle. The size of the data exported to memory is implicitly
 * derived from the image region.
 *
 * If @a completion_signal is NULL, the operation occurs
 * synchronously. Otherwise the function returns immediately and the
 * completion signal is signaled when the operation completes.
 *
 * If @a dst_row_pitch is smaller than the source region width (in bytes), then
 * @a dst_row_pitch = region width.
 *
 * If @a dst_slice_pitch is smaller than the source region width * region height
 * (in bytes), then @a dst_slice_pitch = region width * region height.
 *
 * It is the application's responsibility to avoid out of bounds memory access.
 *
 * None of the destination memory or image data memory in the previously created
 * ::hsa_ext_image_create_handle image handle can overlap. Overlapping of any of
 * the source and destination memory within the export operation produces
 * undefined results.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] src_image_handle Source image handle.
 *
 * @param[in] dst_memory Destination memory.
 *
 * @param[in] dst_row_pitch Number of bytes in one row of the destination
 * memory.
 *
 * @param[in] dst_slice_pitch Number of bytes in one slice of the destination
 * memory.
 *
 * @param[in] image_region Image region to be exported.
 *
 * @param[in] completion_signal Signal to set when the operation is completed.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a agent, @a dst_memory or @a
 * image_region are NULL.
 */
hsa_status_t HSA_API hsa_ext_image_export(
                          const hsa_agent_t *agent,
                          hsa_ext_image_handle_t src_image_handle,
                          void *dst_memory,
                          size_t dst_row_pitch,
                          size_t dst_slice_pitch,
                          const hsa_ext_image_region_t *image_region,
                          const hsa_signal_handle_t *completion_signal);

/**
 * @brief Copies a region from one image to another.
 *
 * @details The operation copies the image data from the source image handle to
 * the destination image handle. The size of the image data copied is implicitly
 * derived from the image region.
 *
 * If @a completion_signal is NULL, the operation occurs
 * synchronously. Otherwise the function returns immediately and the
 * completion signal is signaled when the operation completes.
 *
 * It is the application's responsibility to avoid out of bounds memory access.
 *
 * The source and destination handles must have been previously created using
 * ::hsa_ext_image_create_handle. The source and destination image data memory
 * are not allowed to be the same. Overlapping any of the source and destination
 * memory produces undefined results.
 *
 * The source and destination image formats don't have to match; appropriate
 * format conversion is performed automatically. The source and destination
 * images must be of the same geometry.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] src_image_handle Source image handle.
 *
 * @param[in] dst_image_handle Destination image handle.
 *
 * @param[in] image_region Image region to be copied.
 *
 * @param[in] completion_signal Signal to set when the operation is completed.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a agent or @a image_region
 * are NULL.
 */
hsa_status_t HSA_API hsa_ext_image_copy(
                       const hsa_agent_t *agent,
                       hsa_ext_image_handle_t src_image_handle,
                       hsa_ext_image_handle_t dst_image_handle,
                       const hsa_ext_image_region_t *image_region,
                       const hsa_signal_handle_t *completion_signal);

/**
 * @brief Clears the image to a specified 4-component floating point data.
 *
 * @details The operation clears the elements of the image with the data
 * specified. The lowest bits of the data (number of bits depending on the image
 * component type) are stored in the cleared image are based on the image
 * component order. The size of the image data cleared is implicitly derived
 * from the image region.
 *
 * If @a completion_signal is NULL, the operation occurs
 * synchronously. Otherwise the function returns immediately and the
 * completion signal is signaled when the operation completes.
 *
 * It is the application's responsibility to avoid out of bounds memory access.
 *
 * Clearing an image automatically performs value conversion on the provided
 * floating point values as is appropriate for the image format used.
 *
 * For images of UNORM types, the floating point values must be in the [0..1]
 * range. For images of SNORM types, the floating point values must be in the
 * [-1..1] range. For images of UINT types, the floating point values are
 * rounded down to an integer value. For images of SRGB types, the clear data is
 * specified in a linear space, which is appropriately converted by the Runtime
 * to sRGB color space.
 *
 * Specifying clear value outside of the range representable by an image format
 * produces undefined results.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] image_handle Image to be cleared.
 *
 * @param[in] data 4-component clear value in floating point format.
 *
 * @param[in] image_region Image region to clear.
 *
 * @param[in] completion_signal Signal to set when the operation is completed.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a agent or @a
 * image_region are NULL.
 */
hsa_status_t HSA_API hsa_ext_image_clear(
                        const hsa_agent_t *agent,
                        hsa_ext_image_handle_t image_handle,
                        const float data[4],
                        const hsa_ext_image_region_t *image_region,
                        const hsa_signal_handle_t *completion_signal);

/**
 * @brief Destroys the specified image handle.
 *
 * @details If successful, the image handle previously created using
 *  ::hsa_ext_image_create_handle is destroyed.
 *
 * Destroying the image handle does not free the associated image data.
 *
 * The image handle should not be destroyed while there are references to it
 * queued for execution or currently being used in a dispatch. Failure to
 * properly track image data lifetime causes undefined results due to premature
 * image handle deletion.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] image_handle Image handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a agent or @a image_handle is
 * NULL.
 */
hsa_status_t HSA_API hsa_ext_image_destroy_handle (
                        const hsa_agent_t *agent,
                        hsa_ext_image_handle_t *image_handle);

/**
 * @brief Create an HSA component-defined sampler handle from a
 * component-independent sampler descriptor.
 *
 * @details If successful, the sampler handle is written to the location
 * specified by the sampler handle.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] sampler_descriptor Implementation-independent sampler descriptor.
 *
 * @param[out] sampler_handle Component-specific sampler handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If any of the arguments is NULL.
 *
 * @retval ::HSA_STATUS_ERROR_OUT_OF_RESOURCES If the HSA agent cannot create
 * the specified handle because it is out of resources.
 */
hsa_status_t HSA_API hsa_ext_sampler_create_handle(
                     const hsa_agent_t *agent,
                     const hsa_ext_sampler_descriptor_t *sampler_descriptor,
                     hsa_ext_sampler_handle_t *sampler_handle);

/**
 * @brief Destroys the specified sampler handle.
 *
 * @details If successful, the sampler handle previously created using
 * ::hsa_ext_sampler_create_handle is destroyed.
 *
 * The sampler handle should not be destroyed while there are references to it
 * queued for execution or currently being used in a dispatch.
 *
 * @param[in] agent HSA agent to be associated with the image.
 *
 * @param[in] sampler_handle Sampler handle.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If any of the arguments is NULL.
 */
hsa_status_t HSA_API hsa_ext_sampler_destroy_handle(
                     const hsa_agent_t *agent,
                     hsa_ext_sampler_handle_t *sampler_handle);
/** @} */


/** \defgroup agent_dispatch TODO
 *  @{
 */

/**
 * @brief Callback to be invoked when a user-defined service has been requested
 * by a agent dispatch packet.
 */
typedef void (*hsa_agent_dispatch_callback_t)(
  uint16_t type,
  uint64_t arg0,
  uint64_t arg1,
  uint64_t arg2,
  uint64_t arg3,
  void* return_location);

/**
 * @brief Agent dispatch runtime function registration.
 *
 * @param agent_dispatch_queue Agent dispatch queue.
 *
 * @param[in] agent_dispatch_callback Callback that the user is registering.
 *
 * @param callback Event callback.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been initialized.
 *
 */
hsa_status_t HSA_API hsa_register_agent_dispatch_callback(
    hsa_queue_t *agent_dispatch_queue,
    hsa_agent_dispatch_callback_t agent_dispatch_callback,
    hsa_event_callback_t callback);
/** @} */


/** \defgroup extensions TODO
 *  @{
 */
/**
 * @brief HSA extensions.
 */
typedef enum {
  /**
   * Start of the multi vendor extension range.
   */
  HSA_EXT_START = 0,
  /**
   * Finalizer extension. Finalizes the brig to compilation units that represent
   * kernel and function code objects.
   */
  HSA_EXT_FINALIZER = HSA_EXT_START,
  /**
   * Linker extension.
   */
  HSA_EXT_LINKER = 1,
  /**
   * Images extension.
   */
  HSA_EXT_IMAGES = 2,
  /**
   * Start of the single vendor extension range.
   */
  HSA_SVEXT_START = 10000,
} hsa_extension_t;

/**
 * @brief Query vendor extensions.
 *
 * @details If successful, the extension information is written with
 * extension-specific information such as version information, function
 * pointers, and data values. If the extension is not supported, the extension
 * information is not modified.
 *
 * @param[in] extension The vendor extension that is being queried.
 *
 * @param[out] extension_structure Extension structure.
 *
 * @param[out] result Pointer to memory location where to store the query result.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a extension is not a valid
 * value for a single vendor extension or @a result is NULL.
 */
hsa_status_t HSA_API hsa_vendor_extension_query(hsa_extension_t extension,
                                         void *extension_structure,
                                        int* result);
/**
 * @brief Query HSA extensions.
 *
 * @param[in] extension The  extension that is being queried.
 *
 * @param[out] result Pointer to memory location where to store the query result.
 *
 * @retval ::HSA_STATUS_SUCCESS The function has been executed successfully.
 *
 * @retval ::HSA_STATUS_ERROR_NOT_INITIALIZED The runtime has not been
 * initialized.
 *
 * @retval ::HSA_STATUS_ERROR_INVALID_ARGUMENT If @a extension is not a valid
 * value for a HSA extension or @a result is NULL.
 */
  hsa_status_t HSA_API hsa_extension_query(hsa_extension_t extension,
                                           int* result);
/** @} */


#ifdef __cplusplus
}
#endif  /*__cplusplus*/
#endif
