/*******************************************************************************
* File Name        : specs.h
*
* Description      : This is the application specification header file to select 
*                    the required power mode configuration
*
* Related Document : See README.md
*
*******************************************************************************
* Copyright 2023-2025, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

#ifndef SPECS_H
#define SPECS_H

/* Consult the PSOC Edge E84 datasheet (Section 8.2 , Power supplies and 
 * operating conditions) and pick one of the SPEC IDs from the CPU currents 
 * table among the ones listed below as SPEC_ID constants. Set the #define SPEC_ID 
 * to the desired value. You can also set to CUSTOM and define your own settings.
 */
#define SPEC_ID (1)

/* SPEC IDs CONSTANTS (Do not alter these macros) */
/* System Active Mode (HP, LP and ULP) */
#define SIDH00A (1)
#define SIDH01A (2)
#define SIDH10A (3)
#define SIDH12A (4)
#define SIDH14A (5)
#define SIDH20A (6)
#define SIDL00B (7)
#define SIDL01B (8)
#define SIDL10B (9)
#define SIDL12B (10)
#define SIDL14B (11)
#define SIDL20B (12)
#define SIDU00C (13)
#define SIDU01C (14)
#define SIDU10C (15)
#define SIDU12C (16)
#define SIDU14C (17)
#define SIDU20C (18)
/* System Deepsleep-OFF Mode */
#define SIDDSO  (19)
/* System Hibernate Mode */
#define SIDHIBA (20)
/* CUSTOM Configuration */
#define CUSTOM  (21)
/* END OF SPEC ID CONTSTANTS */

/* Default DPLL Input frequency and timeout */
#define DPLL_INTPUT_FREQ_HZ        (50000000U)
#define DPLL_ENABLE_TIMEOUT_MS     (10000U)

#if(SPEC_ID != CUSTOM)

#if (SPEC_ID == SIDH00A) || (SPEC_ID == SIDH01A)
#define SYSTEM_HP1
#endif 

#if (SPEC_ID == SIDH00A ) || (SPEC_ID == SIDH01A) || (SPEC_ID == SIDH10A) || \
    (SPEC_ID == SIDH12A) || (SPEC_ID == SIDH14A) || (SPEC_ID == SIDH20A)
#define SYSTEM_HP
#endif 

#if (SPEC_ID == SIDL00B) || (SPEC_ID == SIDL01B)
#define SYSTEM_LP
#define DPLL_FREQ_LP_HZ  (70000000U)
#define SET_HF_DIV_ONE
#endif 

#if (SPEC_ID == SIDL10B) || (SPEC_ID == SIDL12B) || (SPEC_ID == SIDL14B) || \
    (SPEC_ID == SIDL20B)
#define SYSTEM_LP
#define DPLL_FREQ_LP_HZ  (140000000U)
#endif 

#if (SPEC_ID == SIDU00C) || (SPEC_ID == SIDU01C) || (SPEC_ID == SIDU10C) || \
    (SPEC_ID == SIDU12C) || (SPEC_ID == SIDU14C) || (SPEC_ID == SIDU20C)
#define SYSTEM_ULP
#endif 

#if (SPEC_ID == SIDH00A) || (SPEC_ID == SIDL00B) || (SPEC_ID == SIDU00C)
    #define DISABLE_PD1
    #define CM33_SLEEP
    #define CM55_DEEP_SLEEP
#elif (SPEC_ID == SIDH01A) || (SPEC_ID == SIDL01B) || (SPEC_ID == SIDU01C)
    #define DISABLE_PD1
    #define CM33_DHRYSTONE
    #define CM55_DEEP_SLEEP
#elif (SPEC_ID == SIDH10A) || (SPEC_ID == SIDL10B) || (SPEC_ID == SIDU10C)
    #define DISABLE_CM55
    #define CM33_SLEEP
    #define CM55_DEEP_SLEEP
#elif (SPEC_ID == SIDH12A) || (SPEC_ID == SIDL12B) || (SPEC_ID == SIDU12C)
    #define CM33_SLEEP
    #define CM55_SLEEP
#elif (SPEC_ID == SIDH14A) || (SPEC_ID == SIDL14B) || (SPEC_ID == SIDU14C) 
    #define CM33_SLEEP
    #define CM55_DHRYSTONE
    #define DISABLE_U55
#elif (SPEC_ID == SIDH20A) || (SPEC_ID == SIDL20B) || (SPEC_ID == SIDU20C)
    #define CM33_DHRYSTONE
    #define CM55_DHRYSTONE
    #define DISABLE_U55
#elif (SPEC_ID == SIDHIBA)
    #define CM33_HIBERNATE
#elif (SPEC_ID == SIDDSO)
    #define CM33_DEEP_SLEEP
    #define CM55_DEEP_SLEEP
    #define DISABLE_PD1
#endif /* if(SPEC_ID != CUSTOM) */

#elif(SPEC_ID == CUSTOM)
#define DPLL_FREQ_LP_HZ            (140000000U)

/* Options for the system power mode:
 * SYSTEM_HP: High-performance mode for maximum processor speed
 * SYSTEM_LP: Low-power mode for reduced processor speed and power consumption
 * SYSTEM_ULP: Ultra-low-power mode for minimal power consumption 
 */
#define SYSTEM_HP

/* Option to disable CM55 core:
 * Uncomment the below macro to disable CM55 core. 
 */
/* #define DISABLE_CM55 */

/* Option to disable Power Domain1 (PD1):
 * Uncomment the below macros to disable Power Domain 1. 
 */
/* #define DISABLE_PD1 */
/* #define CM55_DEEP_SLEEP */

/* Options for the CM33 super loop:
 * CM33_DHRYSTONE: Run Dhrystone benchmark on CM33 CPU
 * CM33_WHILE_LOOP: Run infinite loop and keep CM33 CPU active
 * CM33_SLEEP: Put CM33 CPU in low-power sleep mode
 * CM33_DEEP_SLEEP: Put CM33 CPU in deep sleep mode 
 * CM33_HIBERNATE: Put the System to Hibernate mode 
 */
#define CM33_DHRYSTONE

/* Options for the CM55 super loop. CM55 should be in deepsleep 
 * mode and should not be altered when PD1 is disabled 
 * in above settings:
 * CM55_DHRYSTONE: Run Dhrystone benchmark on CM55 CPU
 * CM55_WHILE_LOOP: Run infinite loop and keep CM55 CPU active
 * CM55_SLEEP: Put CM55 CPU in low-power sleep mode
 * CM55_DEEP_SLEEP: Put CM55 CPU in deep sleep mode 
 */
#if !defined(DISABLE_PD1)
#define CM55_DHRYSTONE

#endif
#endif

/*******************************************************************************
* Function Name: handle_app_error
********************************************************************************
* Summary:
* User defined error handling function
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
__STATIC_INLINE void handle_app_error(void)
{
    /* Disable all interrupts. */
    __disable_irq();

    CY_ASSERT(0);

    /* Infinite loop */
    while(true);
}

#endif

/* [] END OF FILE */