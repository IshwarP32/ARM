/**
 * @file timer_manager.c
 * @brief Timer and Interrupt Management Implementation
 * @author Team Member 5 - Timer/Interrupt Management
 * @date 2024
 * 
 * This module implements system timer, software timers, and interrupt handling
 * for the RTOS system.
 */

#include "timer_manager.h"
#include "scheduler.h"

/* ============================================================================
 * GLOBAL VARIABLES
 * ============================================================================ */
static software_timer_t software_timers[MAX_SOFTWARE_TIMERS];   /* Software timer array */
static timer_stats_t stats;                                    /* Timer statistics */
static volatile uint32_t system_tick_counter = 0;              /* System tick counter */
static volatile bool timer_running = false;                    /* Timer running state */
static bool timer_initialized = false;                         /* Initialization state */

/* ============================================================================
 * PRIVATE FUNCTION PROTOTYPES
 * ============================================================================ */
static uint8_t timer_find_free_slot(void);
static void timer_process_software_timers(void);
static void timer_execute_callback(uint8_t timer_id);

/* ============================================================================
 * PUBLIC FUNCTIONS - SYSTEM TIMER
 * ============================================================================ */

/**
 * @brief Initialize the timer manager
 */
rtos_result_t timer_init(void)
{
    if(timer_initialized)
    {
        return RTOS_SUCCESS;
    }
    
    /* Initialize software timers */
    for(int i = 0; i < MAX_SOFTWARE_TIMERS; i++)
    {
        software_timers[i].timer_id = i;
        software_timers[i].type = TIMER_TYPE_ONE_SHOT;
        software_timers[i].state = TIMER_STATE_STOPPED;
        software_timers[i].period_ms = 0;
        software_timers[i].remaining_ms = 0;
        software_timers[i].callback = NULL;
        software_timers[i].user_data = NULL;
        software_timers[i].is_active = false;
    }
    
    /* Initialize statistics */
    memset(&stats, 0, sizeof(timer_stats_t));
    
    /* Initialize system tick counter */
    system_tick_counter = 0;
    timer_running = false;
    
    timer_initialized = true;
    
    DEBUG_PRINT("Timer Manager initialized\n");
    
    return RTOS_SUCCESS;
}

/**
 * @brief Start the system timer
 */
rtos_result_t timer_start(void)
{
    if(!timer_initialized)
    {
        return RTOS_ERROR;
    }
    
    /* In real implementation, this would configure hardware timer */
    /* For simulation, we'll just set the running flag */
    timer_running = true;
    
    DEBUG_PRINT("System timer started\n");
    
    return RTOS_SUCCESS;
}

/**
 * @brief Stop the system timer
 */
rtos_result_t timer_stop(void)
{
    if(!timer_initialized)
    {
        return RTOS_ERROR;
    }
    
    timer_running = false;
    
    DEBUG_PRINT("System timer stopped\n");
    
    return RTOS_SUCCESS;
}

/**
 * @brief Get current system tick count
 */
uint32_t timer_get_ticks(void)
{
    return system_tick_counter;
}

/**
 * @brief Get system uptime in milliseconds
 */
uint32_t timer_get_uptime_ms(void)
{
    return timer_ticks_to_ms(system_tick_counter);
}

/**
 * @brief Convert milliseconds to ticks
 */
uint32_t timer_ms_to_ticks(uint32_t ms)
{
    return (ms * TICK_RATE_HZ) / 1000;
}

/**
 * @brief Convert ticks to milliseconds
 */
uint32_t timer_ticks_to_ms(uint32_t ticks)
{
    return (ticks * 1000) / TICK_RATE_HZ;
}

/**
 * @brief System timer interrupt handler
 */
void timer_interrupt_handler(void)
{
    if(!timer_running)
    {
        return;
    }
    
    uint32_t interrupt_start_time = system_tick_counter;
    
    /* Increment system tick counter */
    system_tick_counter++;
    
    /* Update statistics */
    stats.timer_interrupts++;
    
    /* Process software timers */
    timer_process_software_timers();
    
    /* Call scheduler tick */
    if(scheduler_is_running())
    {
        scheduler_tick();
    }
    
    /* Update interrupt timing statistics */
    uint32_t interrupt_time = system_tick_counter - interrupt_start_time;
    stats.total_interrupt_time += interrupt_time;
    
    if(interrupt_time > stats.max_interrupt_time)
    {
        stats.max_interrupt_time = interrupt_time;
    }
}

