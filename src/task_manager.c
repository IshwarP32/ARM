/**
 * @file task_manager.c
 * @brief Task Management Implementation
 * @author Team Member 1 - Task Management
 * @date 2024
 * 
 * This module implements task creation, deletion, and state management.
 * It maintains the task control blocks and provides task manipulation functions.
 */

#include "task_manager.h"
#include "memory_manager.h"
#include "scheduler.h"

/* ============================================================================
 * GLOBAL VARIABLES
 * ============================================================================ */
static tcb_t task_table[MAX_TASKS];     /* Task control block table */
static uint8_t task_count = 0;          /* Number of active tasks */
static uint8_t current_task_id = 0xFF;  /* Currently running task ID */
static uint8_t next_task_id = 0;        /* Next available task ID */

/* ============================================================================
 * PRIVATE FUNCTION PROTOTYPES
 * ============================================================================ */
static void task_stack_init(tcb_t* tcb, void (*task_function)(void));
static uint8_t task_get_free_id(void);

/* ============================================================================
 * PUBLIC FUNCTIONS
 * ============================================================================ */

/**
 * @brief Initialize the task manager
 */
rtos_result_t task_manager_init(void)
{
    /* Clear task table */
    memset(task_table, 0, sizeof(task_table));
    
    /* Initialize all tasks as deleted */
    for(int i = 0; i < MAX_TASKS; i++)
    {
        task_table[i].task_id = 0xFF;
        task_table[i].state = TASK_STATE_DELETED;
    }
    
    task_count = 0;
    current_task_id = 0xFF;
    next_task_id = 0;
    
    DEBUG_PRINT("Task Manager initialized\n");
    return RTOS_SUCCESS;
}

/**
 * @brief Create a new task
 */
uint8_t task_create(void (*task_function)(void), 
                   const char* task_name, 
                   uint8_t priority, 
                   uint32_t stack_size)
{
    /* Validate parameters */
    if(task_function == NULL || task_name == NULL)
    {
        return 0xFF;
    }
    
    if(priority > PRIORITY_CRITICAL || stack_size < MIN_STACK_SIZE)
    {
        return 0xFF;
    }
    
    if(task_count >= MAX_TASKS)
    {
        return 0xFF;
    }
    
    /* Get free task ID */
    uint8_t task_id = task_get_free_id();
    if(task_id == 0xFF)
    {
        return 0xFF;
    }
    
    /* Get TCB pointer */
    tcb_t* tcb = &task_table[task_id];
    
    /* Allocate stack memory */
    uint32_t* stack = (uint32_t*)memory_alloc(stack_size);
    if(stack == NULL)
    {
        return 0xFF;
    }
    
    /* Initialize TCB */
    tcb->task_id = task_id;
    strncpy(tcb->task_name, task_name, MAX_TASK_NAME_LENGTH - 1);
    tcb->task_name[MAX_TASK_NAME_LENGTH - 1] = '\0';
    tcb->task_function = task_function;
    tcb->priority = priority;
    tcb->state = TASK_STATE_READY;
    tcb->stack_base = stack;
    tcb->stack_size = stack_size;
    tcb->time_slice_remaining = TIME_SLICE_MS;
    tcb->delay_ticks = 0;
    tcb->execution_time = 0;
    tcb->context_switches = 0;
    
    /* Initialize stack */
    task_stack_init(tcb, task_function);
    
    /* Update counters */
    task_count++;
    
    /* Add task to scheduler ready queue */
    scheduler_add_ready_task(tcb);
    
    DEBUG_PRINT("Task '%s' created with ID %d, Priority %d\n", 
                task_name, task_id, priority);
    
    return task_id;
}

/**
 * @brief Delete a task
 */
rtos_result_t task_delete(uint8_t task_id)
{
    if(task_id >= MAX_TASKS)
    {
        return RTOS_INVALID_PARAM;
    }
    
    tcb_t* tcb = &task_table[task_id];
    
    if(tcb->state == TASK_STATE_DELETED)
    {
        return RTOS_ERROR;
    }
    
    /* Free stack memory */
    if(tcb->stack_base != NULL)
    {
        memory_free(tcb->stack_base);
    }
    
    /* Mark as deleted */
    tcb->state = TASK_STATE_DELETED;
    tcb->task_id = 0xFF;
    
    /* Update counter */
    if(task_count > 0)
    {
        task_count--;
    }
    
    DEBUG_PRINT("Task %d deleted\n", task_id);
    
    return RTOS_SUCCESS;
}

/**
 * @brief Suspend a task
 */
rtos_result_t task_suspend(uint8_t task_id)
{
    if(task_id >= MAX_TASKS)
    {
        return RTOS_INVALID_PARAM;
    }
    
    tcb_t* tcb = &task_table[task_id];
    
    if(tcb->state == TASK_STATE_DELETED)
    {
        return RTOS_ERROR;
    }
    
    tcb->state = TASK_STATE_SUSPENDED;
    
    DEBUG_PRINT("Task %d suspended\n", task_id);
    
    return RTOS_SUCCESS;
}

