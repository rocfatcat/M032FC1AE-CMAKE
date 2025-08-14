/***************************************************************************//**
 * @file     targetdev.c
 * @brief    ISP support function source file
 * @version  0x32
 * @date     14, June, 2017
 *
 * @note
 * Copyright (C) 2017-2018 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include "targetdev.h"
#include "isp_user.h"
#include "fmc.h"

#define IROM2_SECTION __attribute__((section(".irom2_text"), used))

IROM2_SECTION uint32_t GetApromSize()
{
    return BOOTLOADER_OFFSET;
}

IROM2_SECTION void GetDataFlashInfo(uint32_t *addr, uint32_t *size)
{
    uint32_t uData;
    *size = 0;
    FMC_Read_User(Config0, (unsigned int *)&uData);

    if ((uData & 0x01) == 0)   //DFEN enable
    {
        FMC_Read_User(Config1, (unsigned int *)&uData);
        // filter the reserved bits in CONFIG1
        uData &= 0x000FFFFF;

        if (uData > g_apromSize || (uData & (FMC_FLASH_PAGE_SIZE - 1)))   //avoid config1 value from error
        {
            uData = g_apromSize;
        }

        *addr = uData;
        *size = g_apromSize - uData;
    }
    else
    {
        *addr = g_apromSize;
        *size = 0;
    }
}

