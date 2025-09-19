/**
 * @file led_blink_example.c
 * @brief LED Blinking Example using RTOS
 * @author Example Implementation
 * @date 2024
 * 
 * This example demonstrates basic RTOS functionality with LED blinking tasks.
 */

#include "rtos_config.h"
#include "task_manager.h"
#include "scheduler.h"
#include "timer_manager.h"

/* Example GPIO definitions (simulated) */
#define LED1_PIN        (1 << 0)
#define LED2_PIN        (1 << 1)
#define LED3_PIN        (1 << 2)

/* Global variables for LED states */
static volatile uint32_t led_states = 0;

/* Task function prototypes */
void led1_blink_task(void);
void led2_blink_task(void);
void led3_blink_task(void);
void status_monitor_task(void);

/* Timer callback prototypes */
void heartbeat_timer_callback(uint8_t timer_id, void* user_data);

/**
 * @brief Initialize LED example
 */
void led_example_init(void)
{
    /* In real hardware, this would configure GPIO pins */
    led_states = 0;
    
    /* Create LED blinking tasks */
    task_create(led1_blink_task, "LED1_Blink", PRIORITY_LOW, 256);
    task_create(led2_blink_task, "LED2_Blink", PRIORITY_LOW, 256);
    task_create(led3_blink_task, "LED3_Blink", PRIORITY_MEDIUM, 256);
    task_create(status_monitor_task, "Monitor", PRIORITY_HIGH, 512);
    
    /* Create heartbeat timer */
    timer_create(TIMER_TYPE_PERIODIC, 5000, heartbeat_timer_callback, NULL);
    
    DEBUG_PRINT("LED Example initialized\n");
}

/**
 * @brief LED1 blinking task - Slow blink (1Hz)
 */
void led1_blink_task(void)
{
    while(1)
    {
        /* Toggle LED1 */
        led_states ^= LED1_PIN;
        
        DEBUG_PRINT("LED1: %s\n", (led_states & LED1_PIN) ? "ON" : "OFF");
        
        /* Delay 500ms */
        task_delay(500);
    }
}

/**
 * @brief LED2 blinking task - Medium blink (2Hz)
 */
void led2_blink_task(void)
{
    while(1)
    {
        /* Toggle LED2 */
        led_states ^= LED2_PIN;
        
        DEBUG_PRINT("LED2: %s\n", (led_states & LED2_PIN) ? "ON" : "OFF");
        
        /* Delay 250ms */
        task_delay(250);
    }
}

/**
 * @brief LED3 blinking task - Fast blink (5Hz)
 */
void led3_blink_task(void)
{
    while(1)
    {
        /* Toggle LED3 */
        led_states ^= LED3_PIN;
        
        DEBUG_PRINT("LED3: %s\n", (led_states & LED3_PIN) ? "ON" : "OFF");
        
        /* Delay 100ms */
        task_delay(100);
    }
}

/**
 * @brief Status monitoring task
 */
void status_monitor_task(void)
{
    uint32_t counter = 0;
    
    while(1)
    {
        counter++;
        
        /* Print system status every 10 iterations */
        if(counter % 10 == 0)
        {
            DEBUG_PRINT("=== System Status (Count: %ld) ===\n", counter);
            DEBUG_PRINT("LED States: 0x%02lX\n", led_states);
            DEBUG_PRINT("Uptime: %ld ms\n", timer_get_uptime_ms());
            DEBUG_PRINT("Free Memory: %ld bytes\n", memory_get_free_size());
            DEBUG_PRINT("Active Tasks: %d\n", task_get_count());
            
            /* Print task information */
            task_print_info(0xFF);
            
            /* Print memory information */
            if(counter % 50 == 0)
            {
                memory_print_info();
            }
        }
        
        /* Delay 1 second */
        task_delay(1000);
    }
}

/**
 * @brief Heartbeat timer callback
 */
void heartbeat_timer_callback(uint8_t timer_id, void* user_data)
{
    UNUSED(timer_id);
    UNUSED(user_data);
    
    DEBUG_PRINT("*** HEARTBEAT *** Uptime: %ld ms\n", timer_get_uptime_ms());
}