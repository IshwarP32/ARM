/**
 * @file scheduler.h
 * @brief Priority-based Preemptive Scheduler Interface
 * @author Team Member 2 - Scheduler Implementation
 * @date 2024
 * 
 * This module implements a priority-based preemptive scheduler with round-robin
 * scheduling for tasks of equal priority. It handles task switching and 
 * scheduling decisions.
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "rtos_config.h"
#include "task_manager.h"

/* ============================================================================
 * SCHEDULER CONFIGURATION
 * ============================================================================ */
#define SCHEDULER_PRIORITY_LEVELS   5   /* 0-4 priority levels */

/* ============================================================================
 * SCHEDULER STATISTICS
 * ============================================================================ */
typedef struct {
    uint32_t total_context_switches;
    uint32_t total_scheduler_calls;
    uint32_t idle_time_percentage;
    uint32_t cpu_utilization;
} scheduler_stats_t;

/* ============================================================================
 * FUNCTION PROTOTYPES
 * ============================================================================ */

/**
 * @brief Initialize the scheduler
 * @return rtos_result_t Success or error code
 */
rtos_result_t scheduler_init(void);

/**
 * @brief Start the scheduler (begins task execution)
 * @note This function never returns in normal operation
 */
void scheduler_start(void);

/**
 * @brief Schedule next task to run
 * @return tcb_t* Pointer to next task's TCB or NULL if no ready tasks
 */
tcb_t* scheduler_get_next_task(void);

/**
 * @brief Perform context switch to next ready task
 * @note Called by timer interrupt or yield
 */
void scheduler_switch_context(void);

/**
 * @brief Yield CPU to next ready task
 * @note Voluntarily give up CPU time
 */
void scheduler_yield(void);

/**
 * @brief Add task to ready queue
 * @param tcb Pointer to task control block
 * @return rtos_result_t Success or error code
 */
rtos_result_t scheduler_add_ready_task(tcb_t* tcb);

/**
 * @brief Remove task from ready queue
 * @param tcb Pointer to task control block
 * @return rtos_result_t Success or error code
 */
rtos_result_t scheduler_remove_ready_task(tcb_t* tcb);

/**
 * @brief Handle timer tick (called from timer ISR)
 * @note Updates time slices and triggers scheduling
 */
void scheduler_tick(void);

/**
 * @brief Check if scheduler is running
 * @return bool True if scheduler is active
 */
bool scheduler_is_running(void);

/**
 * @brief Get scheduler statistics
 * @param stats Pointer to statistics structure
 * @return rtos_result_t Success or error code
 */
rtos_result_t scheduler_get_stats(scheduler_stats_t* stats);

/**
 * @brief Reset scheduler statistics
 */
void scheduler_reset_stats(void);

/**
 * @brief Lock scheduler (disable preemption)
 * @note Use carefully to avoid deadlocks
 */
void scheduler_lock(void);

/**
 * @brief Unlock scheduler (enable preemption)
 */
void scheduler_unlock(void);

/**
 * @brief Get current scheduler lock state
 * @return bool True if scheduler is locked
 */
bool scheduler_is_locked(void);

/**
 * @brief Print scheduler information (for debugging)
 */
void scheduler_print_info(void);

/**
 * @brief Idle task function (runs when no other tasks are ready)
 */
void scheduler_idle_task(void);

#endif /* SCHEDULER_H */