/* ============================================================================
 * SOFTWARE TIMER FUNCTIONS
 * ============================================================================ */

/**
 * @brief Create a software timer
 */
uint8_t timer_create(timer_type_t type, 
                    uint32_t period_ms, 
                    timer_callback_t callback, 
                    void* user_data)
{
    if(!timer_initialized || callback == NULL || period_ms == 0)
    {
        return TIMER_INVALID_ID;
    }
    
    uint8_t timer_id = timer_find_free_slot();
    if(timer_id == TIMER_INVALID_ID)
    {
        return TIMER_INVALID_ID;
    }
    
    software_timer_t* timer = &software_timers[timer_id];
    
    /* Initialize timer */
    timer->type = type;
    timer->state = TIMER_STATE_STOPPED;
    timer->period_ms = period_ms;
    timer->remaining_ms = period_ms;
    timer->callback = callback;
    timer->user_data = user_data;
    timer->is_active = true;
    
    DEBUG_PRINT("Software timer %d created (%s, %ld ms)\n", 
               timer_id, 
               (type == TIMER_TYPE_ONE_SHOT) ? "One-shot" : "Periodic",
               period_ms);
    
    return timer_id;
}

/**
 * @brief Delete a software timer
 */
rtos_result_t timer_delete(uint8_t timer_id)
{
    if(!timer_initialized || timer_id >= MAX_SOFTWARE_TIMERS)
    {
        return RTOS_INVALID_PARAM;
    }
    
    software_timer_t* timer = &software_timers[timer_id];
    
    if(!timer->is_active)
    {
        return RTOS_ERROR;
    }
    
    /* Mark timer as inactive */
    timer->is_active = false;
    timer->state = TIMER_STATE_STOPPED;
    timer->callback = NULL;
    
    DEBUG_PRINT("Software timer %d deleted\n", timer_id);
    
    return RTOS_SUCCESS;
}

/**
 * @brief Start a software timer
 */
rtos_result_t timer_start_timer(uint8_t timer_id)
{
    if(!timer_initialized || timer_id >= MAX_SOFTWARE_TIMERS)
    {
        return RTOS_INVALID_PARAM;
    }
    
    software_timer_t* timer = &software_timers[timer_id];
    
    if(!timer->is_active)
    {
        return RTOS_ERROR;
    }
    
    ENTER_CRITICAL();
    
    timer->state = TIMER_STATE_RUNNING;
    timer->remaining_ms = timer->period_ms;
    
    EXIT_CRITICAL();
    
    DEBUG_PRINT("Software timer %d started\n", timer_id);
    
    return RTOS_SUCCESS;
}

/**
 * @brief Stop a software timer
 */
rtos_result_t timer_stop_timer(uint8_t timer_id)
{
    if(!timer_initialized || timer_id >= MAX_SOFTWARE_TIMERS)
    {
        return RTOS_INVALID_PARAM;
    }
    
    software_timer_t* timer = &software_timers[timer_id];
    
    if(!timer->is_active)
    {
        return RTOS_ERROR;
    }
    
    ENTER_CRITICAL();
    
    timer->state = TIMER_STATE_STOPPED;
    
    EXIT_CRITICAL();
    
    DEBUG_PRINT("Software timer %d stopped\n", timer_id);
    
    return RTOS_SUCCESS;
}

/**
 * @brief Reset a software timer
 */
rtos_result_t timer_reset_timer(uint8_t timer_id)
{
    if(!timer_initialized || timer_id >= MAX_SOFTWARE_TIMERS)
    {
        return RTOS_INVALID_PARAM;
    }
    
    software_timer_t* timer = &software_timers[timer_id];
    
    if(!timer->is_active)
    {
        return RTOS_ERROR;
    }
    
    ENTER_CRITICAL();
    
    timer->remaining_ms = timer->period_ms;
    timer->state = TIMER_STATE_RUNNING;
    
    EXIT_CRITICAL();
    
    DEBUG_PRINT("Software timer %d reset\n", timer_id);
    
    return RTOS_SUCCESS;
}

