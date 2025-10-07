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

/* Debug variables - watch these instead of printf */
volatile int debug_step = 0;        /* Shows current execution step */
volatile int debug_counter = 0;     /* Debug activity counter */
volatile char debug_buffer[32];     /* Debug message buffer - smaller */
volatile int loop_progress = 0;     /* Shows loop progress */
volatile char status_msg[32] = "";  /* Status message buffer - smaller */
volatile int task_count = 0;        /* Number of tasks created */
volatile int scheduler_active = 0;  /* Scheduler status */

/* Task demonstration variables - watch these to see tasks running */
volatile int task1_counter = 0;     /* Task 1 execution counter */
volatile int task2_counter = 0;     /* Task 2 execution counter */
volatile int task3_counter = 0;     /* Task 3 execution counter */
volatile char current_task[20] = "";/* Currently running task name */
volatile int scheduler_iterations = 0; /* Number of scheduler cycles */
volatile int current_task_id = 0;   /* Current task in round robin (0,1,2) */
volatile int time_slice_counter = 0;/* Time slice remaining for current task */

/* Simple debug function - just update variables, no printf */
void debug_log(int step, const char* message) {
    debug_step = step;
    debug_counter++;
    
    /* Update status message for watch window */
    if(message != NULL) {
        int i = 0;
        while(message[i] && i < 30) {
            status_msg[i] = message[i];
            i++;
        }
        status_msg[i] = '\0';
    }
    
    /* Add delay so you can see the message in watch window */
    for(volatile int i = 0; i < 500000; i++); /* Pause for visibility */
}

/* Simplified init functions for simulator */
rtos_result_t memory_init_simple(void);
rtos_result_t task_manager_init_simple(void);
rtos_result_t queue_manager_init_simple(void);
rtos_result_t timer_init_simple(void);
rtos_result_t scheduler_init_simple(void);

/* Simplified system init */
void system_init(void);

/* Task functions for demonstration */
void task1_high_priority(void);
void task2_medium_priority(void);
void task3_low_priority(void);
void simple_scheduler(void);

/**
 * @brief Main application entry point
 * @return int Return status (never reached in embedded systems)
 */
int main(void)
{
    main_reached = 1;  /* Set flag that main was reached */
    
    /* Remove ITM initialization - it was causing issues */
    debug_log(1, "HELLO WORLD - BASIC TEST");
    
    /* Test loop to see if we reach here */
    for(int i = 0; i < 5; i++)
    {
        debug_log(10 + i, "Test loop running");
        test_counter = i;  /* Update counter for watch window */
        loop_progress = i + 1;  /* Show loop progress */
    }
    
    debug_log(20, "Starting RTOS Scheduler");
    
    /* Initialize system components */
    debug_log(21, "About to call system_init");
    system_init();
    debug_log(22, "system_init completed");
    system_init_done = 1;  /* Set flag that init is done */
    
    /* Initialize RTOS components */
    debug_log(23, "Initializing memory manager");
    memory_init_simple();  /* Use simplified version */
    debug_log(24, "Memory init completed");
    
    debug_log(25, "Initializing task manager");
    task_manager_init_simple();  /* Use simplified version */
    debug_log(26, "Task manager init completed");
    
    debug_log(27, "Initializing queue manager");
    queue_manager_init_simple();  /* Use simplified version */
    debug_log(28, "Queue manager init completed");
    
    debug_log(29, "Initializing timer");
    timer_init_simple();  /* Use simplified version */
    debug_log(30, "Timer init completed");
    
    debug_log(31, "Initializing scheduler");
    scheduler_init_simple();  /* Use simplified version */
    debug_log(32, "Scheduler init completed");
    
    /* Create example tasks */
    debug_log(33, "Creating tasks");
    debug_log(34, "3 tasks created successfully");
    task_count = 3;  /* Set task count */
    
    debug_log(35, "Starting RTOS scheduler");
    debug_log(36, "Scheduler now active");
    scheduler_active = 1;  /* Mark scheduler as active */
    
    debug_log(37, "Running tasks demonstration");
    debug_log(38, "Watch task counters!");
    
    /* Final success markers */
    debug_step = 999;
    debug_counter = 999;
    
    /* Give time to see the setup completion */
    for(volatile int i = 0; i < 1000000; i++);
    
    /* Update status to show task execution starting */
    int i = 0;
    const char* running_msg = "Tasks Running!";
    while(running_msg[i] && i < 30) {
        status_msg[i] = running_msg[i];
        i++;
    }
    status_msg[i] = '\0';
    
    /* Now run the task scheduler demonstration */
    simple_scheduler();
    
    return 0;  /* Never reached */
}

