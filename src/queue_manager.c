/**
 * @file queue_manager.c
 * @brief Queue and Semaphore Management Implementation
 * @author Team Member 3 - Queue/Semaphore Management
 * @date 2024
 * 
 * This module implements message queues and semaphores for inter-task
 * communication and synchronization.
 */

#include "queue_manager.h"
#include "memory_manager.h"
#include "task_manager.h"

/* ============================================================================
 * GLOBAL VARIABLES
 * ============================================================================ */
static queue_t queues[MAX_QUEUES];              /* Queue array */
static semaphore_t semaphores[MAX_SEMAPHORES];  /* Semaphore array */
static bool queue_manager_initialized = false;

/* ============================================================================
 * PRIVATE FUNCTION PROTOTYPES
 * ============================================================================ */
static void queue_add_waiting_task(uint8_t queue_id, uint8_t task_id, bool is_sender);
static void queue_remove_waiting_task(uint8_t queue_id, uint8_t task_id, bool is_sender);
static void queue_wake_waiting_task(uint8_t queue_id, bool is_sender);
static void semaphore_add_waiting_task(uint8_t semaphore_id, uint8_t task_id);
static void semaphore_remove_waiting_task(uint8_t semaphore_id, uint8_t task_id);
static void semaphore_wake_waiting_task(uint8_t semaphore_id);

/* ============================================================================
 * PUBLIC FUNCTIONS - QUEUE MANAGER
 * ============================================================================ */

/**
 * @brief Initialize the queue manager
 */
rtos_result_t queue_manager_init(void)
{
    /* Initialize queues */
    for(int i = 0; i < MAX_QUEUES; i++)
    {
        queues[i].queue_id = i;
        queues[i].buffer = NULL;
        queues[i].size = 0;
        queues[i].head = 0;
        queues[i].tail = 0;
        queues[i].count = 0;
        queues[i].is_active = false;
        queues[i].send_waiting_count = 0;
        queues[i].receive_waiting_count = 0;
    }
    
    /* Initialize semaphores */
    for(int i = 0; i < MAX_SEMAPHORES; i++)
    {
        semaphores[i].semaphore_id = i;
        semaphores[i].count = 0;
        semaphores[i].max_count = 0;
        semaphores[i].is_active = false;
        semaphores[i].waiting_count = 0;
    }
    
    queue_manager_initialized = true;
    
    DEBUG_PRINT("Queue Manager initialized\n");
    return RTOS_SUCCESS;
}

/* ============================================================================
 * QUEUE FUNCTIONS
 * ============================================================================ */

/**
 * @brief Create a message queue
 */
queue_result_t queue_create(uint8_t queue_id, uint32_t size)
{
    if(!queue_manager_initialized || queue_id >= MAX_QUEUES || size == 0 || size > MAX_QUEUE_SIZE)
    {
        return QUEUE_ERROR;
    }
    
    queue_t* queue = &queues[queue_id];
    
    if(queue->is_active)
    {
        return QUEUE_ERROR; /* Queue already exists */
    }
    
    /* Allocate buffer */
    queue->buffer = (uint32_t*)memory_alloc(size * QUEUE_ITEM_SIZE);
    if(queue->buffer == NULL)
    {
        return QUEUE_ERROR;
    }
    
    /* Initialize queue */
    queue->size = size;
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
    queue->is_active = true;
    queue->send_waiting_count = 0;
    queue->receive_waiting_count = 0;
    
    DEBUG_PRINT("Queue %d created with size %ld\n", queue_id, size);
    
    return QUEUE_SUCCESS;
}

/**
 * @brief Delete a message queue
 */
