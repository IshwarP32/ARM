/**
 * @file task_manager.h
 * @brief Task Management Interface
 * @author Team Member 1 - Task Management
 * @date 2024
 * 
 * This module handles task creation, deletion, and state management.
 * It provides the core task control block (TCB) structure and related functions.
 */

#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "rtos_config.h"

/* ============================================================================
 * TASK CONTROL BLOCK (TCB) STRUCTURE
 * ============================================================================ */
typedef struct task_control_block {
    /* Task identification */
    uint8_t task_id;
    char task_name[MAX_TASK_NAME_LENGTH];
    
    /* Task function and parameters */
    void (*task_function)(void);
    void* task_parameter;
    
    /* Task priority and state */
    uint8_t priority;
    task_state_t state;
    
    /* Stack management */
    uint32_t* stack_pointer;
    uint32_t* stack_base;
    uint32_t stack_size;
    
    /* Timing information */
    uint32_t time_slice_remaining;
    uint32_t delay_ticks;
    
    /* Linked list pointers for scheduler */
    struct task_control_block* next;
    struct task_control_block* prev;
    
    /* Task statistics */
    uint32_t execution_time;
    uint32_t context_switches;
    
} tcb_t;

/* ============================================================================
 * FUNCTION PROTOTYPES
 * ============================================================================ */

/**
 * @brief Initialize the task manager
 * @return rtos_result_t Success or error code
 */
rtos_result_t task_manager_init(void);

/**
 * @brief Create a new task
 * @param task_function Pointer to task function
 * @param task_name Task name (max 15 characters + null terminator)
 * @param priority Task priority (0-4)
 * @param stack_size Stack size in bytes
 * @return uint8_t Task ID (0xFF if failed)
 */
uint8_t task_create(void (*task_function)(void), 
                   const char* task_name, 
                   uint8_t priority, 
                   uint32_t stack_size);

/**
 * @brief Delete a task
 * @param task_id Task ID to delete
 * @return rtos_result_t Success or error code
 */
rtos_result_t task_delete(uint8_t task_id);

/**
 * @brief Suspend a task
 * @param task_id Task ID to suspend
 * @return rtos_result_t Success or error code
 */
rtos_result_t task_suspend(uint8_t task_id);

/**
 * @brief Resume a suspended task
 * @param task_id Task ID to resume
 * @return rtos_result_t Success or error code
 */
rtos_result_t task_resume(uint8_t task_id);

/**
 * @brief Put current task to sleep for specified ticks
 * @param delay_ticks Number of system ticks to delay
 */
void task_delay(uint32_t delay_ticks);

/**
 * @brief Get task control block by ID
 * @param task_id Task ID
 * @return tcb_t* Pointer to TCB or NULL if not found
 */
tcb_t* task_get_tcb(uint8_t task_id);

/**
 * @brief Get current running task
 * @return tcb_t* Pointer to current task TCB
 */
tcb_t* task_get_current(void);

/**
 * @brief Set task state
 * @param task_id Task ID
 * @param new_state New task state
 * @return rtos_result_t Success or error code
 */
rtos_result_t task_set_state(uint8_t task_id, task_state_t new_state);

/**
 * @brief Get task state
 * @param task_id Task ID
 * @return task_state_t Current task state
 */
task_state_t task_get_state(uint8_t task_id);

/**
 * @brief Update task delay counters (called by timer ISR)
 */
void task_update_delays(void);

/**
 * @brief Get number of active tasks
 * @return uint8_t Number of active tasks
 */
uint8_t task_get_count(void);

/**
 * @brief Print task information (for debugging)
 * @param task_id Task ID (0xFF for all tasks)
 */
void task_print_info(uint8_t task_id);

#endif /* TASK_MANAGER_H */