/**
 * @brief Change timer period
 */
rtos_result_t timer_change_period(uint8_t timer_id, uint32_t new_period_ms)
{
    if(!timer_initialized || timer_id >= MAX_SOFTWARE_TIMERS || new_period_ms == 0)
    {
        return RTOS_INVALID_PARAM;
    }
    
    software_timer_t* timer = &software_timers[timer_id];
    
    if(!timer->is_active)
    {
        return RTOS_ERROR;
    }
    
    ENTER_CRITICAL();
    
    timer->period_ms = new_period_ms;
    
    /* If timer is running, update remaining time */
    if(timer->state == TIMER_STATE_RUNNING)
    {
        timer->remaining_ms = new_period_ms;
    }
    
    EXIT_CRITICAL();
    
    DEBUG_PRINT("Software timer %d period changed to %ld ms\n", timer_id, new_period_ms);
    
    return RTOS_SUCCESS;
}

/**
 * @brief Get timer state
 */
timer_state_t timer_get_state(uint8_t timer_id)
{
    if(!timer_initialized || timer_id >= MAX_SOFTWARE_TIMERS)
    {
        return TIMER_STATE_STOPPED;
    }
    
    if(!software_timers[timer_id].is_active)
    {
        return TIMER_STATE_STOPPED;
    }
    
    return software_timers[timer_id].state;
}

/**
 * @brief Get remaining time for timer
 */
uint32_t timer_get_remaining_time(uint8_t timer_id)
{
    if(!timer_initialized || timer_id >= MAX_SOFTWARE_TIMERS)
    {
        return 0;
    }
    
    if(!software_timers[timer_id].is_active)
    {
        return 0;
    }
    
    return software_timers[timer_id].remaining_ms;
}

/* ============================================================================
 * DELAY FUNCTIONS
 * ============================================================================ */

/**
 * @brief Blocking delay in milliseconds
 */
void timer_delay_ms(uint32_t ms)
{
    uint32_t start_ticks = system_tick_counter;
    uint32_t target_ticks = timer_ms_to_ticks(ms);
    
    while((system_tick_counter - start_ticks) < target_ticks)
    {
        /* Busy wait - in real implementation might use WFI */
        UNUSED(start_ticks); /* Prevent compiler optimization */
    }
}

/**
 * @brief Blocking delay in microseconds
 */
void timer_delay_us(uint32_t us)
{
    /* Simple busy-wait implementation */
    /* In real implementation, would use high-resolution timer */
    volatile uint32_t count = us * (SYSTEM_CLOCK_HZ / 1000000) / 4;
    
    while(count > 0)
    {
        count--;
    }
}

/* ============================================================================
 * INTERRUPT MANAGEMENT
 * ============================================================================ */

/**
 * @brief Enable interrupts globally
 */
void timer_enable_interrupts(void)
{
    /* In real implementation, would enable ARM NVIC */
    __enable_irq();
}

/**
 * @brief Disable interrupts globally
 */
void timer_disable_interrupts(void)
{
    /* In real implementation, would disable ARM NVIC */
    __disable_irq();
}

/**
 * @brief Get interrupt enable state
 */
bool timer_interrupts_enabled(void)
{
    /* In real implementation, would check PRIMASK register */
    /* For simulation, assume enabled */
    return true;
}

/* ============================================================================
 * STATISTICS AND DEBUG
 * ============================================================================ */

/**
 * @brief Get timer statistics
 */
rtos_result_t timer_get_stats(timer_stats_t* stats_out)
{
    if(!timer_initialized || stats_out == NULL)
    {
        return RTOS_INVALID_PARAM;
    }
    
    ENTER_CRITICAL();
    
    stats.system_ticks = system_tick_counter;
    *stats_out = stats;
    
    EXIT_CRITICAL();
    
    return RTOS_SUCCESS;
}

/**
 * @brief Reset timer statistics
 */