queue_result_t queue_delete(uint8_t queue_id)
{
    if(!queue_manager_initialized || queue_id >= MAX_QUEUES)
    {
        return QUEUE_ERROR;
    }
    
    queue_t* queue = &queues[queue_id];
    
    if(!queue->is_active)
    {
        return QUEUE_ERROR;
    }
    
    /* Free buffer */
    if(queue->buffer != NULL)
    {
        memory_free(queue->buffer);
        queue->buffer = NULL;
    }
    
    /* Wake up all waiting tasks with error */
    while(queue->send_waiting_count > 0)
    {
        queue_wake_waiting_task(queue_id, true);
    }
    
    while(queue->receive_waiting_count > 0)
    {
        queue_wake_waiting_task(queue_id, false);
    }
    
    /* Mark as inactive */
    queue->is_active = false;
    
    DEBUG_PRINT("Queue %d deleted\n", queue_id);
    
    return QUEUE_SUCCESS;
}

/**
 * @brief Send data to queue
 */
queue_result_t queue_send(uint8_t queue_id, const void* data, uint32_t timeout_ms)
{
    if(!queue_manager_initialized || queue_id >= MAX_QUEUES || data == NULL)
    {
        return QUEUE_ERROR;
    }
    
    queue_t* queue = &queues[queue_id];
    
    if(!queue->is_active)
    {
        return QUEUE_ERROR;
    }
    
    ENTER_CRITICAL();
    
    /* Check if queue is full */
    if(queue->count >= queue->size)
    {
        EXIT_CRITICAL();
        
        if(timeout_ms == 0)
        {
            return QUEUE_FULL;
        }
        
        /* Add current task to waiting list */
        tcb_t* current_task = task_get_current();
        if(current_task != NULL)
        {
            queue_add_waiting_task(queue_id, current_task->task_id, true);
            task_set_state(current_task->task_id, TASK_STATE_BLOCKED);
            
            /* In real implementation, would set timeout and yield */
            /* For simulation, return timeout */
            if(timeout_ms != QUEUE_TIMEOUT_INFINITE)
            {
                return QUEUE_TIMEOUT;
            }
        }
        
        return QUEUE_FULL;
    }
    
    /* Add data to queue */
    queue->buffer[queue->tail] = *(uint32_t*)data;
    queue->tail = (queue->tail + 1) % queue->size;
    queue->count++;
    
    EXIT_CRITICAL();
    
    /* Wake up waiting receiver */
    if(queue->receive_waiting_count > 0)
    {
        queue_wake_waiting_task(queue_id, false);
    }
    
    return QUEUE_SUCCESS;
}

/**
 * @brief Receive data from queue
 */
queue_result_t queue_receive(uint8_t queue_id, void* data, uint32_t timeout_ms)
{
    if(!queue_manager_initialized || queue_id >= MAX_QUEUES || data == NULL)
    {
        return QUEUE_ERROR;
    }
    
    queue_t* queue = &queues[queue_id];
    
    if(!queue->is_active)
    {
        return QUEUE_ERROR;
    }
    
    ENTER_CRITICAL();
    
    /* Check if queue is empty */
    if(queue->count == 0)
    {
        EXIT_CRITICAL();
        
        if(timeout_ms == 0)
        {
            return QUEUE_EMPTY;
        }
        
        /* Add current task to waiting list */
        tcb_t* current_task = task_get_current();
        if(current_task != NULL)
        {
            queue_add_waiting_task(queue_id, current_task->task_id, false);
            task_set_state(current_task->task_id, TASK_STATE_BLOCKED);
            
            /* In real implementation, would set timeout and yield */
            /* For simulation, return timeout */
            if(timeout_ms != QUEUE_TIMEOUT_INFINITE)
            {
                return QUEUE_TIMEOUT;
            }
        }
        
        return QUEUE_EMPTY;
    }
    
    /* Get data from queue */
    *(uint32_t*)data = queue->buffer[queue->head];
    queue->head = (queue->head + 1) % queue->size;
    queue->count--;
    
    EXIT_CRITICAL();
    
    /* Wake up waiting sender */
    if(queue->send_waiting_count > 0)
    {
        queue_wake_waiting_task(queue_id, true);
    }
    
    return QUEUE_SUCCESS;
}

/**
 * @brief Peek at queue data without removing it
 */
