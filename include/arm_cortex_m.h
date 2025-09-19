/**
 * @file arm_cortex_m.h
 * @brief ARM Cortex-M specific definitions and functions
 * @author Team Member 6 - System Integration
 * @date 2024
 * 
 * This file contains ARM Cortex-M specific definitions and helper functions
 * for the RTOS implementation.
 */

#ifndef ARM_CORTEX_M_H
#define ARM_CORTEX_M_H

#include "rtos_config.h"

/* ============================================================================
 * ARM CORTEX-M SPECIFIC DEFINITIONS
 * ============================================================================ */

/* Interrupt control and state register */
#define NVIC_INT_CTRL_REG       (*((volatile uint32_t*)0xE000ED04))
#define NVIC_PENDSVSET          0x10000000

/* System control block registers */
#define NVIC_SYSPRI2_REG        (*((volatile uint32_t*)0xE000ED20))
#define NVIC_SYSPRI3_REG        (*((volatile uint32_t*)0xE000ED24))
#define NVIC_PENDSV_PRI         (0xFF << 16)
#define NVIC_SYSTICK_PRI        (0xFF << 24)

/* SysTick timer registers */
#define SYSTICK_CTRL_REG        (*((volatile uint32_t*)0xE000E010))
#define SYSTICK_LOAD_REG        (*((volatile uint32_t*)0xE000E014))
#define SYSTICK_VAL_REG         (*((volatile uint32_t*)0xE000E018))
#define SYSTICK_CALIB_REG       (*((volatile uint32_t*)0xE000E01C))

/* SysTick control bits */
#define SYSTICK_ENABLE          (1 << 0)
#define SYSTICK_TICKINT         (1 << 1)
#define SYSTICK_CLKSOURCE       (1 << 2)
#define SYSTICK_COUNTFLAG       (1 << 16)

/* ============================================================================
 * INLINE ASSEMBLY FUNCTIONS
 * ============================================================================ */

/**
 * @brief Disable interrupts and return previous state
 * @return uint32_t Previous PRIMASK value
 */
static inline uint32_t __disable_irq(void)
{
    uint32_t result;
    
    #ifdef __ARMCC_VERSION
    __asm
    {
        MRS result, PRIMASK
        CPSID I
    }
    #else
    __asm volatile ("MRS %0, PRIMASK" : "=r" (result) );
    __asm volatile ("CPSID I" : : : "memory");
    #endif
    
    return result;
}

/**
 * @brief Enable interrupts
 */
static inline void __enable_irq(void)
{
    #ifdef __ARMCC_VERSION
    __asm
    {
        CPSIE I
    }
    #else
    __asm volatile ("CPSIE I" : : : "memory");
    #endif
}

/**
 * @brief Wait for interrupt
 */
static inline void __WFI(void)
{
    #ifdef __ARMCC_VERSION
    __asm
    {
        WFI
    }
    #else
    __asm volatile ("WFI");
    #endif
}

/**
 * @brief Wait for event
 */
static inline void __WFE(void)
{
    #ifdef __ARMCC_VERSION
    __asm
    {
        WFE
    }
    #else
    __asm volatile ("WFE");
    #endif
}

/**
 * @brief Send event
 */
static inline void __SEV(void)
{
    #ifdef __ARMCC_VERSION
    __asm
    {
        SEV
    }
    #else
    __asm volatile ("SEV");
    #endif
}

/**
 * @brief No operation
 */
static inline void __NOP(void)
{
    #ifdef __ARMCC_VERSION
    __asm
    {
        NOP
    }
    #else
    __asm volatile ("NOP");
    #endif
}

/* ============================================================================
 * FUNCTION PROTOTYPES
 * ============================================================================ */

/**
 * @brief Initialize ARM Cortex-M specific features
 * @return rtos_result_t Success or error code
 */
rtos_result_t cortex_m_init(void);

/**
 * @brief Configure SysTick timer
 * @param ticks Number of ticks between interrupts
 * @return rtos_result_t Success or error code
 */
rtos_result_t cortex_m_systick_config(uint32_t ticks);

/**
 * @brief Start SysTick timer
 */
void cortex_m_systick_start(void);

/**
 * @brief Stop SysTick timer
 */
void cortex_m_systick_stop(void);

/**
 * @brief Trigger PendSV interrupt for context switch
 */
void cortex_m_trigger_pendsv(void);

/**
 * @brief Set priority for PendSV and SysTick interrupts
 */
void cortex_m_set_interrupt_priorities(void);

/**
 * @brief Get current stack pointer
 * @return uint32_t Current stack pointer value
 */
uint32_t cortex_m_get_sp(void);

/**
 * @brief Set stack pointer
 * @param sp New stack pointer value
 */
void cortex_m_set_sp(uint32_t sp);

/**
 * @brief Get PSP (Process Stack Pointer)
 * @return uint32_t Current PSP value
 */
uint32_t cortex_m_get_psp(void);

/**
 * @brief Set PSP (Process Stack Pointer)
 * @param psp New PSP value
 */
void cortex_m_set_psp(uint32_t psp);

/**
 * @brief Get MSP (Main Stack Pointer)
 * @return uint32_t Current MSP value
 */
uint32_t cortex_m_get_msp(void);

/**
 * @brief Set MSP (Main Stack Pointer)
 * @param msp New MSP value
 */
void cortex_m_set_msp(uint32_t msp);

#endif /* ARM_CORTEX_M_H */