/**
 * @file rtos_config.h
 * @brief ARM RTOS Configuration Header
 * @author All Team Members
 * @date 2024
 * 
 * This file contains all configuration parameters and common includes
 * for the ARM RTOS scheduler project.
 */

#ifndef RTOS_CONFIG_H
#define RTOS_CONFIG_H

/* Standard includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/* ARM Cortex-M specific includes (for Keil uVision) */
#ifdef __ARMCC_VERSION
    /* ARM Compiler specific */
    #include <arm_compat.h>
#endif

/* ============================================================================
 * RTOS CONFIGURATION PARAMETERS
 * ============================================================================ */

/* Maximum number of tasks */
#define MAX_TASKS                   8

/* Maximum task name length */
#define MAX_TASK_NAME_LENGTH        16

/* Task stack sizes */
#define MIN_STACK_SIZE              128
#define DEFAULT_STACK_SIZE          256
#define MAX_STACK_SIZE              1024

/* Time slice for round-robin scheduling (in ms) */
#define TIME_SLICE_MS               10

/* Maximum number of queues */
#define MAX_QUEUES                  4

/* Maximum queue size */
#define MAX_QUEUE_SIZE              16

/* System clock frequency (Hz) - for simulation */
#define SYSTEM_CLOCK_HZ             48000000

/* Timer tick frequency (Hz) */
#define TICK_RATE_HZ                1000

/* ============================================================================
 * TASK PRIORITIES
 * ============================================================================ */
#define PRIORITY_IDLE               0
#define PRIORITY_LOW                1
#define PRIORITY_MEDIUM             2
#define PRIORITY_HIGH               3
#define PRIORITY_CRITICAL           4

/* ============================================================================
 * TASK STATES
 * ============================================================================ */
typedef enum {
    TASK_STATE_READY,
    TASK_STATE_RUNNING,
    TASK_STATE_BLOCKED,
    TASK_STATE_SUSPENDED,
    TASK_STATE_DELETED
} task_state_t;

/* ============================================================================
 * RETURN CODES
 * ============================================================================ */
typedef enum {
    RTOS_SUCCESS = 0,
    RTOS_ERROR = -1,
    RTOS_TIMEOUT = -2,
    RTOS_NO_MEMORY = -3,
    RTOS_INVALID_PARAM = -4
} rtos_result_t;

/* ============================================================================
 * QUEUE DEFINITIONS
 * ============================================================================ */
typedef enum {
    QUEUE_SUCCESS = 0,
    QUEUE_ERROR = -1,
    QUEUE_TIMEOUT = -2,
    QUEUE_FULL = -3,
    QUEUE_EMPTY = -4
} queue_result_t;

/* Queue IDs */
#define QUEUE_1                     0
#define QUEUE_2                     1
#define QUEUE_3                     2
#define QUEUE_4                     3

/* ============================================================================
 * MEMORY MANAGEMENT
 * ============================================================================ */
#define HEAP_SIZE                   4096
#define MEMORY_BLOCK_SIZE           32

/* ============================================================================
 * DEBUG CONFIGURATION
 * ============================================================================ */
#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...)   printf(fmt, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...)   
#endif

/* ============================================================================
 * COMMON MACROS
 * ============================================================================ */
#define UNUSED(x)                   ((void)(x))
#define ARRAY_SIZE(arr)             (sizeof(arr) / sizeof((arr)[0]))

/* Critical section macros (for ARM Cortex-M) */
#define ENTER_CRITICAL()            __disable_irq()
#define EXIT_CRITICAL()             __enable_irq()

/* ============================================================================
 * FUNCTION PROTOTYPES
 * ============================================================================ */
void system_init(void);

#endif /* RTOS_CONFIG_H */