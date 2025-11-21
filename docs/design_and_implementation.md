[Click here](../README.md) to view the README.

## Design and implementation

The design of this application is minimalistic to get started with code examples on PSOC&trade; Edge MCU devices. All PSOC&trade; Edge E84 MCU applications have a dual-CPU three-project structure to develop code for the CM33 and CM55 cores. The CM33 core has two separate projects for the secure processing environment (SPE) and non-secure processing environment (NSPE). A project folder consists of various subfolders, each denoting a specific aspect of the project. The three project folders are as follows:

**Table 1. Application projects**

Project | Description
--------|------------------------
*proj_cm33_s* | Project for CM33 secure processing environment (SPE)
*proj_cm33_ns* | Project for CM33 non-secure processing environment (NSPE)
*proj_cm55* | CM55 project

<br>

In this code example, at device reset, the secure boot process starts from the ROM boot with the secure enclave (SE) as the root of trust (RoT). From the secure enclave, the boot flow is passed on to the system CPU subsystem where the secure CM33 application starts. After all necessary secure configurations, the flow is passed on to the non-secure CM33 application. Resource initialization for this example is performed by this CM33 non-secure project. It configures the system clocks, pins, clock to peripheral connections, and other platform resources. It then enables the CM55 core using the `Cy_SysEnableCM55()` function and the CM55 core is enabled and then the program enters an infinite while and the selected options, as configured in the *specs.h* header file, are executed.

By default, the application is configured for High Performance (HP) mode with minimalistic peripheral and clock configurations. Also all three projects, such as *proj_cm33_ns*, *proj_cm_s*, and *proj_cm55* are placed in the RRAM (internal memory) using memory configurator and start address, and offset is provided accordingly for these projects in the *common.mk* file. Open Device Configurator to view the peripheral, clock, and memory configurations.

Based on the selection of the SIDs in the *specs.h* file, the clock, power modes, and other configurations are set accordingly.

A few SIDs from the datasheet are provided as macros in the *shared* > *include* > *specs.h* file. You can provide the SID number to the `SPEC_ID` macro to apply the configurations accordingly as follows: 

```
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
/* END OF SPEC ID CONSTANTS */
```

You can also provide a value as CUSTOM and define the following macros as per your requirements. 

``` 
/* Options for the system power mode:
 * SYSTEM_HP: High-performance mode for maximum processor speed
 * SYSTEM_LP: Low-power mode for reduced processor speed and power consumption
 * SYSTEM_ULP: Ultra-low-power mode for minimal power consumption 
 */
#define SYSTEM_HP

/* To disable CM55 core:
 * Uncomment the following macro to disable the CM55 core. 
 */
/* #define DISABLE_CM55 */

/* To disable Power Domain1 (PD1):
 * Uncomment the following macros to disable Power Domain 1. 
 */
/* #define DISABLE_PD1 */
/* #define CM55_DEEP_SLEEP */

/* Options for the CM33 super loop:
 * CM33_DHRYSTONE: Runs Dhrystone benchmark on CM33 CPU
 * CM33_WHILE_LOOP: Runs infinite loop and keep CM33 CPU active
 * CM33_SLEEP: Puts CM33 CPU in low-power sleep mode
 * CM33_DEEP_SLEEP: Puts CM33 CPU in DeepSleep mode 
 * CM33_HIBERNATE: Puts the System to Hibernate mode 
 */
#define CM33_DHRYSTONE

/* Options for the CM55 super loop. CM55 should be in DeepSleep 
 * mode and should not be altered when PD1 is disabled 
 * in the earlier settings:
 * CM55_DHRYSTONE: Runs Dhrystone benchmark on CM55 CPU
 * CM55_WHILE_LOOP: Runs infinite loop and keep CM55 CPU active
 * CM55_SLEEP: Put CM55 CPU in low-power sleep mode
 * CM55_DEEP_SLEEP: Puts CM55 CPU in DeepSleep mode 
 */
#if !defined(DISABLE_PD1)
#define CM55_DHRYSTONE
```

<br>