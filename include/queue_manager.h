/**
 * @file queue_manager.h
 * @brief Queue and Semaphore Management Interface
 * @author Team Member 3 - Queue/Semaphore Management
 * @date 2024
 * 
 * This module implements message queues and semaphores for inter-task
 * communication and synchronization.
 */

#ifndef QUEUE_MANAGER_H
#define QUEUE_MANAGER_H

#include "rtos_config.h"

/* ============================================================================
 * QUEUE CONFIGURATION
 * ============================================================================ */
#define QUEUE_ITEM_SIZE             sizeof(uint32_t)    /* Size of each queue item */
#define QUEUE_TIMEOUT_INFINITE      0xFFFFFFFF          /* Infinite timeout */

/* ============================================================================
 * SEMAPHORE CONFIGURATION
 * ============================================================================ */
#define MAX_SEMAPHORES              4                   /* Maximum number of semaphores */
#define SEMAPHORE_MAX_COUNT         255                 /* Maximum semaphore count */

/* ============================================================================
 * QUEUE STRUCTURE
 * ============================================================================ */
typedef struct {
    uint8_t queue_id;                   /* Queue identifier */
    uint32_t* buffer;                   /* Queue buffer */
    uint32_t size;                      /* Queue size (number of items) */
    uint32_t head;                      /* Head index */
    uint32_t tail;                      /* Tail index */
    uint32_t count;                     /* Current number of items */
    bool is_active;                     /* Queue active status */
    
    /* Waiting task lists */
    uint8_t send_waiting_tasks[MAX_TASKS];      /* Tasks waiting to send */
    uint8_t receive_waiting_tasks[MAX_TASKS];   /* Tasks waiting to receive */
    uint8_t send_waiting_count;
    uint8_t receive_waiting_count;
} queue_t;

/* ============================================================================
 * SEMAPHORE STRUCTURE
 * ============================================================================ */
typedef struct {
    uint8_t semaphore_id;               /* Semaphore identifier */
    uint8_t count;                      /* Current count */
    uint8_t max_count;                  /* Maximum count */
    bool is_active;                     /* Semaphore active status */
    
    /* Waiting task list */
    uint8_t waiting_tasks[MAX_TASKS];   /* Tasks waiting for semaphore */
    uint8_t waiting_count;              /* Number of waiting tasks */
} semaphore_t;

/* ============================================================================
 * FUNCTION PROTOTYPES - QUEUE MANAGEMENT
 * ============================================================================ */

/**
 * @brief Initialize the queue manager
 * @return rtos_result_t Success or error code
 */
rtos_result_t queue_manager_init(void);

/**
 * @brief Create a message queue
 * @param queue_id Queue identifier (0-3)
 * @param size Queue size (number of items)
 * @return queue_result_t Success or error code
 */
queue_result_t queue_create(uint8_t queue_id, uint32_t size);

/**
 * @brief Delete a message queue
 * @param queue_id Queue identifier
 * @return queue_result_t Success or error code
 */
queue_result_t queue_delete(uint8_t queue_id);

/**
 * @brief Send data to queue
 * @param queue_id Queue identifier
 * @param data Pointer to data to send
 * @param timeout_ms Timeout in milliseconds (0 = no wait, QUEUE_TIMEOUT_INFINITE = wait forever)
 * @return queue_result_t Success or error code
 */
queue_result_t queue_send(uint8_t queue_id, const void* data, uint32_t timeout_ms);

/**
 * @brief Receive data from queue
 * @param queue_id Queue identifier
 * @param data Pointer to buffer for received data
 * @param timeout_ms Timeout in milliseconds
 * @return queue_result_t Success or error code
 */
queue_result_t queue_receive(uint8_t queue_id, void* data, uint32_t timeout_ms);

/**
 * @brief Peek at queue data without removing it
 * @param queue_id Queue identifier
 * @param data Pointer to buffer for peeked data
 * @return queue_result_t Success or error code
 */
queue_result_t queue_peek(uint8_t queue_id, void* data);

/**
 * @brief Get number of items in queue
 * @param queue_id Queue identifier
 * @return uint32_t Number of items (0xFFFFFFFF if error)
 */
uint32_t queue_get_count(uint8_t queue_id);

/**
 * @brief Get available space in queue
 * @param queue_id Queue identifier
 * @return uint32_t Available space (0xFFFFFFFF if error)
 */
uint32_t queue_get_space(uint8_t queue_id);

/**
 * @brief Check if queue is full
 * @param queue_id Queue identifier
 * @return bool True if full
 */
bool queue_is_full(uint8_t queue_id);

/**
 * @brief Check if queue is empty
 * @param queue_id Queue identifier
 * @return bool True if empty
 */
bool queue_is_empty(uint8_t queue_id);

/* ============================================================================
 * FUNCTION PROTOTYPES - SEMAPHORE MANAGEMENT
 * ============================================================================ */

/**
 * @brief Create a semaphore
 * @param semaphore_id Semaphore identifier (0-3)
 * @param initial_count Initial count
 * @param max_count Maximum count
 * @return rtos_result_t Success or error code
 */
rtos_result_t semaphore_create(uint8_t semaphore_id, uint8_t initial_count, uint8_t max_count);

/**
 * @brief Delete a semaphore
 * @param semaphore_id Semaphore identifier
 * @return rtos_result_t Success or error code
 */
rtos_result_t semaphore_delete(uint8_t semaphore_id);

/**
 * @brief Take/acquire a semaphore
 * @param semaphore_id Semaphore identifier
 * @param timeout_ms Timeout in milliseconds
 * @return rtos_result_t Success or error code
 */
rtos_result_t semaphore_take(uint8_t semaphore_id, uint32_t timeout_ms);

/**
 * @brief Give/release a semaphore
 * @param semaphore_id Semaphore identifier
 * @return rtos_result_t Success or error code
 */
rtos_result_t semaphore_give(uint8_t semaphore_id);

/**
 * @brief Get current semaphore count
 * @param semaphore_id Semaphore identifier
 * @return uint8_t Current count (0xFF if error)
 */
uint8_t semaphore_get_count(uint8_t semaphore_id);

/* ============================================================================
 * FUNCTION PROTOTYPES - UTILITY
 * ============================================================================ */

/**
 * @brief Print queue information (for debugging)
 * @param queue_id Queue identifier (0xFF for all queues)
 */
void queue_print_info(uint8_t queue_id);

/**
 * @brief Print semaphore information (for debugging)
 * @param semaphore_id Semaphore identifier (0xFF for all semaphores)
 */
void semaphore_print_info(uint8_t semaphore_id);

/**
 * @brief Handle timeouts for waiting tasks (called by timer)
 */
void queue_manager_handle_timeouts(void);

#endif /* QUEUE_MANAGER_H */