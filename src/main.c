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

/* Global test variables visible in watch window */
volatile int test_counter = 0;
volatile int main_reached = 0;
volatile int system_init_done = 0;

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
    main_reached = 1;  /* Set flag that main was reached */
    
    /* Simple test - this should always print */
    printf("HELLO WORLD - BASIC TEST\n");
    
    /* Test loop to see if we reach here */
    for(int i = 0; i < 5; i++)
    {
        printf("Test %d - Program is running\n", i);
        test_counter = i;  /* Update counter for watch window */
    }
    
    DEBUG_PRINT("=== ARM RTOS Scheduler Starting ===\n");
    
    /* Initialize system components */
    DEBUG_PRINT("[INIT] Initializing system...\n");
    system_init();
    system_init_done = 1;  /* Set flag that init is done */
    
    /* Initialize RTOS components */
    DEBUG_PRINT("[INIT] Initializing memory manager...\n");
    memory_init();
    DEBUG_PRINT("[INIT] Initializing task manager...\n");
    task_manager_init();
    DEBUG_PRINT("[INIT] Initializing queue manager...\n");
    queue_manager_init();
    DEBUG_PRINT("[INIT] Initializing timer...\n");
    timer_init();
    DEBUG_PRINT("[INIT] Initializing scheduler...\n");
    scheduler_init();
    
    /* Create example tasks */
    DEBUG_PRINT("[TASK] Creating tasks...\n");
    task_create(task1_function, "Task1", PRIORITY_HIGH, 256);
    task_create(task2_function, "Task2", PRIORITY_MEDIUM, 256);
    task_create(task3_function, "Task3", PRIORITY_LOW, 256);
    
    DEBUG_PRINT("[SCHED] Starting RTOS scheduler...\n");
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
    DEBUG_PRINT("[TASK1] High priority task started\n");
    
    while(1)
    {
        /* Simulate high priority work */
        counter++;
        DEBUG_PRINT("[TASK1] Running - Counter: %d\n", counter);
        
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
    DEBUG_PRINT("[TASK2] Medium priority task started\n");
    
    while(1)
    {
        /* Wait for data from queue */
        if(queue_receive(QUEUE_1, &received_data, 50) == QUEUE_SUCCESS)
        {
            DEBUG_PRINT("[TASK2] Received data: %d\n", received_data);
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
    DEBUG_PRINT("[TASK3] Low priority background task started\n");
    
    while(1)
    {
        DEBUG_PRINT("[TASK3] Background task running...\n");
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
    DEBUG_PRINT("[SYS] Starting system initialization...\n");
    
    /* Initialize ARM Cortex-M specific features */
    cortex_m_init();
    DEBUG_PRINT("[SYS] ARM Cortex-M initialized\n");
    
    /* Configure system clock (simulation - actual implementation would configure PLL, etc.) */
    /* For Keil simulator, clock is configured automatically */
    
    /* Configure SysTick timer for RTOS tick */
    cortex_m_systick_config(SYSTEM_CLOCK_HZ / TICK_RATE_HZ);
    DEBUG_PRINT("[SYS] SysTick configured\n");
    
    /* Set interrupt priorities */
    cortex_m_set_interrupt_priorities();
    DEBUG_PRINT("[SYS] Interrupt priorities set\n");
    
    /* Configure GPIO pins (simulation) */
    /* In real implementation, would configure GPIO registers */
    
    /* Configure peripherals (simulation) */
    /* In real implementation, would configure UART, SPI, etc. */
    
    DEBUG_PRINT("[SYS] System initialized for ARM Cortex-M3 simulation\n");
}