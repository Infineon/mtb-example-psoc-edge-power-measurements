/*******************************************************************************
* File Name        : main.c
*
* Description      : This source file contains the main routine for CM55 CPU
*
* Related Document : See README.md
*
********************************************************************************
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

/*******************************************************************************
* Header Files
*******************************************************************************/
#include "cybsp.h"
#include "specs.h"
#include "dhry.h"
#include "cy_device.h"
#include "ppu_v1.h"
#include "cy_syspm_ppu.h"
#include "cy_syspm_pdcm.h"

/*******************************************************************************
* Macros
*******************************************************************************/
#define DEEP_SLEEP_OFF_TOKEN        (0xB1)
#define PERI_NUM                    (1U)
#define GROUP_NUM                   (1U)
#define SLAVE_NUM_0                 (0U)
#define SLAVE_NUM_1                 (1U)
#define SLAVE_NUM_2                 (2U)

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function for CM55 application. The programs executes the 
* selected options, as configured in the specs.h header file.
*
* Parameters:
* void
*
* Return:
* int
*
*******************************************************************************/
int main(void)
{

    cy_rslt_t result = CY_RSLT_SUCCESS;

    /* Initialize the device and board peripherals */ 
    result = cybsp_init();

    /*  Board init failed. Stop program execution */
    if (CY_RSLT_SUCCESS != result)
    {
        handle_app_error();
    }

    /* Enable global interrupts */
    __enable_irq();
 
#if defined(DISABLE_PD1)
     /* Set deepsleep mode to deepsleep off */
     Cy_SysPm_SetDeepSleepMode(CY_SYSPM_DEEPSLEEP_OFF); 
#endif
 
    /* Disable PeriGroupSlave */
    Cy_SysClk_PeriGroupSlaveDeinit(PERI_NUM, GROUP_NUM,
     SLAVE_NUM_0);
    Cy_SysClk_PeriGroupSlaveDeinit(PERI_NUM, GROUP_NUM, 
    SLAVE_NUM_1);
    Cy_SysClk_PeriGroupSlaveDeinit(PERI_NUM, GROUP_NUM, 
    SLAVE_NUM_2);
    
    /* Disable HF clocks which are not required */
    Cy_SysClk_ClkHfDisable(CY_CFG_SYSCLK_CLKHF3);
    Cy_SysClk_ClkHfDisable(CY_CFG_SYSCLK_CLKHF4);
    Cy_SysClk_ClkHfDisable(CY_CFG_SYSCLK_CLKHF5);
    Cy_SysClk_ClkHfDisable(CY_CFG_SYSCLK_CLKHF6);
    Cy_SysClk_ClkHfDisable(CY_CFG_SYSCLK_CLKHF7);
    Cy_SysClk_ClkHfDisable(CY_CFG_SYSCLK_CLKHF9);
    Cy_SysClk_ClkHfDisable(CY_CFG_SYSCLK_CLKHF10);
    Cy_SysClk_ClkHfDisable(CY_CFG_SYSCLK_CLKHF11);
    Cy_SysClk_ClkHfDisable(CY_CFG_SYSCLK_CLKHF12);
    Cy_SysClk_ClkHfDisable(CY_CFG_SYSCLK_CLKHF13);

#if defined(DISABLE_CM55)
    /* Disable APPCPU(CM55) power*/ 
    (void)cy_pd_ppu_set_power_mode((struct ppu_v1_reg *)CY_PPU_APPCPU_BASE, 
             (uint32_t)PPU_V1_MODE_OFF);    
#endif

#if defined(DISABLE_PD1)  
    /* Clear APPCPUSS and SYSCPU power domain dependency */
    cy_pd_pdcm_clear_dependency(CY_PD_PDCM_APPCPUSS , 
    CY_PD_PDCM_SYSCPU);
    
    /* Disable PD1 power*/  
    (void)cy_pd_ppu_set_power_mode((struct ppu_v1_reg *)CY_PPU_PD1_BASE, 
             (uint32_t)PPU_V1_MODE_OFF);
    
    /* Disable SoCMEM power */ 
    (void)cy_pd_ppu_set_power_mode((struct ppu_v1_reg *)CY_PPU_SOCMEM_BASE, 
             (uint32_t)PPU_V1_MODE_OFF);
    
    /* Disable APPCPUSS power */
    (void)cy_pd_ppu_set_power_mode((struct ppu_v1_reg *)CY_PPU_APPCPUSS_BASE, 
             (uint32_t)PPU_V1_MODE_OFF);
    
    /* Disable APPCPU(CM55) power */
    (void)cy_pd_ppu_set_power_mode((struct ppu_v1_reg *)CY_PPU_APPCPU_BASE, 
             (uint32_t)PPU_V1_MODE_OFF);
#endif   
   
    for (;;)
    {
        /* Check if CM55 should enter CPU Sleep */
        #if defined(CM55_SLEEP)
        Cy_SysPm_CpuEnterSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
        
        /* Check if CM55 should enter CPU Deep Sleep */
        #elif defined(CM55_DEEP_SLEEP)
        SRSS_PWR_HIBERNATE = (SRSS_PWR_HIBERNATE  | DEEP_SLEEP_OFF_TOKEN);
        Cy_SysPm_CpuEnterDeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
        
        /* Check if CM55 should run a while loop */
        #elif defined(CM55_WHILE_LOOP)
        while (true);
        
        /* Check if CM55 should run Dhrystone */
        #elif defined(CM55_DHRYSTONE) 
        dhrystone();
        #endif
    }
}

/* [] END OF FILE */