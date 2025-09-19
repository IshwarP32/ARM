/**
 * @file memory_manager.c
 * @brief Memory Management Implementation
 * @author Team Member 4 - Memory Management
 * @date 2024
 * 
 * This module implements dynamic memory allocation using a first-fit algorithm
 * with coalescing of adjacent free blocks.
 */

#include "memory_manager.h"

/* ============================================================================
 * GLOBAL VARIABLES
 * ============================================================================ */
static uint8_t heap[HEAP_SIZE];             /* Static heap memory */
static memory_block_t* free_list = NULL;    /* Head of free blocks list */
static memory_stats_t stats;                /* Memory statistics */
static bool memory_initialized = false;

/* ============================================================================
 * PRIVATE FUNCTION PROTOTYPES
 * ============================================================================ */
static memory_block_t* memory_find_free_block(uint32_t size);
static void memory_split_block(memory_block_t* block, uint32_t size);
static void memory_coalesce_blocks(void);
static void memory_insert_free_block(memory_block_t* block);
static void memory_remove_free_block(memory_block_t* block);
static uint32_t memory_align_size(uint32_t size);
static void memory_update_stats(void);

/* ============================================================================
 * PUBLIC FUNCTIONS
 * ============================================================================ */

/**
 * @brief Initialize the memory manager
 */
rtos_result_t memory_init(void)
{
    if(memory_initialized)
    {
        return RTOS_SUCCESS;
    }
    
    /* Clear heap */
    memset(heap, 0, HEAP_SIZE);
    
    /* Initialize first free block covering entire heap */
    free_list = (memory_block_t*)heap;
    free_list->magic = MEMORY_MAGIC_FREE;
    free_list->size = HEAP_SIZE;
    free_list->next = NULL;
    free_list->prev = NULL;
    
    /* Initialize statistics */
    memset(&stats, 0, sizeof(memory_stats_t));
    stats.total_heap_size = HEAP_SIZE;
    stats.free_heap_size = HEAP_SIZE - sizeof(memory_block_t);
    stats.min_free_heap_size = stats.free_heap_size;
    stats.largest_free_block = stats.free_heap_size;
    stats.free_blocks_count = 1;
    
    memory_initialized = true;
    
    DEBUG_PRINT("Memory Manager initialized with %d bytes heap\n", HEAP_SIZE);
    
    return RTOS_SUCCESS;
}

/**
 * @brief Allocate memory block
 */
void* memory_alloc(uint32_t size)
{
    if(!memory_initialized || size == 0)
    {
        return NULL;
    }
    
    /* Align size and add header overhead */
    uint32_t aligned_size = memory_align_size(size + sizeof(memory_block_t));
    
    if(aligned_size < MIN_BLOCK_SIZE)
    {
        aligned_size = MIN_BLOCK_SIZE;
    }
    
    ENTER_CRITICAL();
    
    /* Find suitable free block */
    memory_block_t* block = memory_find_free_block(aligned_size);
    
    if(block == NULL)
    {
        EXIT_CRITICAL();
        stats.failed_allocations++;
        DEBUG_PRINT("Memory allocation failed for %u bytes\n", size);
        return NULL;
    }
    
    /* Remove block from free list */
    memory_remove_free_block(block);
    
    /* Split block if too large */
    if(block->size >= aligned_size + MIN_BLOCK_SIZE)
    {
        memory_split_block(block, aligned_size);
    }
    
    /* Mark block as used */
    block->magic = MEMORY_MAGIC_USED;
    
    /* Update statistics */
    stats.allocation_count++;
    stats.used_heap_size += block->size;
    stats.free_heap_size -= block->size;
    
    if(stats.used_heap_size > stats.max_used_heap_size)
    {
        stats.max_used_heap_size = stats.used_heap_size;
    }
    
    if(stats.free_heap_size < stats.min_free_heap_size)
    {
        stats.min_free_heap_size = stats.free_heap_size;
    }
    
    memory_update_stats();
    
    EXIT_CRITICAL();
    
    /* Return pointer to user data (skip header) */
    return (void*)((uint8_t*)block + sizeof(memory_block_t));
}

/**
 * @brief Free previously allocated memory block
 */
