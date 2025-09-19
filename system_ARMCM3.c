/**
 * @file system_ARMCM3.c
 * @brief System initialization for ARM Cortex-M3
 * @author ARM Limited (modified for ARM RTOS project)
 * @date 2024
 * 
 * This file provides the SystemInit function required by the startup code.
 */

#include <stdint.h>

/* System Clock Frequency (Core Clock) */
uint32_t SystemCoreClock = 25000000;  /* 25 MHz for simulation */

/**
 * @brief Setup the microcontroller system.
 *        Initialize the System.
 */
void SystemInit (void)
{
    /* For ARM Cortex-M3 simulation, no special system initialization is needed */
    /* In a real system, this would configure:
     * - Clock sources and PLLs
     * - Flash wait states
     * - Voltage regulator settings
     * - Memory protection unit (MPU)
     * - Cache and TCM settings
     */
    
    /* Set System Clock to 25 MHz for simulation */
    SystemCoreClock = 25000000;
}

/**
 * @brief Update SystemCoreClock variable.
 *        Updates the SystemCoreClock with current core Clock retrieved from CPU registers.
 */
void SystemCoreClockUpdate (void)
{
    /* For simulation, the clock is fixed */
    SystemCoreClock = 25000000;
}