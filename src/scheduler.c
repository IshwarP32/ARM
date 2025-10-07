/**
 * @file scheduler.c
 * @brief Priority-based Preemptive Scheduler Implementation
 * @author Team Member 2 - Scheduler Implementation
 * @date 2024
 * 
 * This module implements a priority-based preemptive scheduler with round-robin
 * scheduling for tasks of equal priority.
 */

#include "scheduler.h"
#include "timer_manager.h"

/* ============================================================================
 * GLOBAL VARIABLES
 * ============================================================================ */
static tcb_t* ready_queues[SCHEDULER_PRIORITY_LEVELS];  /* Ready task queues by priority */
static bool scheduler_running = false;                 /* Scheduler state */
static bool scheduler_locked = false;                  /* Scheduler lock state */
static uint8_t current_priority = 0;                   /* Current executing priority */
static scheduler_stats_t stats;                        /* Scheduler statistics */
static uint8_t idle_task_id = 0xFF;                   /* Idle task ID */

/* ============================================================================
 * PRIVATE FUNCTION PROTOTYPES
 * ============================================================================ */
static tcb_t* scheduler_find_highest_priority_task(void);
static void scheduler_update_statistics(void);
static void scheduler_round_robin_next(uint8_t priority);

/* ============================================================================
 * PUBLIC FUNCTIONS
 * ============================================================================ */

/**
 * @brief Initialize the scheduler
 */
rtos_result_t scheduler_init(void)
{
    /* Initialize ready queues */
    for(int i = 0; i < SCHEDULER_PRIORITY_LEVELS; i++)
    {
        ready_queues[i] = NULL;
    }
    
    /* Initialize state */
    scheduler_running = false;
    scheduler_locked = false;
    current_priority = 0;
    
    /* Initialize statistics */
    memset(&stats, 0, sizeof(scheduler_stats_t));
    
    /* Create idle task */
    idle_task_id = task_create(scheduler_idle_task, "IDLE", PRIORITY_IDLE, MIN_STACK_SIZE);
    
    DEBUG_PRINT("Scheduler initialized\n");
    return RTOS_SUCCESS;
}

/**
 * @brief Start the scheduler
 */
void scheduler_start(void)
{
    DEBUG_PRINT("Starting scheduler...\n");
    
    scheduler_running = true;
    
    /* Find first task to run */
    tcb_t* first_task = scheduler_get_next_task();
    
    if(first_task != NULL)
    {
        /* Set first task as running */
        task_set_state(first_task->task_id, TASK_STATE_RUNNING);
        
        DEBUG_PRINT("Scheduler started with first task: %s (ID: %d)\n", first_task->task_name, first_task->task_id);
    }
    else
    {
        DEBUG_PRINT("No tasks ready to run!\n");
    }
    
    /* NOTE: In this simplified implementation, we don't call task functions directly.
     * Instead, the main loop will repeatedly call scheduler_run_next_task() to execute
     * each task for one iteration. This allows all tasks to run without getting stuck
     * in infinite loops. */
}

/**
 * @brief Get next task to run
 */
tcb_t* scheduler_get_next_task(void)
{
    if(scheduler_locked)
    {
        /* Return current task if scheduler is locked */
        return task_get_current();
    }
    
    /* Find highest priority ready task */
    tcb_t* next_task = scheduler_find_highest_priority_task();
    
    if(next_task == NULL)
    {
        /* No ready tasks, return idle task */
        next_task = task_get_tcb(idle_task_id);
    }
    
    return next_task;
}

/**
 * @brief Perform context switch
 */
void scheduler_switch_context(void)
{
    if(!scheduler_running || scheduler_locked)
    {
        return;
    }
    
    tcb_t* current_task = task_get_current();
    tcb_t* next_task = scheduler_get_next_task();
    
    if(next_task == NULL || next_task == current_task)
    {
        return; /* No switch needed */
    }
    
    /* Update current task state */
    if(current_task != NULL && current_task->state == TASK_STATE_RUNNING)
    {
        current_task->state = TASK_STATE_READY;
        current_task->time_slice_remaining = TIME_SLICE_MS; /* Reset time slice */
    }
    
    /* Set next task as running */
    task_set_state(next_task->task_id, TASK_STATE_RUNNING);
    
    /* Update statistics */
    if(current_task != NULL)
    {
        current_task->context_switches++;
    }
    stats.total_context_switches++;
    
    DEBUG_PRINT("Context switch: %s -> %s\n", 
               current_task ? current_task->task_name : "NULL",
               next_task->task_name);
    
    /* In real implementation, this would save/restore CPU context */
    /* For simulation, we just update the task states */
}