rtos_result_t memory_free(void* ptr)
{
    if(!memory_initialized || ptr == NULL)
    {
        return RTOS_INVALID_PARAM;
    }
    
    /* Get block header */
    memory_block_t* block = (memory_block_t*)((uint8_t*)ptr - sizeof(memory_block_t));
    
    /* Validate block */
    if(block->magic != MEMORY_MAGIC_USED)
    {
        DEBUG_PRINT("Invalid memory block or double free detected\n");
        return RTOS_ERROR;
    }
    
    /* Check bounds */
    if((uint8_t*)block < heap || (uint8_t*)block >= heap + HEAP_SIZE)
    {
        DEBUG_PRINT("Memory block outside heap bounds\n");
        return RTOS_ERROR;
    }
    
    ENTER_CRITICAL();
    
    /* Mark block as free */
    block->magic = MEMORY_MAGIC_FREE;
    
    /* Update statistics */
    stats.free_count++;
    stats.used_heap_size -= block->size;
    stats.free_heap_size += block->size;
    
    /* Add block to free list */
    memory_insert_free_block(block);
    
    /* Coalesce adjacent free blocks */
    memory_coalesce_blocks();
    
    memory_update_stats();
    
    EXIT_CRITICAL();
    
    return RTOS_SUCCESS;
}

/**
 * @brief Reallocate memory block with new size
 */
void* memory_realloc(void* ptr, uint32_t new_size)
{
    if(!memory_initialized)
    {
        return NULL;
    }
    
    if(ptr == NULL)
    {
        return memory_alloc(new_size);
    }
    
    if(new_size == 0)
    {
        memory_free(ptr);
        return NULL;
    }
    
    /* Get current block size */
    uint32_t current_size = memory_get_block_size(ptr);
    if(current_size == 0)
    {
        return NULL; /* Invalid pointer */
    }
    
    /* If new size fits in current block, return same pointer */
    uint32_t aligned_new_size = memory_align_size(new_size + sizeof(memory_block_t));
    if(aligned_new_size <= current_size)
    {
        return ptr;
    }
    
    /* Allocate new block */
    void* new_ptr = memory_alloc(new_size);
    if(new_ptr == NULL)
    {
        return NULL;
    }
    
    /* Copy data */
    uint32_t copy_size = (new_size < current_size - sizeof(memory_block_t)) ? 
                        new_size : current_size - sizeof(memory_block_t);
    memcpy(new_ptr, ptr, copy_size);
    
    /* Free old block */
    memory_free(ptr);
    
    return new_ptr;
}

/**
 * @brief Allocate and clear memory block
 */
void* memory_calloc(uint32_t num, uint32_t size)
{
    if(!memory_initialized || num == 0 || size == 0)
    {
        return NULL;
    }
    
    uint32_t total_size = num * size;
    
    /* Check for overflow */
    if(total_size / size != num)
    {
        return NULL;
    }
    
    void* ptr = memory_alloc(total_size);
    if(ptr != NULL)
    {
        memset(ptr, 0, total_size);
    }
    
    return ptr;
}

/**
 * @brief Get memory statistics
 */
rtos_result_t memory_get_stats(memory_stats_t* stats_out)
{
    if(!memory_initialized || stats_out == NULL)
    {
        return RTOS_INVALID_PARAM;
    }
    
    ENTER_CRITICAL();
    *stats_out = stats;
    EXIT_CRITICAL();
    
    return RTOS_SUCCESS;
}

/**
 * @brief Get current free heap size
 */
uint32_t memory_get_free_size(void)
{
    return memory_initialized ? stats.free_heap_size : 0;
}

/**
 * @brief Get current used heap size
 */
uint32_t memory_get_used_size(void)
{
    return memory_initialized ? stats.used_heap_size : 0;
}

/**
 * @brief Get size of largest available free block
 */
uint32_t memory_get_largest_free_block(void)
{
    return memory_initialized ? stats.largest_free_block : 0;
}

/**
 * @brief Check if pointer is valid allocated memory
 */