queue_result_t queue_peek(uint8_t queue_id, void* data)
{
    if(!queue_manager_initialized || queue_id >= MAX_QUEUES || data == NULL)
    {
        return QUEUE_ERROR;
    }
    
    queue_t* queue = &queues[queue_id];
    
    if(!queue->is_active || queue->count == 0)
    {
        return QUEUE_EMPTY;
    }
    
    ENTER_CRITICAL();
    *(uint32_t*)data = queue->buffer[queue->head];
    EXIT_CRITICAL();
    
    return QUEUE_SUCCESS;
}

/**
 * @brief Get number of items in queue
 */
uint32_t queue_get_count(uint8_t queue_id)
{
    if(!queue_manager_initialized || queue_id >= MAX_QUEUES)
    {
        return 0xFFFFFFFF;
    }
    
    return queues[queue_id].count;
}

/**
 * @brief Get available space in queue
 */
uint32_t queue_get_space(uint8_t queue_id)
{
    if(!queue_manager_initialized || queue_id >= MAX_QUEUES)
    {
        return 0xFFFFFFFF;
    }
    
    queue_t* queue = &queues[queue_id];
    return queue->size - queue->count;
}

/**
 * @brief Check if queue is full
 */
bool queue_is_full(uint8_t queue_id)
{
    if(!queue_manager_initialized || queue_id >= MAX_QUEUES)
    {
        return true;
    }
    
    queue_t* queue = &queues[queue_id];
    return (queue->count >= queue->size);
}

/**
 * @brief Check if queue is empty
 */
bool queue_is_empty(uint8_t queue_id)
{
    if(!queue_manager_initialized || queue_id >= MAX_QUEUES)
    {
        return true;
    }
    
    return (queues[queue_id].count == 0);
}

/* ============================================================================
 * SEMAPHORE FUNCTIONS
 * ============================================================================ */

/**
 * @brief Create a semaphore
 */
rtos_result_t semaphore_create(uint8_t semaphore_id, uint8_t initial_count, uint8_t max_count)
{
    if(!queue_manager_initialized || semaphore_id >= MAX_SEMAPHORES || 
       initial_count > max_count || max_count > SEMAPHORE_MAX_COUNT)
    {
        return RTOS_INVALID_PARAM;
    }
    
    semaphore_t* sem = &semaphores[semaphore_id];
    
    if(sem->is_active)
    {
        return RTOS_ERROR; /* Semaphore already exists */
    }
    
    /* Initialize semaphore */
    sem->count = initial_count;
    sem->max_count = max_count;
    sem->is_active = true;
    sem->waiting_count = 0;
    
    DEBUG_PRINT("Semaphore %d created (initial: %d, max: %d)\n", 
               semaphore_id, initial_count, max_count);
    
    return RTOS_SUCCESS;
}

/**
 * @brief Delete a semaphore
 */
rtos_result_t semaphore_delete(uint8_t semaphore_id)
{
    if(!queue_manager_initialized || semaphore_id >= MAX_SEMAPHORES)
    {
        return RTOS_INVALID_PARAM;
    }
    
    semaphore_t* sem = &semaphores[semaphore_id];
    
    if(!sem->is_active)
    {
        return RTOS_ERROR;
    }
    
    /* Wake up all waiting tasks with error */
    while(sem->waiting_count > 0)
    {
        semaphore_wake_waiting_task(semaphore_id);
    }
    
    /* Mark as inactive */
    sem->is_active = false;
    
    DEBUG_PRINT("Semaphore %d deleted\n", semaphore_id);
    
    return RTOS_SUCCESS;
}

/**
 * @brief Take/acquire a semaphore
 */
rtos_result_t semaphore_take(uint8_t semaphore_id, uint32_t timeout_ms)
{
    if(!queue_manager_initialized || semaphore_id >= MAX_SEMAPHORES)
    {
        return RTOS_INVALID_PARAM;
    }
    
    semaphore_t* sem = &semaphores[semaphore_id];
    
    if(!sem->is_active)
    {
        return RTOS_ERROR;
    }
    
    ENTER_CRITICAL();
    
    if(sem->count > 0)
    {
        /* Semaphore available */
        sem->count--;
        EXIT_CRITICAL();
        return RTOS_SUCCESS;
    }
    
    EXIT_CRITICAL();
    
    /* Semaphore not available */
    if(timeout_ms == 0)
    {
        return RTOS_TIMEOUT;
    }
    
    /* Add current task to waiting list */
    tcb_t* current_task = task_get_current();
    if(current_task != NULL)
    {
        semaphore_add_waiting_task(semaphore_id, current_task->task_id);
        task_set_state(current_task->task_id, TASK_STATE_BLOCKED);
        
        /* In real implementation, would set timeout and yield */
        /* For simulation, return timeout */
        if(timeout_ms != QUEUE_TIMEOUT_INFINITE)
        {
            return RTOS_TIMEOUT;
        }
    }
    
    return RTOS_TIMEOUT;
}

