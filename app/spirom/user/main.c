/**************************************************************************//**
 * @file     main.c
 * @version  V1.0
 * $Revision: 11 $
 * $Date: 25/09/16 10:00a $
 * @brief    Configure SPI0 as Master mode and demonstrate how to read JEDEC ID
 *           from an off-chip SPI Flash in a loop.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

#define SPI_CLK_FREQ    10000000

/* Function prototype declaration */
void SYS_Init(void);
void SPI_Init(void);

/* ------------- */
/* Main function */
/* ------------- */
int main(void)
{
    uint32_t u32DataCount;
    uint8_t u8RxData[3];
    uint32_t u32JedecId;

    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Init System, IP clock and multi-function I/O. */
    SYS_Init();
    /* Lock protected registers */
    SYS_LockReg();

    /* Configure UART0: 115200, 8-bit word, no parity bit, 1 stop bit. */
    UART_Open(UART0, 115200);

    /* Init SPI */
    SPI_Init();

    printf("\n\n");
    printf("+----------------------------------------------------------------------+\n");
    printf("|           SPI Master Mode Sample Code to Read JEDEC ID (Loop)        |\n");
    printf("+----------------------------------------------------------------------+\n");
    printf("\n");
    printf("Configure SPI0 as a master.\n");
    printf("Bit length of a transaction: 8\n");
    printf("The I/O connection for SPI0:\n");
    printf("    SPI0_SS(PA.3 controlled by GPIO)\n    SPI0_CLK(PB.14)\n");
    printf("    SPI0_MISO(PB.13)\n    SPI0_MOSI(PB.12)\n\n");
    printf("The I/O connection for UART0:\n");
    printf("    UART0_RXD(PF.2)\n    UART0_TXD(PF.3)\n\n");
    printf("Reading JEDEC ID from SPI flash...\n");

    while(1)
    {
        /* Set PA.3 low to select the slave */
        PA3 = 0;

        /* Clear TX/RX FIFO */
        SPI_ClearTxFIFO(SPI0);
        SPI_ClearRxFIFO(SPI0);

        /* Send command 0x9F */
        SPI_WRITE_TX(SPI0, 0x9F);
        /* Wait for transfer finish */
        while(SPI_IS_BUSY(SPI0));
        /* Read dummy byte */
        SPI_READ_RX(SPI0);

        /* Read 3 bytes ID */
        for(u32DataCount = 0; u32DataCount < 3; u32DataCount++)
        {
            SPI_WRITE_TX(SPI0, 0xFF);
            while(SPI_IS_BUSY(SPI0));
            u8RxData[u32DataCount] = SPI_READ_RX(SPI0);
        }

        /* Set PA.3 high to deselect the slave */
        PA3 = 1;

        u32JedecId = (u8RxData[0] << 16) | (u8RxData[1] << 8) | u8RxData[2];

        printf("JEDEC ID: 0x%X\n", u32JedecId);
        printf("  Manufacturer ID: 0x%X\n", u8RxData[0]);
        printf("  Memory Type ID: 0x%X\n", u8RxData[1]);
        printf("  Memory Capacity ID: 0x%X\n", u8RxData[2]);
        printf("\n");

        /* Delay for a while */
        CLK_SysTickDelay(1000000);
    }
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable HIRC clock */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);

    /* Waiting for HIRC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Switch HCLK clock source to HIRC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    /* Select HIRC as the clock source of UART0 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* Select PCLK1 as the clock source of SPI0 */
    CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL2_SPI0SEL_PCLK1, MODULE_NoMsk);

    /* Enable UART peripheral clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Enable SPI0 peripheral clock */
    CLK_EnableModuleClock(SPI0_MODULE);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set multi-function pins for UART0 RXD=PF.2 and TXD=PF.3 */
    SYS->GPF_MFPL = (SYS->GPF_MFPL & ~(SYS_GPF_MFPL_PF2MFP_Msk | SYS_GPF_MFPL_PF3MFP_Msk)) |
                    (SYS_GPF_MFPL_PF2MFP_UART0_RXD | SYS_GPF_MFPL_PF3MFP_UART0_TXD);

    /* Setup SPI0 multi-function pins for PB.12(MOSI), PB.13(MISO), PB.14(CLK) */
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk | SYS_GPB_MFPH_PB14MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_SPI0_MOSI | SYS_GPB_MFPH_PB13MFP_SPI0_MISO | SYS_GPB_MFPH_PB14MFP_SPI0_CLK);

    /* Configure PA.3 as GPIO output pin for manual SS control */
    GPIO_SetMode(PA, BIT3, GPIO_MODE_OUTPUT);
    PA3 = 1; /* Set high by default */


    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and CyclesPerUs automatically. */
    SystemCoreClockUpdate();
}

void SPI_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init SPI                                                                                                */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Configure as a master, clock idle low, 8-bit transaction, drive output on falling clock edge and latch input on rising edge. */
    /* Set IP clock divider. SPI clock rate = 2 MHz */
    SPI_Open(SPI0, SPI_MASTER, SPI_MODE_0, 8, SPI_CLK_FREQ);

    /* Disable the automatic hardware slave select function. */
    SPI_DisableAutoSS(SPI0);
}


/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/