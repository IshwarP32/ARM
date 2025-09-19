/**
 * @file memory_manager.h
 * @brief Memory Management Interface
 * @author Team Member 4 - Memory Management
 * @date 2024
 * 
 * This module implements dynamic memory allocation and management
 * for the RTOS system.
 */

#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "rtos_config.h"

/* ============================================================================
 * MEMORY CONFIGURATION
 * ============================================================================ */
#define MEMORY_ALIGNMENT            4       /* Memory alignment in bytes */
#define MIN_BLOCK_SIZE              16      /* Minimum allocation size */
#define MEMORY_MAGIC_FREE           0xDEAD  /* Magic number for free blocks */
#define MEMORY_MAGIC_USED           0xBEEF  /* Magic number for used blocks */

/* ============================================================================
 * MEMORY BLOCK STRUCTURE
 * ============================================================================ */
typedef struct memory_block {
    uint16_t magic;                         /* Magic number for validation */
    uint16_t size;                          /* Block size including header */
    struct memory_block* next;              /* Next block in list */
    struct memory_block* prev;              /* Previous block in list */
} memory_block_t;

/* ============================================================================
 * MEMORY STATISTICS
 * ============================================================================ */
typedef struct {
    uint32_t total_heap_size;               /* Total heap size */
    uint32_t free_heap_size;                /* Current free heap size */
    uint32_t used_heap_size;                /* Current used heap size */
    uint32_t min_free_heap_size;            /* Minimum free heap size reached */
    uint32_t max_used_heap_size;            /* Maximum used heap size reached */
    uint32_t allocation_count;              /* Number of allocations */
    uint32_t free_count;                    /* Number of frees */
    uint32_t failed_allocations;            /* Number of failed allocations */
    uint32_t largest_free_block;            /* Size of largest free block */
    uint32_t free_blocks_count;             /* Number of free blocks */
} memory_stats_t;

/* ============================================================================
 * FUNCTION PROTOTYPES
 * ============================================================================ */

/**
 * @brief Initialize the memory manager
 * @return rtos_result_t Success or error code
 */
rtos_result_t memory_init(void);

/**
 * @brief Allocate memory block
 * @param size Size in bytes to allocate
 * @return void* Pointer to allocated memory or NULL if failed
 */
void* memory_alloc(uint32_t size);

/**
 * @brief Free previously allocated memory block
 * @param ptr Pointer to memory block to free
 * @return rtos_result_t Success or error code
 */
rtos_result_t memory_free(void* ptr);

/**
 * @brief Reallocate memory block with new size
 * @param ptr Pointer to existing memory block (NULL for new allocation)
 * @param new_size New size in bytes
 * @return void* Pointer to reallocated memory or NULL if failed
 */
void* memory_realloc(void* ptr, uint32_t new_size);

/**
 * @brief Allocate and clear memory block
 * @param num Number of elements
 * @param size Size of each element
 * @return void* Pointer to allocated and cleared memory or NULL if failed
 */
void* memory_calloc(uint32_t num, uint32_t size);

/**
 * @brief Get memory statistics
 * @param stats Pointer to statistics structure
 * @return rtos_result_t Success or error code
 */
rtos_result_t memory_get_stats(memory_stats_t* stats);

/**
 * @brief Get current free heap size
 * @return uint32_t Free heap size in bytes
 */
uint32_t memory_get_free_size(void);

/**
 * @brief Get current used heap size
 * @return uint32_t Used heap size in bytes
 */
uint32_t memory_get_used_size(void);

/**
 * @brief Get size of largest available free block
 * @return uint32_t Size of largest free block
 */
uint32_t memory_get_largest_free_block(void);

/**
 * @brief Check if pointer is valid allocated memory
 * @param ptr Pointer to check
 * @return bool True if valid, false otherwise
 */
bool memory_is_valid_ptr(void* ptr);

/**
 * @brief Get allocated block size for given pointer
 * @param ptr Pointer to allocated memory
 * @return uint32_t Size of allocated block (0 if invalid)
 */
uint32_t memory_get_block_size(void* ptr);

/**
 * @brief Defragment heap memory
 * @return rtos_result_t Success or error code
 */
rtos_result_t memory_defragment(void);

/**
 * @brief Check heap integrity
 * @return rtos_result_t Success if heap is valid, error otherwise
 */
rtos_result_t memory_check_integrity(void);

/**
 * @brief Print memory information (for debugging)
 */
void memory_print_info(void);

/**
 * @brief Print heap layout (for debugging)
 */
void memory_print_heap_layout(void);

/**
 * @brief Reset memory statistics
 */
void memory_reset_stats(void);

#endif /* MEMORY_MANAGER_H */