/**
 * @brief Give/release a semaphore
 */
rtos_result_t semaphore_give(uint8_t semaphore_id)
{
    if(!queue_manager_initialized || semaphore_id >= MAX_SEMAPHORES)
    {
        return RTOS_INVALID_PARAM;
    }
    
    semaphore_t* sem = &semaphores[semaphore_id];
    
    if(!sem->is_active)
    {
        return RTOS_ERROR;
    }
    
    ENTER_CRITICAL();
    
    /* Wake up waiting task first */
    if(sem->waiting_count > 0)
    {
        EXIT_CRITICAL();
        semaphore_wake_waiting_task(semaphore_id);
        return RTOS_SUCCESS;
    }
    
    /* No waiting tasks, increment count if not at max */
    if(sem->count < sem->max_count)
    {
        sem->count++;
    }
    
    EXIT_CRITICAL();
    
    return RTOS_SUCCESS;
}

/**
 * @brief Get current semaphore count
 */
uint8_t semaphore_get_count(uint8_t semaphore_id)
{
    if(!queue_manager_initialized || semaphore_id >= MAX_SEMAPHORES)
    {
        return 0xFF;
    }
    
    return semaphores[semaphore_id].count;
}

/* ============================================================================
 * UTILITY FUNCTIONS
 * ============================================================================ */

/**
 * @brief Print queue information
 */
void queue_print_info(uint8_t queue_id)
{
    if(queue_id == 0xFF)
    {
        /* Print all queues */
        DEBUG_PRINT("=== Queue Information ===\n");
        for(int i = 0; i < MAX_QUEUES; i++)
        {
            if(queues[i].is_active)
            {
                DEBUG_PRINT("Queue %d: Size=%ld, Count=%ld, Senders=%d, Receivers=%d\n",
                           i, queues[i].size, queues[i].count,
                           queues[i].send_waiting_count, queues[i].receive_waiting_count);
            }
        }
    }
    else if(queue_id < MAX_QUEUES && queues[queue_id].is_active)
    {
        queue_t* queue = &queues[queue_id];
        DEBUG_PRINT("Queue %d: Size=%ld, Count=%ld, Head=%ld, Tail=%ld\n",
                   queue_id, queue->size, queue->count, queue->head, queue->tail);
    }
}

/**
 * @brief Print semaphore information
 */
void semaphore_print_info(uint8_t semaphore_id)
{
    if(semaphore_id == 0xFF)
    {
        /* Print all semaphores */
        DEBUG_PRINT("=== Semaphore Information ===\n");
        for(int i = 0; i < MAX_SEMAPHORES; i++)
        {
            if(semaphores[i].is_active)
            {
                DEBUG_PRINT("Semaphore %d: Count=%d, Max=%d, Waiting=%d\n",
                           i, semaphores[i].count, semaphores[i].max_count,
                           semaphores[i].waiting_count);
            }
        }
    }
    else if(semaphore_id < MAX_SEMAPHORES && semaphores[semaphore_id].is_active)
    {
        semaphore_t* sem = &semaphores[semaphore_id];
        DEBUG_PRINT("Semaphore %d: Count=%d, Max=%d, Waiting=%d\n",
                   semaphore_id, sem->count, sem->max_count, sem->waiting_count);
    }
}

/**
 * @brief Handle timeouts for waiting tasks
 */