bool memory_is_valid_ptr(void* ptr)
{
    if(!memory_initialized || ptr == NULL)
    {
        return false;
    }
    
    /* Get block header */
    memory_block_t* block = (memory_block_t*)((uint8_t*)ptr - sizeof(memory_block_t));
    
    /* Check bounds */
    if((uint8_t*)block < heap || (uint8_t*)block >= heap + HEAP_SIZE)
    {
        return false;
    }
    
    /* Check magic number */
    return (block->magic == MEMORY_MAGIC_USED);
}

/**
 * @brief Get allocated block size for given pointer
 */
uint32_t memory_get_block_size(void* ptr)
{
    if(!memory_is_valid_ptr(ptr))
    {
        return 0;
    }
    
    memory_block_t* block = (memory_block_t*)((uint8_t*)ptr - sizeof(memory_block_t));
    return block->size;
}

/**
 * @brief Defragment heap memory
 */
rtos_result_t memory_defragment(void)
{
    if(!memory_initialized)
    {
        return RTOS_ERROR;
    }
    
    ENTER_CRITICAL();
    
    /* Coalesce all adjacent free blocks */
    memory_coalesce_blocks();
    
    memory_update_stats();
    
    EXIT_CRITICAL();
    
    return RTOS_SUCCESS;
}

/**
 * @brief Check heap integrity
 */
rtos_result_t memory_check_integrity(void)
{
    if(!memory_initialized)
    {
        return RTOS_ERROR;
    }
    
    ENTER_CRITICAL();
    
    /* Walk through heap and verify block structure */
    uint8_t* current = heap;
    uint32_t total_checked = 0;
    
    while(current < heap + HEAP_SIZE)
    {
        memory_block_t* block = (memory_block_t*)current;
        
        /* Check magic number */
        if(block->magic != MEMORY_MAGIC_FREE && block->magic != MEMORY_MAGIC_USED)
        {
            EXIT_CRITICAL();
            DEBUG_PRINT("Heap corruption detected: invalid magic at %p\n", block);
            return RTOS_ERROR;
        }
        
        /* Check block size */
        if(block->size < sizeof(memory_block_t) || 
           current + block->size > heap + HEAP_SIZE)
        {
            EXIT_CRITICAL();
            DEBUG_PRINT("Heap corruption detected: invalid size at %p\n", block);
            return RTOS_ERROR;
        }
        
        total_checked += block->size;
        current += block->size;
    }
    
    /* Verify total size */
    if(total_checked != HEAP_SIZE)
    {
        EXIT_CRITICAL();
        DEBUG_PRINT("Heap corruption detected: size mismatch\n");
        return RTOS_ERROR;
    }
    
    EXIT_CRITICAL();
    
    DEBUG_PRINT("Heap integrity check passed\n");
    return RTOS_SUCCESS;
}

/**
 * @brief Print memory information
 */
void memory_print_info(void)
{
    if(!memory_initialized)
    {
        DEBUG_PRINT("Memory manager not initialized\n");
        return;
    }
    
    DEBUG_PRINT("=== Memory Information ===\n");
    DEBUG_PRINT("Total Heap: %u bytes\n", stats.total_heap_size);
    DEBUG_PRINT("Used: %u bytes\n", stats.used_heap_size);
    DEBUG_PRINT("Free: %u bytes\n", stats.free_heap_size);
    DEBUG_PRINT("Largest Free Block: %u bytes\n", stats.largest_free_block);
    DEBUG_PRINT("Free Blocks: %u\n", stats.free_blocks_count);
    DEBUG_PRINT("Allocations: %u\n", stats.allocation_count);
    DEBUG_PRINT("Frees: %u\n", stats.free_count);
    DEBUG_PRINT("Failed Allocations: %u\n", stats.failed_allocations);
    DEBUG_PRINT("Max Used: %u bytes\n", stats.max_used_heap_size);
    DEBUG_PRINT("Min Free: %u bytes\n", stats.min_free_heap_size);
}

/**
 * @brief Print heap layout
 */
