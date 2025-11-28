/******************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Use embedded data flash as storage to implement a USB Mass-Storage device.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "M031Series_User.h"
#include "NuMicro.h"
#include "rom.h"
#define TRIM_INIT           (SYS_BASE+0x118)
int SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
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
    // /* Switch USB clock source to HIRC */
    // CLK->CLKSEL0 = (CLK->CLKSEL0 & ~CLK_CLKSEL0_USBDSEL_Msk) | CLK_CLKSEL0_USBDSEL_HIRC;
    // /* USB Clock = HIRC / 1 */
    // CLK->CLKDIV0 = CLK->CLKDIV0 & ~CLK_CLKDIV0_USBDIV_Msk;

    /* Enable module clock */
    CLK->APBCLK0 |= CLK_APBCLK0_USBDCKEN_Msk;

    /* Enable UART0 clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Switch UART0 clock source to HIRC */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* Update System Core Clock */
    SystemCoreClockUpdate();

    /* Set PF multi-function pins for UART0 RXD=PF.2 and TXD=PF.3 */
    SYS->GPF_MFPL = (SYS->GPF_MFPL & ~(SYS_GPF_MFPL_PF2MFP_Msk | SYS_GPF_MFPL_PF3MFP_Msk))    |       \
                    (SYS_GPF_MFPL_PF2MFP_UART0_RXD | SYS_GPF_MFPL_PF3MFP_UART0_TXD);

    /* Lock protected registers */
    SYS_LockReg();
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    uint32_t u32TrimInit;
    uint32_t u32LedCounter = 0;

    /* The code should boot from LDROM: check the boot setting */
     // 設定 PA.0 為 QUASI 模式
    GPIO_SetMode(PA, BIT0, GPIO_MODE_QUASI);
    PA0 = 1;  // internal pull high
    /* Check if GPA.0 is low */
    // if (PA0 != 0)
    // {
    //     /* Boot from AP */
    //     gotoAPROM();
    // }

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable FMC ISP function. Before using FMC function, it should unlock system register first. */
    FMC->ISPCTL = FMC_ISPCTL_ISPEN_Msk|FMC_ISPCTL_APUEN_Msk;
    
    SYS_Init();
    UART_Open(UART0, 115200);
    UART_Write(UART0, "Hello World ISP MSD Bootload\n", 12);
    
    GPIO_SetMode(PA, BIT0, GPIO_MODE_OUTPUT);
    PA0 = 1;

    while(1) ;
}