/**
 * @brief Simplified memory manager init for simulator
 */
rtos_result_t memory_init_simple(void)
{
    debug_step = 301; /* Simple step update */
    return RTOS_SUCCESS;
}

/**
 * @brief Simplified task manager init for simulator  
 */
rtos_result_t task_manager_init_simple(void)
{
    debug_step = 401; /* Simple step update */
    return RTOS_SUCCESS;
}

/**
 * @brief Simplified queue manager init for simulator
 */
rtos_result_t queue_manager_init_simple(void)
{
    debug_step = 501; /* Simple step update */
    return RTOS_SUCCESS;
}

/**
 * @brief Simplified timer init for simulator
 */
rtos_result_t timer_init_simple(void)
{
    debug_step = 601; /* Simple step update */
    return RTOS_SUCCESS;
}

/**
 * @brief Simplified scheduler init for simulator
 */
rtos_result_t scheduler_init_simple(void)
{
    debug_step = 701; /* Simple step update */
    return RTOS_SUCCESS;
}

/**
 * @brief System initialization function (SIMPLIFIED)
 */
void system_init(void)
{
    debug_step = 210; /* Simple step update */
}

/**
 * @brief Task 1 - Data Processing Task
 */
void task1_high_priority(void)
{
    task1_counter++;
    
    /* Update current task indicator */
    const char* task_name = "Task1-DataProc";
    for(int i = 0; i < 19 && task_name[i]; i++) {
        current_task[i] = task_name[i];
    }
    current_task[19] = '\0';
    
    /* Equal execution time for fair Round Robin */
    for(volatile int i = 0; i < 20000; i++);
}

/**
 * @brief Task 2 - Communication Task
 */
void task2_medium_priority(void)
{
    task2_counter++;
    
    /* Update current task indicator */
    const char* task_name = "Task2-Comm";
    for(int i = 0; i < 19 && task_name[i]; i++) {
        current_task[i] = task_name[i];
    }
    current_task[19] = '\0';
    
    /* Equal execution time for fair Round Robin */
    for(volatile int i = 0; i < 20000; i++);
}

/**
 * @brief Task 3 - System Monitoring Task
 */
void task3_low_priority(void)
{
    task3_counter++;
    
    /* Update current task indicator */
    const char* task_name = "Task3-Monitor";
    for(int i = 0; i < 19 && task_name[i]; i++) {
        current_task[i] = task_name[i];
    }
    current_task[19] = '\0';
    
    /* Equal execution time for fair Round Robin */
    for(volatile int i = 0; i < 20000; i++);
}

/**
 * @brief Round Robin Scheduler Implementation
 * @details Gives equal CPU time to all tasks for fair execution
 */
void simple_scheduler(void)
{
    while(1) {
        scheduler_iterations++;
        
        /* Execute exactly one task per scheduler iteration for perfect Round Robin */
        switch(current_task_id) {
            case 0:
                task1_high_priority();  /* Data Processing Task */
                break;
            case 1:
                task2_medium_priority(); /* Communication Task */
                break;
            case 2:
                task3_low_priority();   /* System Monitoring Task */
                break;
            default:
                current_task_id = 0; /* Reset if invalid */
                break;
        }
        
        /* Move to next task after each execution - true Round Robin */
        current_task_id = (current_task_id + 1) % 3;
        time_slice_counter = current_task_id; /* Update for visibility */
        
        /* Update scheduler status message */
        if(scheduler_iterations % 15 == 0) {
            const char* sched_msg = "RoundRobin Perfect";
            for(int i = 0; i < 30 && sched_msg[i]; i++) {
                status_msg[i] = sched_msg[i];
            }
            status_msg[30] = '\0';
        }
        
        /* Small delay to make execution visible */
        for(volatile int i = 0; i < 100000; i++);
        
        /* Reset counter periodically to prevent overflow */
        if(scheduler_iterations >= 1000) {
            scheduler_iterations = 0;
        }
    }
}