/**
 * @brief Resume a suspended task
 */
rtos_result_t task_resume(uint8_t task_id)
{
    if(task_id >= MAX_TASKS)
    {
        return RTOS_INVALID_PARAM;
    }
    
    tcb_t* tcb = &task_table[task_id];
    
    if(tcb->state != TASK_STATE_SUSPENDED)
    {
        return RTOS_ERROR;
    }
    
    tcb->state = TASK_STATE_READY;
    
    DEBUG_PRINT("Task %d resumed\n", task_id);
    
    return RTOS_SUCCESS;
}

/**
 * @brief Put current task to sleep
 */
void task_delay(uint32_t delay_ticks)
{
    if(current_task_id < MAX_TASKS)
    {
        tcb_t* tcb = &task_table[current_task_id];
        tcb->delay_ticks = delay_ticks;
        tcb->state = TASK_STATE_BLOCKED;
        
        /* Trigger context switch */
        // This would trigger scheduler in real implementation
    }
}

/**
 * @brief Get task control block by ID
 */
tcb_t* task_get_tcb(uint8_t task_id)
{
    if(task_id >= MAX_TASKS)
    {
        return NULL;
    }
    
    if(task_table[task_id].state == TASK_STATE_DELETED)
    {
        return NULL;
    }
    
    return &task_table[task_id];
}

/**
 * @brief Get current running task
 */
tcb_t* task_get_current(void)
{
    if(current_task_id >= MAX_TASKS)
    {
        return NULL;
    }
    
    return &task_table[current_task_id];
}

/**
 * @brief Set task state
 */
rtos_result_t task_set_state(uint8_t task_id, task_state_t new_state)
{
    if(task_id >= MAX_TASKS)
    {
        return RTOS_INVALID_PARAM;
    }
    
    tcb_t* tcb = &task_table[task_id];
    
    if(tcb->state == TASK_STATE_DELETED)
    {
        return RTOS_ERROR;
    }
    
    tcb->state = new_state;
    
    /* Update current task ID if setting to running */
    if(new_state == TASK_STATE_RUNNING)
    {
        current_task_id = task_id;
    }
    
    return RTOS_SUCCESS;
}

/**
 * @brief Get task state
 */
task_state_t task_get_state(uint8_t task_id)
{
    if(task_id >= MAX_TASKS)
    {
        return TASK_STATE_DELETED;
    }
    
    return task_table[task_id].state;
}

/**
 * @brief Update task delay counters
 */
void task_update_delays(void)
{
    for(int i = 0; i < MAX_TASKS; i++)
    {
        tcb_t* tcb = &task_table[i];
        
        if(tcb->state == TASK_STATE_BLOCKED && tcb->delay_ticks > 0)
        {
            tcb->delay_ticks--;
            
            if(tcb->delay_ticks == 0)
            {
                tcb->state = TASK_STATE_READY;
            }
        }
    }
}

/**
 * @brief Get number of active tasks
 */
uint8_t task_get_count(void)
{
    return task_count;
}

/**
 * @brief Print task information
 */
void task_print_info(uint8_t task_id)
{
    if(task_id == 0xFF)
    {
        /* Print all tasks */
        DEBUG_PRINT("=== Task Information ===\n");
        DEBUG_PRINT("Total Tasks: %d\n", task_count);
        
        for(int i = 0; i < MAX_TASKS; i++)
        {
            tcb_t* tcb = &task_table[i];
            if(tcb->state != TASK_STATE_DELETED)
            {
                DEBUG_PRINT("ID:%d Name:'%s' Priority:%d State:%d\n",
                           tcb->task_id, tcb->task_name, tcb->priority, tcb->state);
            }
        }
    }
    else if(task_id < MAX_TASKS)
    {
        tcb_t* tcb = &task_table[task_id];
        if(tcb->state != TASK_STATE_DELETED)
        {
            DEBUG_PRINT("Task %d: '%s' Priority:%d State:%d Switches:%u\n",
                       tcb->task_id, tcb->task_name, tcb->priority, 
                       tcb->state, tcb->context_switches);
        }
    }
}

/* ============================================================================
 * PRIVATE FUNCTIONS
 * ============================================================================ */

/**
 * @brief Initialize task stack
 */
static void task_stack_init(tcb_t* tcb, void (*task_function)(void))
{
    /* For ARM Cortex-M, stack grows downward */
    uint32_t* stack_top = tcb->stack_base + (tcb->stack_size / sizeof(uint32_t)) - 1;
    
    /* Initialize stack with task function address */
    /* This is a simplified version - real implementation would set up
       proper ARM Cortex-M exception stack frame */
    tcb->stack_pointer = stack_top;
    
    /* Store function pointer at top of stack */
    *stack_top = (uint32_t)task_function;
}

/**
 * @brief Get next available task ID
 */
static uint8_t task_get_free_id(void)
{
    for(int i = 0; i < MAX_TASKS; i++)
    {
        if(task_table[i].state == TASK_STATE_DELETED)
        {
            return i;
        }
    }
    
    return 0xFF; /* No free slot */
}