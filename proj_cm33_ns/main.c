/*******************************************************************************
* File Name        : main.c
*
* Description      : This source file contains the main routine for non-secure
*                    application in the CM33 CPU
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
#include "cy_syspm_ppu.h"
#include "cy_device.h"

/*******************************************************************************
* Macros
*******************************************************************************/
/* The timeout value in microsecond used to wait for core to be booted */
#define CM55_BOOT_WAIT_TIME_USEC      (10U)
#define DPLL_LP_FREQ_HP_MODE          (200000000U)
#define WAIT_FOR_CM55_EXE             (300U)
#define DELAY_FOR_HIBERNATE           (100U)

/* App boot address for CM55 project */
#define CM55_APP_BOOT_ADDR                (CYMEM_CM33_0_m55_nvm_C_START + \
                                           CYBSP_MCUBOOT_HEADER_SIZE)

#if !defined(SYSTEM_HP) && !defined(SYSTEM_ULP) && (SPEC_ID != SIDDSA) \
     && (SPEC_ID != SIDHIBA)  && (SPEC_ID != SIDDSO)
/*******************************************************************************
* Function Name: dpll_lp_set_freq
********************************************************************************
* Summary:
* Configures the DPLL-LP (Low Power DPLL) frequency.
*
* Parameters:
* uint32_t frequency
*
* Return:
* void
*
*******************************************************************************/
static void dpll_lp_set_freq(uint32_t freq)
{
    /* Define a PLL configuration structure */
    cy_stc_pll_config_t dpll_lp;

    /* Set the input frequency of the PLL */
    dpll_lp.inputFreq = DPLL_INTPUT_FREQ_HZ;

    /* Set the output mode of the PLL to auto */
    dpll_lp.outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO;

    /* Set the desired output frequency of the PLL */
    dpll_lp.outputFreq = freq;

    /* Disable the DPLL_HP_0 PLL path */
    Cy_SysClk_PllDisable(SRSS_DPLL_LP_0_PATH_NUM);

    /* Configure the PLL with the specified settings */
    if (CY_SYSCLK_SUCCESS !=
            Cy_SysClk_PllConfigure(SRSS_DPLL_LP_0_PATH_NUM, 
            &dpll_lp))
    {
        /* Assert if PLL configuration fails */
       handle_app_error();
    }

    /* Enable the DPLL_HP_0 PLL path with a timeout */
    if (CY_SYSCLK_SUCCESS !=
            Cy_SysClk_PllEnable(SRSS_DPLL_LP_0_PATH_NUM, 
            DPLL_ENABLE_TIMEOUT_MS))
    {
        /* Assert if PLL enable fails */
        handle_app_error();
    }
}
#endif

