/**
 * @file main.c
 * @brief ARM RTOS Scheduler - Main Application Entry Point (SIMPLIFIED VERSION)
 * @author Team Member 6 - System Integration
 * @date 2024
 * 
 * This file contains the main application entry point and system initialization.
 * Compatible with Keil uVision and ARM Cortex-M simulators.
 * 
 * SIMPLIFIED FOR BEGINNERS:
 * This version uses cooperative scheduling without infinite loops in tasks.
 * Tasks execute one iteration at a time and return control to the scheduler.
 * This allows the program to run completely without getting stuck.
 * See SIMPLIFIED_APPROACH.md for detailed explanation.
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
    
    DEBUG_PRINT("[MAIN] Entering scheduler loop...\n");
    DEBUG_PRINT("[MAIN] This is a SIMPLIFIED scheduler - tasks run one iteration at a time\n");
    DEBUG_PRINT("[MAIN] Press stop to end the simulation\n\n");
    
    /* Main scheduler loop - simplified for beginners
     * In a real RTOS, the scheduler would use timer interrupts and context switching.
     * Here, we simply call each task in turn to demonstrate task execution.
     * This allows all tasks to run without getting stuck in infinite loops. */
    uint32_t iteration = 0;
    while(1)
    {
        iteration++;
        
        /* Update task delays (simulate timer tick) */
        if(iteration % 10 == 0)  /* Update delays every 10 iterations */
        {
            task_update_delays();
        }
        
        /* Run next ready task */
        scheduler_run_next_task();
        
        /* Print status every 50 iterations to avoid overwhelming output */
        if(iteration % 50 == 0)
        {
            DEBUG_PRINT("\n[MAIN] Scheduler iteration %d completed\n", iteration);
            scheduler_print_info();
            DEBUG_PRINT("\n");
        }
        
        /* Small delay to make output readable in simulator */
        for(volatile int i = 0; i < 10000; i++);
        
        /* Stop after a reasonable number of iterations for testing */
        if(iteration >= 1000)
        {
            DEBUG_PRINT("\n[MAIN] Completed 1000 iterations - stopping for demonstration\n");
            DEBUG_PRINT("[MAIN] In a real system, this would run indefinitely\n");
            break;
        }
    }
    
    DEBUG_PRINT("\n[MAIN] Program completed successfully!\n");
    
    return 0;
}

/**
 * @brief Example Task 1 - High Priority Producer Task
 * @note Simplified version: executes one iteration per call
 */
void task1_function(void)
{
    static uint32_t counter = 0;
    static bool first_run = true;
    
    if(first_run)
    {
        DEBUG_PRINT("[TASK1] High priority task started\n");
        first_run = false;
    }
    
    /* Simulate high priority work */
    counter++;
    DEBUG_PRINT("[TASK1] Running - Counter: %d\n", counter);
    
    /* Send data to queue */
    queue_send(QUEUE_1, &counter, 10);
    
    /* Note: In simplified version, task_delay() just marks task as blocked.
     * The scheduler will skip blocked tasks for a number of iterations. */
    if(counter % 5 == 0)  /* Simulate delay every 5 iterations */
    {
        task_delay(100);
    }
}

/**
 * @brief Example Task 2 - Medium Priority Consumer Task
 * @note Simplified version: executes one iteration per call
 */
void task2_function(void)
{
    static bool first_run = true;
    uint32_t received_data;
    
    if(first_run)
    {
        DEBUG_PRINT("[TASK2] Medium priority task started\n");
        first_run = false;
    }
    
    /* Wait for data from queue */
    if(queue_receive(QUEUE_1, &received_data, 50) == QUEUE_SUCCESS)
    {
        DEBUG_PRINT("[TASK2] Received data: %d\n", received_data);
        /* Process received data */
    }
    else
    {
        DEBUG_PRINT("[TASK2] No data in queue\n");
    }
}

/**
 * @brief Example Task 3 - Low Priority Background Task
 * @note Simplified version: executes one iteration per call
 */
void task3_function(void)
{
    static bool first_run = true;
    static uint32_t run_count = 0;
    
    if(first_run)
    {
        DEBUG_PRINT("[TASK3] Low priority background task started\n");
        first_run = false;
    }
    
    run_count++;
    DEBUG_PRINT("[TASK3] Background task running (iteration %d)...\n", run_count);
    
    /* Background processing */
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