void queue_manager_handle_timeouts(void)
{
    /* This would be called by timer to handle task timeouts */
    /* Implementation would check timeout values and wake up timed-out tasks */
    
    /* For simulation, we'll keep this simple */
}

/* ============================================================================
 * PRIVATE FUNCTIONS
 * ============================================================================ */

/**
 * @brief Add task to queue waiting list
 */
static void queue_add_waiting_task(uint8_t queue_id, uint8_t task_id, bool is_sender)
{
    queue_t* queue = &queues[queue_id];
    
    if(is_sender && queue->send_waiting_count < MAX_TASKS)
    {
        queue->send_waiting_tasks[queue->send_waiting_count++] = task_id;
    }
    else if(!is_sender && queue->receive_waiting_count < MAX_TASKS)
    {
        queue->receive_waiting_tasks[queue->receive_waiting_count++] = task_id;
    }
}

/**
 * @brief Remove task from queue waiting list
 */
static void queue_remove_waiting_task(uint8_t queue_id, uint8_t task_id, bool is_sender)
{
    queue_t* queue = &queues[queue_id];
    
    if(is_sender)
    {
        for(int i = 0; i < queue->send_waiting_count; i++)
        {
            if(queue->send_waiting_tasks[i] == task_id)
            {
                /* Shift remaining tasks */
                for(int j = i; j < queue->send_waiting_count - 1; j++)
                {
                    queue->send_waiting_tasks[j] = queue->send_waiting_tasks[j + 1];
                }
                queue->send_waiting_count--;
                break;
            }
        }
    }
    else
    {
        for(int i = 0; i < queue->receive_waiting_count; i++)
        {
            if(queue->receive_waiting_tasks[i] == task_id)
            {
                /* Shift remaining tasks */
                for(int j = i; j < queue->receive_waiting_count - 1; j++)
                {
                    queue->receive_waiting_tasks[j] = queue->receive_waiting_tasks[j + 1];
                }
                queue->receive_waiting_count--;
                break;
            }
        }
    }
}

/**
 * @brief Wake up waiting task from queue
 */
static void queue_wake_waiting_task(uint8_t queue_id, bool is_sender)
{
    queue_t* queue = &queues[queue_id];
    
    if(is_sender && queue->send_waiting_count > 0)
    {
        uint8_t task_id = queue->send_waiting_tasks[0];
        queue_remove_waiting_task(queue_id, task_id, true);
        task_set_state(task_id, TASK_STATE_READY);
    }
    else if(!is_sender && queue->receive_waiting_count > 0)
    {
        uint8_t task_id = queue->receive_waiting_tasks[0];
        queue_remove_waiting_task(queue_id, task_id, false);
        task_set_state(task_id, TASK_STATE_READY);
    }
}

/**
 * @brief Add task to semaphore waiting list
 */
static void semaphore_add_waiting_task(uint8_t semaphore_id, uint8_t task_id)
{
    semaphore_t* sem = &semaphores[semaphore_id];
    
    if(sem->waiting_count < MAX_TASKS)
    {
        sem->waiting_tasks[sem->waiting_count++] = task_id;
    }
}

/**
 * @brief Remove task from semaphore waiting list
 */
static void semaphore_remove_waiting_task(uint8_t semaphore_id, uint8_t task_id)
{
    semaphore_t* sem = &semaphores[semaphore_id];
    
    for(int i = 0; i < sem->waiting_count; i++)
    {
        if(sem->waiting_tasks[i] == task_id)
        {
            /* Shift remaining tasks */
            for(int j = i; j < sem->waiting_count - 1; j++)
            {
                sem->waiting_tasks[j] = sem->waiting_tasks[j + 1];
            }
            sem->waiting_count--;
            break;
        }
    }
}

/**
 * @brief Wake up waiting task from semaphore
 */
static void semaphore_wake_waiting_task(uint8_t semaphore_id)
{
    semaphore_t* sem = &semaphores[semaphore_id];
    
    if(sem->waiting_count > 0)
    {
        uint8_t task_id = sem->waiting_tasks[0];
        semaphore_remove_waiting_task(semaphore_id, task_id);
        task_set_state(task_id, TASK_STATE_READY);
    }
}