/*******************************************************************************
* File Name        : main.c
*
* Description      : This source file contains the main routine for CM55 CPU
*
* Related Document : See README.md
*
********************************************************************************
 * (c) 2023-2025, Infineon Technologies AG, or an affiliate of Infineon
 * Technologies AG. All rights reserved.
 * This software, associated documentation and materials ("Software") is
 * owned by Infineon Technologies AG or one of its affiliates ("Infineon")
 * and is protected by and subject to worldwide patent protection, worldwide
 * copyright laws, and international treaty provisions. Therefore, you may use
 * this Software only as provided in the license agreement accompanying the
 * software package from which you obtained this Software. If no license
 * agreement applies, then any use, reproduction, modification, translation, or
 * compilation of this Software is prohibited without the express written
 * permission of Infineon.
 *
 * Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
 * IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
 * THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
 * SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
 * Infineon reserves the right to make changes to the Software without notice.
 * You are responsible for properly designing, programming, and testing the
 * functionality and safety of your intended application of the Software, as
 * well as complying with any legal requirements related to its use. Infineon
 * does not guarantee that the Software will be free from intrusion, data theft
 * or loss, or other breaches ("Security Breaches"), and Infineon shall have
 * no liability arising out of any Security Breaches. Unless otherwise
 * explicitly approved by Infineon, the Software may not be used in any
 * application where a failure of the Product or any consequences of the use
 * thereof can reasonably be expected to result in personal injury.
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