/**
 * @brief Yield CPU to next task
 */
void scheduler_yield(void)
{
    tcb_t* current_task = task_get_current();
    
    if(current_task != NULL)
    {
        /* Reset time slice */
        current_task->time_slice_remaining = TIME_SLICE_MS;
        
        /* Trigger context switch */
        scheduler_switch_context();
    }
}

/**
 * @brief Add task to ready queue
 */
rtos_result_t scheduler_add_ready_task(tcb_t* tcb)
{
    if(tcb == NULL || tcb->priority >= SCHEDULER_PRIORITY_LEVELS)
    {
        return RTOS_INVALID_PARAM;
    }
    
    /* Add to appropriate priority queue (simple linked list) */
    uint8_t priority = tcb->priority;
    
    if(ready_queues[priority] == NULL)
    {
        /* First task in this priority level */
        ready_queues[priority] = tcb;
        tcb->next = tcb;
        tcb->prev = tcb;
    }
    else
    {
        /* Add to end of circular list */
        tcb_t* tail = ready_queues[priority]->prev;
        tcb->next = ready_queues[priority];
        tcb->prev = tail;
        tail->next = tcb;
        ready_queues[priority]->prev = tcb;
    }
    
    return RTOS_SUCCESS;
}

/**
 * @brief Remove task from ready queue
 */
rtos_result_t scheduler_remove_ready_task(tcb_t* tcb)
{
    if(tcb == NULL || tcb->priority >= SCHEDULER_PRIORITY_LEVELS)
    {
        return RTOS_INVALID_PARAM;
    }
    
    uint8_t priority = tcb->priority;
    
    if(ready_queues[priority] == NULL)
    {
        return RTOS_ERROR; /* Task not in queue */
    }
    
    if(tcb->next == tcb)
    {
        /* Only task in queue */
        ready_queues[priority] = NULL;
    }
    else
    {
        /* Remove from circular list */
        tcb->prev->next = tcb->next;
        tcb->next->prev = tcb->prev;
        
        if(ready_queues[priority] == tcb)
        {
            ready_queues[priority] = tcb->next;
        }
    }
    
    tcb->next = NULL;
    tcb->prev = NULL;
    
    return RTOS_SUCCESS;
}

/**
 * @brief Handle timer tick
 */
void scheduler_tick(void)
{
    if(!scheduler_running)
    {
        return;
    }
    
    stats.total_scheduler_calls++;
    
    /* Update task delays */
    task_update_delays();
    
    /* Update current task time slice */
    tcb_t* current_task = task_get_current();
    if(current_task != NULL && current_task->state == TASK_STATE_RUNNING)
    {
        if(current_task->time_slice_remaining > 0)
        {
            current_task->time_slice_remaining--;
        }
        
        /* Check if time slice expired */
        if(current_task->time_slice_remaining == 0)
        {
            /* Move to next task in round-robin */
            scheduler_round_robin_next(current_task->priority);
            scheduler_switch_context();
        }
    }
    
    scheduler_update_statistics();
}

/**
 * @brief Check if scheduler is running
 */
bool scheduler_is_running(void)
{
    return scheduler_running;
}

/**
 * @brief Get scheduler statistics
 */
rtos_result_t scheduler_get_stats(scheduler_stats_t* stats_out)
{
    if(stats_out == NULL)
    {
        return RTOS_INVALID_PARAM;
    }
    
    *stats_out = stats;
    return RTOS_SUCCESS;
}

/**
 * @brief Reset scheduler statistics
 */
void scheduler_reset_stats(void)
{
    memset(&stats, 0, sizeof(scheduler_stats_t));
}

/**
 * @brief Lock scheduler
 */