void memory_print_heap_layout(void)
{
    if(!memory_initialized)
    {
        return;
    }
    
    DEBUG_PRINT("=== Heap Layout ===\n");
    
    uint8_t* current = heap;
    int block_num = 0;
    
    while(current < heap + HEAP_SIZE)
    {
        memory_block_t* block = (memory_block_t*)current;
        
        DEBUG_PRINT("Block %d: %p, Size: %d, %s\n", 
                   block_num++, block, block->size,
                   (block->magic == MEMORY_MAGIC_FREE) ? "FREE" : "USED");
        
        current += block->size;
    }
}

/**
 * @brief Reset memory statistics
 */
void memory_reset_stats(void)
{
    if(!memory_initialized)
    {
        return;
    }
    
    ENTER_CRITICAL();
    
    stats.allocation_count = 0;
    stats.free_count = 0;
    stats.failed_allocations = 0;
    stats.max_used_heap_size = stats.used_heap_size;
    stats.min_free_heap_size = stats.free_heap_size;
    
    EXIT_CRITICAL();
}

/* ============================================================================
 * PRIVATE FUNCTIONS
 * ============================================================================ */

/**
 * @brief Find free block of sufficient size
 */
static memory_block_t* memory_find_free_block(uint32_t size)
{
    memory_block_t* current = free_list;
    
    /* First-fit algorithm */
    while(current != NULL)
    {
        if(current->size >= size)
        {
            return current;
        }
        current = current->next;
    }
    
    return NULL; /* No suitable block found */
}

/**
 * @brief Split block into allocated and free parts
 */
static void memory_split_block(memory_block_t* block, uint32_t size)
{
    if(block->size <= size + sizeof(memory_block_t))
    {
        return; /* Block too small to split */
    }
    
    /* Create new free block */
    memory_block_t* new_block = (memory_block_t*)((uint8_t*)block + size);
    new_block->magic = MEMORY_MAGIC_FREE;
    new_block->size = block->size - size;
    new_block->next = NULL;
    new_block->prev = NULL;
    
    /* Update original block size */
    block->size = size;
    
    /* Add new block to free list */
    memory_insert_free_block(new_block);
}

/**
 * @brief Coalesce adjacent free blocks
 */
static void memory_coalesce_blocks(void)
{
    /* Simple coalescing: walk through heap and merge adjacent free blocks */
    uint8_t* current = heap;
    
    while(current < heap + HEAP_SIZE)
    {
        memory_block_t* block = (memory_block_t*)current;
        
        if(block->magic == MEMORY_MAGIC_FREE)
        {
            /* Check if next block is also free */
            uint8_t* next_addr = current + block->size;
            
            if(next_addr < heap + HEAP_SIZE)
            {
                memory_block_t* next_block = (memory_block_t*)next_addr;
                
                if(next_block->magic == MEMORY_MAGIC_FREE)
                {
                    /* Merge blocks */
                    memory_remove_free_block(next_block);
                    block->size += next_block->size;
                    continue; /* Check again without advancing */
                }
            }
        }
        
        current += block->size;
    }
}

/**
 * @brief Insert block into free list
 */
static void memory_insert_free_block(memory_block_t* block)
{
    block->next = free_list;
    block->prev = NULL;
    
    if(free_list != NULL)
    {
        free_list->prev = block;
    }
    
    free_list = block;
}

/**
 * @brief Remove block from free list
 */
static void memory_remove_free_block(memory_block_t* block)
{
    if(block->prev != NULL)
    {
        block->prev->next = block->next;
    }
    else
    {
        free_list = block->next;
    }
    
    if(block->next != NULL)
    {
        block->next->prev = block->prev;
    }
    
    block->next = NULL;
    block->prev = NULL;
}

/**
 * @brief Align size to memory boundary
 */
static uint32_t memory_align_size(uint32_t size)
{
    return (size + MEMORY_ALIGNMENT - 1) & ~(MEMORY_ALIGNMENT - 1);
}

/**
 * @brief Update memory statistics
 */
static void memory_update_stats(void)
{
    /* Count free blocks and find largest */
    stats.free_blocks_count = 0;
    stats.largest_free_block = 0;
    
    memory_block_t* current = free_list;
    while(current != NULL)
    {
        stats.free_blocks_count++;
        
        if(current->size > stats.largest_free_block)
        {
            stats.largest_free_block = current->size - sizeof(memory_block_t);
        }
        
        current = current->next;
    }
}