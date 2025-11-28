/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * @brief    A project template for M031 MCU.
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (C) 2017 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "rom.h"
#include "NuMicro.h"
#include "uart.h"
/*
 * This is a template project for M031 series MCU. Users could based on this project to create their
 * own application without worry about the IAR/Keil project settings.
 *
 * This template application uses external crystal as HCLK source and configures UART0 to print out
 * "Hello World", users may need to do extra system configuration based on their system design.
 */

IROM2_SECTION void SYS_Init(void)
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

    /* Enable UART0 clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Switch UART0 clock source to HIRC */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* Update System Core Clock */
    SystemCoreClockUpdate();

    /* Set PB multi-function pins for UART0 RXD=PB.12 and TXD=PB.13 */
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk))    |       \
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    /* Lock protected registers */
    SYS_LockReg();

}


IROM2_SECTION int main(void)
{

    /* Unlock protected registers */
    SYS_UnlockReg();
    
    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();
    UART_Open(UART0, 115200);

    CLK->AHBCLK |= CLK_AHBCLK_ISPCKEN_Msk;

    FMC->ISPCTL |= (FMC_ISPCTL_ISPEN_Msk | FMC_ISPCTL_APUEN_Msk);

    /* Connect UART to PC, and open a terminal tool to receive following message */
//     printf("Hello\n");
    UART_Write(UART0, "Hello\n", 6);
    GPIO_SetMode(PB, BIT14, GPIO_MODE_QUASI);

    /* Got no where to go, just loop forever */
    while(1){
        PB14 ^= 1;
        CLK_SysTickDelay(500000);
    }

//     if (DetectPin != 0)
//     {
//         goto _APROM;
//     }


// _APROM:
//     /* Change vector page address to APROM */
//     FMC_SetVectorPageAddr(FMC_APROM_BASE);
//     /* Point to the reset handler address which application uses */
//     func = (FUNC_PTR *) * (uint32_t *)(FMC_APROM_BASE + 4);
//     /* Set stack pointer base address to the one which application uses */
//     __set_SP(*(uint32_t *)FMC_APROM_BASE);
//     func();

    /* Trap the CPU */
//     while (1);
}

/**
 * @brief 硬體錯誤處理函式 (Hard Fault Processor)
 * * 此函式在發生 Hard Fault 時被啟動程式碼呼叫。
 * 您可以在此處實作任何錯誤處理或除錯邏輯。
 */
IROM2_SECTION uint32_t ProcessHardFault(uint32_t lr, uint32_t msp, uint32_t psp)
{
    // 在這裡加入您的錯誤處理邏輯，例如：
    // 1. 讓一個 LED 閃爍以指示錯誤。
    // 2. 存取堆疊資訊以進行更深層的除錯。
    // 3. 永遠停留在此處，以便除錯器可以連上來檢查狀態。

    // 簡單地進入無限迴圈
    while (1)
    {
        // 保持迴圈
    }
}