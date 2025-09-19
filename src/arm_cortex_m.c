/**
 * @file arm_cortex_m.c
 * @brief ARM Cortex-M specific implementation
 * @author Team Member 6 - System Integration
 * @date 2024
 * 
 * This file contains ARM Cortex-M specific implementations for the RTOS.
 */

#include "arm_cortex_m.h"

/* ============================================================================
 * GLOBAL VARIABLES
 * ============================================================================ */
static bool cortex_m_initialized = false;

/* ============================================================================
 * PUBLIC FUNCTIONS
 * ============================================================================ */

/**
 * @brief Initialize ARM Cortex-M specific features
 */
rtos_result_t cortex_m_init(void)
{
    if(cortex_m_initialized)
    {
        return RTOS_SUCCESS;
    }
    
    /* Set interrupt priorities for PendSV and SysTick */
    cortex_m_set_interrupt_priorities();
    
    cortex_m_initialized = true;
    
    DEBUG_PRINT("ARM Cortex-M initialized\n");
    
    return RTOS_SUCCESS;
}

/**
 * @brief Configure SysTick timer
 */
rtos_result_t cortex_m_systick_config(uint32_t ticks)
{
    /* Check if ticks is within valid range */
    if(ticks == 0 || ticks > 0x00FFFFFF)
    {
        return RTOS_INVALID_PARAM;
    }
    
    /* Stop SysTick timer */
    SYSTICK_CTRL_REG = 0;
    
    /* Set reload value */
    SYSTICK_LOAD_REG = ticks - 1;
    
    /* Clear current value */
    SYSTICK_VAL_REG = 0;
    
    /* Configure SysTick */
    SYSTICK_CTRL_REG = SYSTICK_ENABLE |      /* Enable SysTick */
                       SYSTICK_TICKINT |     /* Enable interrupt */
                       SYSTICK_CLKSOURCE;    /* Use processor clock */
    
    DEBUG_PRINT("SysTick configured for %u ticks\n", ticks);
    
    return RTOS_SUCCESS;
}

/**
 * @brief Start SysTick timer
 */
void cortex_m_systick_start(void)
{
    SYSTICK_CTRL_REG |= SYSTICK_ENABLE;
}

/**
 * @brief Stop SysTick timer
 */
void cortex_m_systick_stop(void)
{
    SYSTICK_CTRL_REG &= ~SYSTICK_ENABLE;
}

/**
 * @brief Trigger PendSV interrupt for context switch
 */
void cortex_m_trigger_pendsv(void)
{
    NVIC_INT_CTRL_REG |= NVIC_PENDSVSET;
}

/**
 * @brief Set priority for PendSV and SysTick interrupts
 */
void cortex_m_set_interrupt_priorities(void)
{
    /* Set PendSV to lowest priority */
    NVIC_SYSPRI3_REG |= NVIC_PENDSV_PRI;
    
    /* Set SysTick to low priority (but higher than PendSV) */
    NVIC_SYSPRI3_REG |= NVIC_SYSTICK_PRI;
}

/**
 * @brief Get current stack pointer
 */
uint32_t cortex_m_get_sp(void)
{
    uint32_t result;
    
    #ifdef __ARMCC_VERSION
    __asm
    {
        MOV result, SP
    }
    #else
    __asm volatile ("MOV %0, SP" : "=r" (result) );
    #endif
    
    return result;
}

/**
 * @brief Set stack pointer
 */
void cortex_m_set_sp(uint32_t sp)
{
    #ifdef __ARMCC_VERSION
    __asm
    {
        MOV SP, sp
    }
    #else
    __asm volatile ("MOV SP, %0" : : "r" (sp) : "sp");
    #endif
}

/**
 * @brief Get PSP (Process Stack Pointer)
 */
uint32_t cortex_m_get_psp(void)
{
    uint32_t result;
    
    #ifdef __ARMCC_VERSION
    __asm
    {
        MRS result, PSP
    }
    #else
    __asm volatile ("MRS %0, PSP" : "=r" (result) );
    #endif
    
    return result;
}

/**
 * @brief Set PSP (Process Stack Pointer)
 */
void cortex_m_set_psp(uint32_t psp)
{
    #ifdef __ARMCC_VERSION
    __asm
    {
        MSR PSP, psp
    }
    #else
    __asm volatile ("MSR PSP, %0" : : "r" (psp));
    #endif
}

/**
 * @brief Get MSP (Main Stack Pointer)
 */
uint32_t cortex_m_get_msp(void)
{
    uint32_t result;
    
    #ifdef __ARMCC_VERSION
    __asm
    {
        MRS result, MSP
    }
    #else
    __asm volatile ("MRS %0, MSP" : "=r" (result) );
    #endif
    
    return result;
}

/**
 * @brief Set MSP (Main Stack Pointer)
 */
void cortex_m_set_msp(uint32_t msp)
{
    #ifdef __ARMCC_VERSION
    __asm
    {
        MSR MSP, msp
    }
    #else
    __asm volatile ("MSR MSP, %0" : : "r" (msp));
    #endif
}