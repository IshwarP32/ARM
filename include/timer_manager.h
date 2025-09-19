/**
 * @file timer_manager.h
 * @brief Timer and Interrupt Management Interface
 * @author Team Member 5 - Timer/Interrupt Management
 * @date 2024
 * 
 * This module handles system timer, interrupts, and provides timing services
 * for the RTOS scheduler and tasks.
 */

#ifndef TIMER_MANAGER_H
#define TIMER_MANAGER_H

#include "rtos_config.h"

/* ============================================================================
 * TIMER CONFIGURATION
 * ============================================================================ */
#define MAX_SOFTWARE_TIMERS         8       /* Maximum number of software timers */
#define TIMER_INVALID_ID            0xFF    /* Invalid timer ID */

/* ============================================================================
 * TIMER TYPES
 * ============================================================================ */
typedef enum {
    TIMER_TYPE_ONE_SHOT,                    /* Timer runs once */
    TIMER_TYPE_PERIODIC                     /* Timer repeats */
} timer_type_t;

typedef enum {
    TIMER_STATE_STOPPED,                    /* Timer is stopped */
    TIMER_STATE_RUNNING,                    /* Timer is running */
    TIMER_STATE_EXPIRED                     /* Timer has expired */
} timer_state_t;

/* ============================================================================
 * TIMER CALLBACK FUNCTION TYPE
 * ============================================================================ */
typedef void (*timer_callback_t)(uint8_t timer_id, void* user_data);

/* ============================================================================
 * SOFTWARE TIMER STRUCTURE
 * ============================================================================ */
typedef struct {
    uint8_t timer_id;                       /* Timer identifier */
    timer_type_t type;                      /* Timer type */
    timer_state_t state;                    /* Timer state */
    uint32_t period_ms;                     /* Timer period in milliseconds */
    uint32_t remaining_ms;                  /* Remaining time in milliseconds */
    timer_callback_t callback;              /* Callback function */
    void* user_data;                        /* User data for callback */
    bool is_active;                         /* Timer slot active */
} software_timer_t;

/* ============================================================================
 * TIMER STATISTICS
 * ============================================================================ */
typedef struct {
    uint32_t system_ticks;                  /* Total system ticks */
    uint32_t timer_interrupts;              /* Number of timer interrupts */
    uint32_t missed_ticks;                  /* Number of missed ticks */
    uint32_t max_interrupt_time;            /* Maximum interrupt processing time */
    uint32_t total_interrupt_time;          /* Total interrupt processing time */
    uint32_t software_timer_expirations;   /* Software timer expirations */
} timer_stats_t;

/* ============================================================================
 * FUNCTION PROTOTYPES - SYSTEM TIMER
 * ============================================================================ */

/**
 * @brief Initialize the timer manager
 * @return rtos_result_t Success or error code
 */
rtos_result_t timer_init(void);

/**
 * @brief Start the system timer
 * @return rtos_result_t Success or error code
 */
rtos_result_t timer_start(void);

/**
 * @brief Stop the system timer
 * @return rtos_result_t Success or error code
 */
rtos_result_t timer_stop(void);

/**
 * @brief Get current system tick count
 * @return uint32_t Current tick count
 */
uint32_t timer_get_ticks(void);

/**
 * @brief Get system uptime in milliseconds
 * @return uint32_t Uptime in milliseconds
 */
uint32_t timer_get_uptime_ms(void);

/**
 * @brief Convert milliseconds to ticks
 * @param ms Milliseconds
 * @return uint32_t Equivalent ticks
 */
uint32_t timer_ms_to_ticks(uint32_t ms);

/**
 * @brief Convert ticks to milliseconds
 * @param ticks Number of ticks
 * @return uint32_t Equivalent milliseconds
 */
uint32_t timer_ticks_to_ms(uint32_t ticks);

/**
 * @brief System timer interrupt handler
 * @note Called by hardware timer interrupt
 */
void timer_interrupt_handler(void);

/* ============================================================================
 * FUNCTION PROTOTYPES - SOFTWARE TIMERS
 * ============================================================================ */

/**
 * @brief Create a software timer
 * @param type Timer type (one-shot or periodic)
 * @param period_ms Timer period in milliseconds
 * @param callback Callback function to call when timer expires
 * @param user_data User data to pass to callback
 * @return uint8_t Timer ID (TIMER_INVALID_ID if failed)
 */
uint8_t timer_create(timer_type_t type, 
                    uint32_t period_ms, 
                    timer_callback_t callback, 
                    void* user_data);

/**
 * @brief Delete a software timer
 * @param timer_id Timer ID
 * @return rtos_result_t Success or error code
 */
rtos_result_t timer_delete(uint8_t timer_id);

/**
 * @brief Start a software timer
 * @param timer_id Timer ID
 * @return rtos_result_t Success or error code
 */
rtos_result_t timer_start_timer(uint8_t timer_id);

/**
 * @brief Stop a software timer
 * @param timer_id Timer ID
 * @return rtos_result_t Success or error code
 */
rtos_result_t timer_stop_timer(uint8_t timer_id);

/**
 * @brief Reset a software timer
 * @param timer_id Timer ID
 * @return rtos_result_t Success or error code
 */
rtos_result_t timer_reset_timer(uint8_t timer_id);

/**
 * @brief Change timer period
 * @param timer_id Timer ID
 * @param new_period_ms New period in milliseconds
 * @return rtos_result_t Success or error code
 */
rtos_result_t timer_change_period(uint8_t timer_id, uint32_t new_period_ms);

/**
 * @brief Get timer state
 * @param timer_id Timer ID
 * @return timer_state_t Current timer state
 */
timer_state_t timer_get_state(uint8_t timer_id);

/**
 * @brief Get remaining time for timer
 * @param timer_id Timer ID
 * @return uint32_t Remaining time in milliseconds
 */
uint32_t timer_get_remaining_time(uint8_t timer_id);

/* ============================================================================
 * FUNCTION PROTOTYPES - DELAY FUNCTIONS
 * ============================================================================ */

/**
 * @brief Blocking delay in milliseconds
 * @param ms Delay time in milliseconds
 * @note This is a busy-wait delay, use task_delay() for task yielding
 */
void timer_delay_ms(uint32_t ms);

/**
 * @brief Blocking delay in microseconds
 * @param us Delay time in microseconds
 * @note This is a busy-wait delay
 */
void timer_delay_us(uint32_t us);

/* ============================================================================
 * FUNCTION PROTOTYPES - INTERRUPT MANAGEMENT
 * ============================================================================ */

/**
 * @brief Enable interrupts globally
 */
void timer_enable_interrupts(void);

/**
 * @brief Disable interrupts globally
 */
void timer_disable_interrupts(void);

/**
 * @brief Get interrupt enable state
 * @return bool True if interrupts are enabled
 */
bool timer_interrupts_enabled(void);

/* ============================================================================
 * FUNCTION PROTOTYPES - STATISTICS AND DEBUG
 * ============================================================================ */

/**
 * @brief Get timer statistics
 * @param stats Pointer to statistics structure
 * @return rtos_result_t Success or error code
 */
rtos_result_t timer_get_stats(timer_stats_t* stats);

/**
 * @brief Reset timer statistics
 */
void timer_reset_stats(void);

/**
 * @brief Print timer information (for debugging)
 */
void timer_print_info(void);

/**
 * @brief Print software timers status (for debugging)
 */
void timer_print_software_timers(void);

#endif /* TIMER_MANAGER_H */