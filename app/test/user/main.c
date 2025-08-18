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
#define IROM2_SECTION __attribute__((section(".irom2_text"), used))
void UART_Open(UART_T *uart, uint32_t u32baudrate);

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

/*
 * This is a template project for M031 series MCU. Users could based on this project to create their
 * own application without worry about the IAR/Keil project settings.
 *
 * This template application uses external crystal as HCLK source and configures UART0 to print out
 * "Hello World", users may need to do extra system configuration based on their system design.
 */

#define IROM2_SECTION_D __attribute__((section(".rodata_irom2"), used))

IROM2_SECTION_D uint8_t const pu8TxBuff[] = "Hello World WTF\n\r";
IROM2_SECTION_D uint32_t const u32WriteBytes = sizeof(pu8TxBuff) - 1;
IROM2_SECTION int main()
{
    SYS_Init();

    /* Init UART0 to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    /* Connect UART to PC, and open a terminal tool to receive following message */
    // printf("Hello World\n");

    uint32_t u32Count = 0;
    uint8_t pu8TxBuf[30] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    // uint32_t u32WriteBytes = sizeof(pu8TxBuf) - 1;
    for (uint32_t i = 0; i < u32WriteBytes; i++)
    {
        pu8TxBuf[i] = pu8TxBuff[i];
    }
    u32Count = 0;
    while(1)
    {
        /* Print out a message every 1000 ms */
        // if(u32Count++ >= 1000)
        // {
            // u32Count = 0;
            // printf("Hello World\n");
            UART_Write(UART0, pu8TxBuf, u32WriteBytes);
        // }
    }
}
/* memcpy: 將 src 的前 n 個 byte 複製到 dest */
void *memcpy(void *dest, const void *src, size_t n)
{
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;

    while (n--) {
        *d++ = *s++;
    }

    return dest;
}
/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/
