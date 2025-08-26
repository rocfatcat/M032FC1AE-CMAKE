/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * @brief    A project template for M031 MCU.
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (C) 2017 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "M031Series_User.h"
#include "massstorage.h"
#include "NuMicro.h"
#include "rom.h"
void UART_Open(UART_T *uart, uint32_t u32baudrate);
#define TRIM_INIT           (SYS_BASE+0x118)
IROM2_SECTION void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable FMC ISP function. Before using FMC function, it should unlock system register first. */
    FMC->ISPCTL = FMC_ISPCTL_ISPEN_Msk|FMC_ISPCTL_APUEN_Msk;
    
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
    
    /* Switch USB clock source to HIRC & USB Clock = HIRC / 1 */
    CLK_SetModuleClock(USBD_MODULE, CLK_CLKSEL0_USBDSEL_HIRC, CLK_CLKDIV0_USB(1));
    
    /* Enable USB clock */
    CLK_EnableModuleClock(USBD_MODULE);
    
    /* Update System Core Clock */
    SystemCoreClockUpdate();
    
    /* Set PB multi-function pins for UART0 RXD=PB.12 and TXD=PB.13 */
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk))    |       \
    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);
    
    /* Set Flash Access Delay */
    FMC->FTCTL |= FMC_FTCTL_FOM_Msk;
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

IROM2_SECTION void USBProcess(uint32_t u32TrimInit)
{
       /* Start USB trim if it is not enabled. */
        if((SYS->HIRCTRIMCTL & SYS_HIRCTRIMCTL_FREQSEL_Msk) != 1)
        {
            /* Start USB trim only when SOF */
            if(USBD->INTSTS & USBD_INTSTS_SOFIF_Msk)
            {
                /* Clear SOF */
                USBD->INTSTS = USBD_INTSTS_SOFIF_Msk;

                /* Re-enable crystal-less */
                SYS->HIRCTRIMCTL = 0x01;
                SYS->HIRCTRIMCTL |= SYS_HIRCTRIMCTL_REFCKSEL_Msk;
            }
        }

        /* Disable USB Trim when error */
        if(SYS->HIRCTRIMSTS & (SYS_HIRCTRIMSTS_CLKERIF_Msk | SYS_HIRCTRIMSTS_TFAILIF_Msk))
        {
            /* Init TRIM */
            M32(TRIM_INIT) = u32TrimInit;

            /* Disable crystal-less */
            SYS->HIRCTRIMCTL = 0;

            /* Clear error flags */
            SYS->HIRCTRIMSTS = SYS_HIRCTRIMSTS_CLKERIF_Msk | SYS_HIRCTRIMSTS_TFAILIF_Msk;

            /* Clear SOF */
            USBD->INTSTS = USBD_INTSTS_SOFIF_Msk;
        }	
}

IROM2_SECTION void Func_Config(uint32_t u32TrimInit)
{
    SYS_Init();
    /* Init UART0 to 115200-8n1 for print message */
    // UART_Open(UART0, 115200);
    USBD_Open(&gsInfo);
    /* Endpoint configuration */
    MSC_Init();

    /* Start of USBD_Start() */
    CLK_SysTickDelay(100000);


    /* Disable software-disconnect function */
    USBD->SE0 = 0;

    /* Clear USB-related interrupts before enable interrupt */
    USBD->INTSTS = (USBD_INT_BUS | USBD_INT_USB | USBD_INT_FLDET | USBD_INT_WAKEUP);

    /* Enable USB-related interrupts. */
    USBD->INTEN = (USBD_INT_BUS | USBD_INT_USB | USBD_INT_FLDET | USBD_INT_WAKEUP);
    /* End of USBD_Start() */

    NVIC_EnableIRQ(USBD_IRQn);

    /* Backup default trim */
    u32TrimInit = M32(TRIM_INIT);

    /* Clear SOF */
    USBD->INTSTS = USBD_INTSTS_SOFIF_Msk;
}

int main()
{
    uint32_t u32TrimInit;

    /* The code should boot from LDROM: check the boot setting */
    
    /* Check if GPA.0 is low */
    // if (PE8 != 0)
    if( 0 )
    {
        /* Boot from AP */
        gotoAPROM();
    }

    Func_Config(u32TrimInit);
    
    
    while(1)
    {
        USBProcess(u32TrimInit);
				
        MSC_ProcessCmd();

        // if (PE8)
        if( 0)
        {   
            /* Reset */
            gotoAPROM();
        }   

    }
}
/* memcpy: 將 src 的前 n 個 byte 複製到 dest */
// void *memcpy(void *dest, const void *src, size_t n)
// {
//     unsigned char *d = (unsigned char *)dest;
//     const unsigned char *s = (const unsigned char *)src;

//     while (n--) {
//         *d++ = *s++;
//     }

//     return dest;
// }
// void *memset(void *s, int c, size_t n) {
//     unsigned char *p = s;
//     while (n--) *p++ = (unsigned char)c;
//     return s;
// }
/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/