/*******************************************************************************
* Function Name: configure_system_power_mode
********************************************************************************
* Summary:
* Configures the system power mode to LP or ULP mode. The default configuration 
* is set to HP mode via the device configurator tool. 
*******************************************************************************/
static void configure_system_power_mode(void)
{ 

#if defined(SYSTEM_LP)
    cy_en_syspm_status_t status;
    
    /* Set the DPLL-HP frequency to the low power frequency */
    dpll_lp_set_freq(DPLL_FREQ_LP_HZ);

#if defined(SET_HF_DIV_ONE)  
     /* Set HF0 and HF1 Clock divider to 1 */
    Cy_SysClk_ClkHfSetDivider(CY_CFG_SYSCLK_CLKHF0, CY_SYSCLK_CLKHF_NO_DIVIDE);
    Cy_SysClk_ClkHfSetDivider(CY_CFG_SYSCLK_CLKHF1, CY_SYSCLK_CLKHF_NO_DIVIDE);
#endif

    /*  Enter Low Power (LP) mode. */
    status = Cy_SysPm_SystemEnterLp();
    if (CY_SYSPM_SUCCESS != status)
    {
         handle_app_error();
    }
    
    /* Check if the system successfully entered LP mode. */
    if (Cy_SysPm_ReadStatus() & CY_SYSPM_STATUS_SYSTEM_LP)
    {
        /* Set the RRAM to LP voltage mode for lower power consumption. */ 
        Cy_RRAM_SetVoltageMode(RRAMC0, CY_RRAM_VMODE_LP);
    }
    
#elif defined(SYSTEM_ULP)
    cy_en_syspm_status_t status;
    
    /* Disable the high-performance DPLL and Directly clock from IHO (50 MHz)*/
    Cy_SysClk_PllDisable(SRSS_DPLL_LP_0_PATH_NUM);

    /* Enter Ultra-Low Power (ULP) mode. */
    status = Cy_SysPm_SystemEnterUlp();
    if (CY_SYSPM_SUCCESS != status)
    {
         handle_app_error();
    }
    
    /* Check if the system successfully entered ULP mode. */
    if (Cy_SysPm_ReadStatus() & CY_SYSPM_STATUS_SYSTEM_ULP)
    {
        /* Set the RRAM to ULP voltage mode for lower power consumption. */
        Cy_RRAM_SetVoltageMode(RRAMC0, CY_RRAM_VMODE_ULP);

        /* Set the high-frequency clock (CLKHF) to no divide */
        Cy_SysClk_ClkHfSetDivider(CY_CFG_SYSCLK_CLKHF0, CY_SYSCLK_CLKHF_NO_DIVIDE);
    }

#if defined(SYSTEM_HP1)
    /* Set DPLL LP Frequency to 200 MHz*/
     dpll_lp_set_freq(DPLL_LP_FREQ_HP_MODE);
 
    /* Set HF0 Clock divider to 1 */
     Cy_SysClk_ClkHfSetDivider(CY_CFG_SYSCLK_CLKHF0, CY_SYSCLK_CLKHF_NO_DIVIDE);
#endif
#endif
}

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function of the CM33 non-secure application.  
* It initializes the device and board peripherals. SMIF and SOCMEM are disabled. 
* The sytem power mode is configured as mentioned in the specs.h header file. 
* The CM55 core is enabled and then the programs enters an infinite while and 
* the selected options, as configured in the specs.h header file, are executed.
*
* Parameters:
* None
*
* Return:
* Int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;

    /* Initialize the device and board peripherals */ 
    result = cybsp_init();

    /* Board Initialization failed. Stop program execution */
    if (CY_RSLT_SUCCESS != result)
    {
        handle_app_error();
    }

   /* Enable global interrupts */
    __enable_irq();
    
    /* Disable SoCMEM */
    Cy_SysEnableSOCMEM(false);

#if defined(DEEPSLEEP_OFF)
    /* Use low-power settings for Bandgap reference circuit */
    SRSS_PWR_CTL2 |= SRSS_PWR_CTL2_BGREF_LPMODE_Msk;
#endif
    
#if defined(DISABLE_PD1)
     /* Set deepsleep mode to deepsleep off */
     Cy_SysPm_SetDeepSleepMode(CY_SYSPM_MODE_DEEPSLEEP_OFF);
#endif   

    /* CM55_APP_BOOT_ADDR must be updated if CM55 memory layout is changed.*/
    Cy_SysEnableCM55 (MXCM55, CM55_APP_BOOT_ADDR, CM55_BOOT_WAIT_TIME_USEC);
    
    /* Wait for CM55 execution */
    Cy_SysLib_Delay(WAIT_FOR_CM55_EXE);
    
    /* Configure the system power mode */
    configure_system_power_mode();

#if defined(DISABLE_PD1) 
     /* Disable HF1 and HF2 clocks */  
     Cy_SysClk_ClkHfDisable(CY_CFG_SYSCLK_CLKHF1);
     Cy_SysClk_ClkHfDisable(CY_CFG_SYSCLK_CLKHF2);
#endif

    for (;;)
    {
        #if defined(CM33_SLEEP) 
        Cy_SysPm_CpuEnterSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
        
        #elif defined(CM33_DEEP_SLEEP)
        Cy_SysPm_CpuEnterDeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
        
        #elif defined(CM33_HIBERNATE) 
        Cy_SysLib_Delay(DELAY_FOR_HIBERNATE);
        Cy_SysPm_SystemEnterHibernate();
        
        #elif defined(CM33_WHILE_LOOP)
        while(true);
        
        #elif defined(CM33_DHRYSTONE)
        dhrystone();
        #endif
    }
}

/* [] END OF FILE */
