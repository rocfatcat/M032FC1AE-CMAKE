/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * @brief    A project template for M031 MCU.
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (C) 2017 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "rom.h"
#define TRIM_INIT           (SYS_BASE+0x118)
IROM2_SECTION int SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable FMC ISP function. Before using FMC function, it should unlock system register first. */
    FMC->ISPCTL = FMC_ISPCTL_ISPEN_Msk|FMC_ISPCTL_APUEN_Msk;
    
    /* Enable Internal RC 48MHz clock */
    CLK->PWRCTL = (CLK_PWRCTL_HIRCEN_Msk);

    /* Set Flash Access Delay */
    FMC->FTCTL |= FMC_FTCTL_FOM_Msk;

    /* Set core clock */
    /* Switch HCLK clock source to HIRC */
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~CLK_CLKSEL0_HCLKSEL_Msk) | CLK_CLKSEL0_HCLKSEL_HIRC;
    /* Switch USB clock source to HIRC */
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~CLK_CLKSEL0_USBDSEL_Msk) | CLK_CLKSEL0_USBDSEL_HIRC;
    /* USB Clock = HIRC / 1 */
    CLK->CLKDIV0 = CLK->CLKDIV0 & ~CLK_CLKDIV0_USBDIV_Msk;

    /* Enable module clock */
    CLK->APBCLK0 |= CLK_APBCLK0_USBDCKEN_Msk;

    /* Enable UART0 clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Switch UART0 clock source to HIRC */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* Update System Core Clock */
    SystemCoreClockUpdate();

    /* Set PB multi-function pins for UART0 RXD=PF.2 and TXD=PF.3 */
    SYS->GPF_MFPL = (SYS->GPF_MFPL & ~(SYS_GPF_MFPL_PF2MFP_Msk | SYS_GPF_MFPL_PF3MFP_Msk))    |       \
                    (SYS_GPF_MFPL_PF2MFP_UART0_RXD | SYS_GPF_MFPL_PF3MFP_UART0_TXD);

    /* Lock protected registers */
    SYS_LockReg();
}

/*
 * This is a template project for M031 series MCU. Users could based on this project to create their
 * own application without worry about the IAR/Keil project settings.
 *
 * This template application uses external crystal as HCLK source and configures UART0 to print out
 * "Hello World", users may need to do extra system configuration based on their system design.
 */


IROM2_SECTION void gotoAPROM(void)
{
    /* Boot from AP */
    FMC->ISPCTL &= ~FMC_ISPCTL_BS_Msk;
    NVIC_SystemReset();
    //SYS->IPRST0 = SYS_IPRST0_CPURST_Msk;
    while(1);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
IROM2_SECTION int32_t main(void)
{   
    uint32_t u32TrimInit;

    /* The code should boot from LDROM: check the boot setting */
    
    /* Check if GPA.0 is low */
//     if (PE8 != 0)
//     {
//         /* Boot from AP */
//         gotoAPROM();
//     }

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable FMC ISP function. Before using FMC function, it should unlock system register first. */
    FMC->ISPCTL = FMC_ISPCTL_ISPEN_Msk|FMC_ISPCTL_APUEN_Msk;
    
    SYS_Init();
    UART_Open(UART0, 115200);
    UART_Write(UART0, "Hello World\n", 13);

    /* Start of USBD_Start() */

    /* Backup default trim */
    u32TrimInit = M32(TRIM_INIT);

    while(1)
    {
        // if (PE8)
        // {   
        //     /* Reset */
        //     gotoAPROM();
        // }
    	CLK_SysTickDelay(10000);
    	UART_Write(UART0, "Hello World2\n\r", 15);
    }
}
