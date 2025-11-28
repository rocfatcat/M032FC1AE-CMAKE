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
#include "massstorage.h"
#include "NuMicro.h"
#include "rom.h"
#define TRIM_INIT           (SYS_BASE+0x118)
IROM2_SECTION int SYS_Init(void)
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

    /* Set PF multi-function pins for UART0 RXD=PF.2 and TXD=PF.3 */
    SYS->GPF_MFPL = (SYS->GPF_MFPL & ~(SYS_GPF_MFPL_PF2MFP_Msk | SYS_GPF_MFPL_PF3MFP_Msk))    |       \
                    (SYS_GPF_MFPL_PF2MFP_UART0_RXD | SYS_GPF_MFPL_PF3MFP_UART0_TXD);

    /* Lock protected registers */
    SYS_LockReg();
}

IROM2_SECTION void gotoAPROM_Old(void)
{
    /* Boot from AP */
    FMC->ISPCTL &= ~FMC_ISPCTL_BS_Msk;
    NVIC_SystemReset();
    //SYS->IPRST0 = SYS_IPRST0_CPURST_Msk;
    while(1);
}

IROM2_SECTION void gotoAPROM()
{
#ifdef __GNUC__                        /* for GNU C compiler */
    uint32_t    u32Data;
#endif

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable FMC ISP function. Before using FMC function, it should unlock system register first. */
    FMC_Open();

    NVIC_DisableIRQ(USBD_IRQn);

    /*  NOTE!
     *     Before change VECMAP, user MUST disable all interrupts.
     */
    FMC_SetVectorPageAddr(FMC_APROM_BASE);        /* Vector remap APROM page 0 to address 0. */
    if (g_FMC_i32ErrCode != 0)
    {
        while (1);
    }

    FMC->ISPCTL &= ~FMC_ISPCTL_BS_Msk;

    SYS_LockReg();                                /* Lock protected registers */

    /* Software reset to boot to LDROM */
    NVIC_SystemReset();
}
/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
IROM2_SECTION int32_t main(void)
{
    uint32_t u32TrimInit;
    uint32_t u32LedCounter = 0;
    char buffer[35]; // 足夠大的緩衝區 (32 位整數最大 32 位 + 負號 + '\0')


    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable FMC ISP function. Before using FMC function, it should unlock system register first. */
    FMC->ISPCTL = FMC_ISPCTL_ISPEN_Msk|FMC_ISPCTL_APUEN_Msk;
    
    SYS_Init();
    UART_Open(UART0, 115200);
    UART_Write(UART0, "ISP MSD Bootload\r\n", 18);

    /* The code should boot from LDROM: check the boot setting */
    // 設定 PA.0 為 QUASI 模式
    GPIO_SetMode(PA, BIT0, GPIO_MODE_QUASI);
    PA0 = 1;  // internal pull high
    /* Check if GPA.0 is low */
    if (PA0)
    {
        /* Boot from AP */
        gotoAPROM();
    }
    
    GPIO_SetMode(PA, BIT0, GPIO_MODE_OUTPUT);
    PA0 = 1;

    
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
    while(1)
    {
        if(u32LedCounter++ > 100000)
        {
            PA0 ^= 1;
            u32LedCounter = 0;
        }
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
			
        MSC_ProcessCmd();

        // if (PE8 != 0)
        // {
        //     /* Reset */
        //     gotoAPROM();
        // }
    }
}