void timer_reset_stats(void)
{
    if(!timer_initialized)
    {
        return;
    }
    
    ENTER_CRITICAL();
    
    stats.timer_interrupts = 0;
    stats.missed_ticks = 0;
    stats.max_interrupt_time = 0;
    stats.total_interrupt_time = 0;
    stats.software_timer_expirations = 0;
    /* Keep system_ticks running */
    
    EXIT_CRITICAL();
}

/**
 * @brief Print timer information
 */
void timer_print_info(void)
{
    if(!timer_initialized)
    {
        DEBUG_PRINT("Timer manager not initialized\n");
        return;
    }
    
    DEBUG_PRINT("=== Timer Information ===\n");
    DEBUG_PRINT("System Ticks: %ld\n", stats.system_ticks);
    DEBUG_PRINT("Uptime: %ld ms\n", timer_get_uptime_ms());
    DEBUG_PRINT("Timer Running: %s\n", timer_running ? "Yes" : "No");
    DEBUG_PRINT("Timer Interrupts: %ld\n", stats.timer_interrupts);
    DEBUG_PRINT("Missed Ticks: %ld\n", stats.missed_ticks);
    DEBUG_PRINT("Max Interrupt Time: %ld\n", stats.max_interrupt_time);
    DEBUG_PRINT("Software Timer Expirations: %ld\n", stats.software_timer_expirations);
}

/**
 * @brief Print software timers status
 */
void timer_print_software_timers(void)
{
    if(!timer_initialized)
    {
        return;
    }
    
    DEBUG_PRINT("=== Software Timers ===\n");
    
    for(int i = 0; i < MAX_SOFTWARE_TIMERS; i++)
    {
        software_timer_t* timer = &software_timers[i];
        
        if(timer->is_active)
        {
            const char* type_str = (timer->type == TIMER_TYPE_ONE_SHOT) ? "One-shot" : "Periodic";
            const char* state_str;
            
            switch(timer->state)
            {
                case TIMER_STATE_STOPPED: state_str = "Stopped"; break;
                case TIMER_STATE_RUNNING: state_str = "Running"; break;
                case TIMER_STATE_EXPIRED: state_str = "Expired"; break;
                default: state_str = "Unknown"; break;
            }
            
            DEBUG_PRINT("Timer %d: %s, %s, Period: %ld ms, Remaining: %ld ms\n",
                       i, type_str, state_str, timer->period_ms, timer->remaining_ms);
        }
    }
}

/* ============================================================================
 * PRIVATE FUNCTIONS
 * ============================================================================ */

/**
 * @brief Find free timer slot
 */
static uint8_t timer_find_free_slot(void)
{
    for(int i = 0; i < MAX_SOFTWARE_TIMERS; i++)
    {
        if(!software_timers[i].is_active)
        {
            return i;
        }
    }
    
    return TIMER_INVALID_ID;
}

/**
 * @brief Process software timers (called from interrupt)
 */
static void timer_process_software_timers(void)
{
    for(int i = 0; i < MAX_SOFTWARE_TIMERS; i++)
    {
        software_timer_t* timer = &software_timers[i];
        
        if(timer->is_active && timer->state == TIMER_STATE_RUNNING)
        {
            if(timer->remaining_ms > 0)
            {
                timer->remaining_ms--;
                
                if(timer->remaining_ms == 0)
                {
                    /* Timer expired */
                    stats.software_timer_expirations++;
                    
                    if(timer->type == TIMER_TYPE_PERIODIC)
                    {
                        /* Restart periodic timer */
                        timer->remaining_ms = timer->period_ms;
                    }
                    else
                    {
                        /* One-shot timer - stop it */
                        timer->state = TIMER_STATE_EXPIRED;
                    }
                    
                    /* Execute callback outside critical section */
                    timer_execute_callback(i);
                }
            }
        }
    }
}

/**
 * @brief Execute timer callback
 */
static void timer_execute_callback(uint8_t timer_id)
{
    software_timer_t* timer = &software_timers[timer_id];
    
    if(timer->callback != NULL)
    {
        /* Call callback function */
        timer->callback(timer_id, timer->user_data);
    }
}