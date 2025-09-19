/**
 * @file main.c
 * @brief ARM RTOS Scheduler - Main Application Entry Point
 * @author Team Member 6 - System Integration
 * @date 2024
 * 
 * This file contains the main application entry point and system initialization.
 * Compatible with Keil uVision and ARM Cortex-M simulators.
 */

#include "rtos_config.h"
#include "task_manager.h"
#include "scheduler.h"
#include "queue_manager.h"
#include "memory_manager.h"
#include "timer_manager.h"
#include "arm_cortex_m.h"

/* Example task functions */
void task1_function(void);
void task2_function(void);
void task3_function(void);

/**
 * @brief Main application entry point
 * @return int Return status (never reached in embedded systems)
 */
int main(void)
{
    /* Initialize system components */
    system_init();
    
    /* Initialize RTOS components */
    memory_init();
    task_manager_init();
    queue_manager_init();
    timer_init();
    scheduler_init();
    
    /* Create example tasks */
    task_create(task1_function, "Task1", PRIORITY_HIGH, 256);
    task_create(task2_function, "Task2", PRIORITY_MEDIUM, 256);
    task_create(task3_function, "Task3", PRIORITY_LOW, 256);
    
    /* Start the RTOS scheduler */
    scheduler_start();
    
    /* Should never reach here */
    while(1)
    {
        /* Emergency fallback */
    }
    
    return 0;
}

/**
 * @brief Example Task 1 - High Priority Producer Task
 */
void task1_function(void)
{
    uint32_t counter = 0;
    
    while(1)
    {
        /* Simulate high priority work */
        counter++;
        
        /* Send data to queue */
        queue_send(QUEUE_1, &counter, 10);
        
        /* Task delay */
        task_delay(100);
    }
}

/**
 * @brief Example Task 2 - Medium Priority Consumer Task
 */
void task2_function(void)
{
    uint32_t received_data;
    
    while(1)
    {
        /* Wait for data from queue */
        if(queue_receive(QUEUE_1, &received_data, 50) == QUEUE_SUCCESS)
        {
            /* Process received data */
            // Add your processing logic here
        }
        
        /* Task delay */
        task_delay(200);
    }
}

/**
 * @brief Example Task 3 - Low Priority Background Task
 */
void task3_function(void)
{
    while(1)
    {
        /* Background processing */
        // Add background tasks here
        
        /* Long delay for background task */
        task_delay(1000);
    }
}

/**
 * @brief System initialization function
 */
void system_init(void)
{
    /* Initialize ARM Cortex-M specific features */
    cortex_m_init();
    
    /* Configure system clock (simulation - actual implementation would configure PLL, etc.) */
    /* For Keil simulator, clock is configured automatically */
    
    /* Configure SysTick timer for RTOS tick */
    cortex_m_systick_config(SYSTEM_CLOCK_HZ / TICK_RATE_HZ);
    
    /* Set interrupt priorities */
    cortex_m_set_interrupt_priorities();
    
    /* Configure GPIO pins (simulation) */
    /* In real implementation, would configure GPIO registers */
    
    /* Configure peripherals (simulation) */
    /* In real implementation, would configure UART, SPI, etc. */
    
    DEBUG_PRINT("System initialized for ARM Cortex-M3 simulation\n");
}