void scheduler_lock(void)
{
    ENTER_CRITICAL();
    scheduler_locked = true;
    EXIT_CRITICAL();
}

/**
 * @brief Unlock scheduler
 */
void scheduler_unlock(void)
{
    ENTER_CRITICAL();
    scheduler_locked = false;
    EXIT_CRITICAL();
    
    /* Check if context switch is needed */
    scheduler_switch_context();
}

/**
 * @brief Check if scheduler is locked
 */
bool scheduler_is_locked(void)
{
    return scheduler_locked;
}

/**
 * @brief Print scheduler information
 */
void scheduler_print_info(void)
{
    DEBUG_PRINT("=== Scheduler Information ===\n");
    DEBUG_PRINT("Running: %s\n", scheduler_running ? "Yes" : "No");
    DEBUG_PRINT("Locked: %s\n", scheduler_locked ? "Yes" : "No");
    DEBUG_PRINT("Context Switches: %u\n", stats.total_context_switches);
    DEBUG_PRINT("Scheduler Calls: %u\n", stats.total_scheduler_calls);
    
    /* Show ready queues */
    for(int i = SCHEDULER_PRIORITY_LEVELS - 1; i >= 0; i--)
    {
        if(ready_queues[i] != NULL)
        {
            DEBUG_PRINT("Priority %d: %s\n", i, ready_queues[i]->task_name);
        }
    }
}

/**
 * @brief Idle task function
 */
void scheduler_idle_task(void)
{
    /* Idle processing - could include power management */
    stats.idle_time_percentage++;
    
    /* In real system, could enter low power mode */
    // __WFI(); /* Wait for interrupt */
    
    DEBUG_PRINT("[IDLE] Idle task running\n");
}

/**
 * @brief Run the next ready task for one iteration
 * @note This is a simplified scheduler that runs each task once per call
 */
void scheduler_run_next_task(void)
{
    if(!scheduler_running)
    {
        return;
    }
    
    /* Get next task to run */
    tcb_t* next_task = scheduler_get_next_task();
    
    if(next_task == NULL)
    {
        /* No ready tasks, run idle */
        scheduler_idle_task();
        return;
    }
    
    /* Check if task is blocked or suspended */
    if(next_task->state == TASK_STATE_BLOCKED || next_task->state == TASK_STATE_SUSPENDED)
    {
        /* Skip this task */
        return;
    }
    
    /* Set task as running */
    task_set_state(next_task->task_id, TASK_STATE_RUNNING);
    
    /* Execute the task function once */
    if(next_task->task_function != NULL)
    {
        next_task->task_function();
    }
    
    /* Set task back to ready (unless it was blocked/suspended by itself) */
    if(next_task->state == TASK_STATE_RUNNING)
    {
        next_task->state = TASK_STATE_READY;
    }
    
    /* Move to next task in round-robin */
    scheduler_round_robin_next(next_task->priority);
    
    /* Update statistics */
    stats.total_context_switches++;
}

/* ============================================================================
 * PRIVATE FUNCTIONS
 * ============================================================================ */

/**
 * @brief Find highest priority ready task
 */
static tcb_t* scheduler_find_highest_priority_task(void)
{
    /* Search from highest to lowest priority */
    for(int i = SCHEDULER_PRIORITY_LEVELS - 1; i >= 0; i--)
    {
        if(ready_queues[i] != NULL)
        {
            return ready_queues[i];
        }
    }
    
    return NULL;
}

/**
 * @brief Update scheduler statistics
 */
static void scheduler_update_statistics(void)
{
    /* Calculate CPU utilization */
    if(stats.total_scheduler_calls > 0)
    {
        stats.cpu_utilization = 100 - (stats.idle_time_percentage * 100 / stats.total_scheduler_calls);
    }
}

/**
 * @brief Move to next task in round-robin for given priority
 */
static void scheduler_round_robin_next(uint8_t priority)
{
    if(priority >= SCHEDULER_PRIORITY_LEVELS || ready_queues[priority] == NULL)
    {
        return;
    }
    
    /* Move to next task in circular list */
    ready_queues[priority] = ready_queues[priority]->next;
}