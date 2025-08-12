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
/*
 * This is a template project for M031 series MCU. Users could based on this project to create their
 * own application without worry about the IAR/Keil project settings.
 *
 * This template application uses external crystal as HCLK source and configures UART0 to print out
 * "Hello World", users may need to do extra system configuration based on their system design.
 */

void SYS_Init(void)
{
        /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable HIRC clock (Internal RC 48MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);

    /* Wait for HIRC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Select HCLK clock source as HIRC and HCLK source divider as 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    /* Set Flash Access Delay */
    FMC->FTCTL |= FMC_FTCTL_FOM_Msk;

    /* Update System Core Clock */
    SystemCoreClockUpdate();

    /* Switch USB clock source to HIRC */
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~CLK_CLKSEL0_USBDSEL_Msk) | CLK_CLKSEL0_USBDSEL_HIRC;
    /* USB Clock = HIRC / 1 */
    CLK->CLKDIV0 = CLK->CLKDIV0 & ~CLK_CLKDIV0_USBDIV_Msk;

    /* Enable module clock */
    CLK->APBCLK0 |= CLK_APBCLK0_USBDCKEN_Msk;


    /* Lock protected registers */
    SYS_LockReg();

}
void gotoAPROM(void)
{
    /* Boot from AP */
    FMC->ISPCTL &= ~FMC_ISPCTL_BS_Msk;
    NVIC_SystemReset();
    //SYS->IPRST0 = SYS_IPRST0_CPURST_Msk;
    while(1);
}


int main()
{
    uint32_t u32TrimInit;

    /* The code should boot from LDROM: check the boot setting */
    
    /* Check if GPA.0 is low */
    if (PE8 != 0)
    {
        /* Boot from AP */
        gotoAPROM();
    }

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable FMC ISP function. Before using FMC function, it should unlock system register first. */
    FMC->ISPCTL = FMC_ISPCTL_ISPEN_Msk|FMC_ISPCTL_APUEN_Msk;
    
    SYS_Init();

    /* Got no where to go, just loop forever */
    while(1);
}

/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/
