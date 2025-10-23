/****************************************************************************
 * @file     uart.c
 * @version  V1.00
 * @brief    M031 series UART driver source file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#include <stdio.h>
#include "NuMicro.h"
#include "rom.h"

/**
 *    @brief        Open and set UART function
 *
 *    @param[in]    uart            The pointer of the specified UART module.
 *    @param[in]    u32baudrate     The baudrate of UART module.
 *
 *    @return       None
 *
 *    @details      This function use to enable UART function and set baud-rate.
 */
void UART_Open(UART_T *uart, uint32_t u32baudrate)
{
    uint32_t u32UartClkSrcSel = 0ul, u32UartClkDivNum = 0ul;
    uint32_t u32ClkTbl[6ul] = {__HXT, 0ul, __LXT, __HIRC, 0ul, __LIRC};
    uint32_t u32Baud_Div = 0ul;


    if (uart == (UART_T *)UART0)
    {
        /* Get UART clock source selection */
        u32UartClkSrcSel = ((uint32_t)(CLK->CLKSEL1 & CLK_CLKSEL1_UART0SEL_Msk)) >> CLK_CLKSEL1_UART0SEL_Pos;
        /* Get UART clock divider number */
        u32UartClkDivNum = (CLK->CLKDIV0 & CLK_CLKDIV0_UART0DIV_Msk) >> CLK_CLKDIV0_UART0DIV_Pos;
    }
   
    /* Select UART function */
    uart->FUNCSEL = UART_FUNCSEL_UART;

    /* Set UART line configuration */
    uart->LINE = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;

    /* Set UART Rx and RTS trigger level */
    uart->FIFO &= ~(UART_FIFO_RFITL_Msk | UART_FIFO_RTSTRGLV_Msk);

    /* Get PLL clock frequency if UART clock source selection is PLL */
    if (u32UartClkSrcSel == 1ul)
    {
        u32ClkTbl[u32UartClkSrcSel] = CLK_GetPLLClockFreq();
    }

    /* Get PCLK clock frequency if UART clock source selection is PCLK */
    if (u32UartClkSrcSel == 4ul)
    {
        /* UART Port as UART0 ,UART2, UART4 or UART6 */
        if ((uart == (UART_T *)UART0) || (uart == (UART_T *)UART2) || (uart == (UART_T *)UART4) || (uart == (UART_T *)UART6))
        {
            u32ClkTbl[u32UartClkSrcSel] =  CLK_GetPCLK0Freq();
        }
        else     /* UART Port as UART1, UART3, UART5 or UART7*/
        {
            u32ClkTbl[u32UartClkSrcSel] =  CLK_GetPCLK1Freq();
        }

    }

    /* Set UART baud rate */
    if (u32baudrate != 0ul)
    {
        u32Baud_Div = UART_BAUD_MODE2_DIVIDER((u32ClkTbl[u32UartClkSrcSel]) / (u32UartClkDivNum + 1ul), u32baudrate);

        if (u32Baud_Div > 0xFFFFul)
        {
            uart->BAUD = (UART_BAUD_MODE0 | UART_BAUD_MODE0_DIVIDER((u32ClkTbl[u32UartClkSrcSel]) / (u32UartClkDivNum + 1ul), u32baudrate));
        }
        else
        {
            uart->BAUD = (UART_BAUD_MODE2 | u32Baud_Div);
        }
    }
}




/**
 *    @brief        Write UART data
 *
 *    @param[in]    uart            The pointer of the specified UART module.
 *    @param[in]    pu8TxBuf        The buffer to send the data to UART transmission FIFO.
 *    @param[out]   u32WriteBytes   The byte number of data.
 *
 *    @return       u32Count transfer byte count
 *
 *    @details      The function is to write data into TX buffer to transmit data by UART.
 */
uint32_t UART_Write(UART_T *uart, uint8_t pu8TxBuf[], uint32_t u32WriteBytes)
{
    uint32_t  u32Count, u32delayno;
    uint32_t  u32Exit = 0ul;

    for (u32Count = 0ul; u32Count != u32WriteBytes; u32Count++)
    {
        u32delayno = 0ul;

        while (uart->FIFOSTS & UART_FIFOSTS_TXFULL_Msk)   /* Check Tx Full */
        {
            u32delayno++;

            if (u32delayno >= 0x40000000ul)
            {
                u32Exit = 1ul;
                break;
            }
        }

        if (u32Exit == 1ul)
        {
            break;
        }
        else
        {
            uart->DAT = pu8TxBuf[u32Count];    /* Send UART Data from buffer */
        }
    }

    return u32Count;

}

void UART_WriteHex(UART_T *uart, uint32_t value, uint8_t digits)
{
    char hex[] = "0123456789ABCDEF";
    char out[2] = {'0','0'};
    for (int i = digits - 1; i >= 0; i--)
    {
        uint8_t nibble = (value >> (i * 4)) & 0xF;
        out[0] = hex[nibble];
        UART_Write(uart,out,1);
    }
    out[0] = '\r';
    out[1] = '\n';
    UART_Write(uart,out,2);
}
/*@}*/ /* end of group UART_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group UART_Driver */

/*@}*/ /* end of group Standard_Driver */

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/



