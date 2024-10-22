/*
 * Copyright 2020-2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board_init.h"
#include "demo_config.h"
#include "demo_info.h"
#include "fsl_debug_console.h"
#include "image.h"
#include "image_utils.h"
#include "model.h"
#include "output_postproc.h"
#include "timer.h"
#include "video.h"
#include "ov7670.h"
#include "servo_motor_control.h"
#include "fsl_flash.h"

static flash_config_t s_flashCfg; // flash configuration structure


extern uint32_t writeToFlash(flash_config_t * flashCfg, uint32_t destFlashAddr);


void error_trap(void)
{
    PRINTF("\r\n\r\n\r\n\t---- HALTED DUE TO FLASH ERROR! ----");
    while (1)
    {
    }
}



int main(void)
{
    uint32_t pflashBlockBase  = 0U;
    uint32_t pflashTotalSize  = 0U;  //celkova velikost flash
    uint32_t pflashSectorSize = 0U;
    uint32_t PflashPageSize   = 0U;
    uint32_t destFlashAddr = 0U;  // address where we write the current image
    status_t status;  // status of the flash command
    char ch;

    BOARD_Init();
    TIMER_Init();

    DEMO_PrintInfo();

    Ov7670_Init();

    display_init();

    ezh_start();

    /* Clean up Flash, Cache driver Structure*/
    memset(&s_flashCfg, 0, sizeof(flash_config_t));
    if (FLASH_Init(&s_flashCfg) != kStatus_Success) {
        error_trap();
    }
    /* Get flash properties kFLASH_ApiEraseKey */
    FLASH_GetProperty(&s_flashCfg, kFLASH_PropertyPflashBlockBaseAddr, &pflashBlockBase);
    FLASH_GetProperty(&s_flashCfg, kFLASH_PropertyPflashSectorSize, &pflashSectorSize);
    FLASH_GetProperty(&s_flashCfg, kFLASH_PropertyPflashTotalSize, &pflashTotalSize);
    FLASH_GetProperty(&s_flashCfg, kFLASH_PropertyPflashPageSize, &PflashPageSize);

    /* print welcome message */
    PRINTF("\r\n PFlash Information:");
    /* Print flash information - PFlash. */
    PRINTF("\r\n kFLASH_PropertyPflashBlockBaseAddr = 0x%X", pflashBlockBase);
    PRINTF("\r\n kFLASH_PropertyPflashSectorSize = %d", pflashSectorSize);
    PRINTF("\r\n kFLASH_PropertyPflashTotalSize = %d", pflashTotalSize);
    PRINTF("\r\n kFLASH_PropertyPflashPageSize = 0x%X", PflashPageSize);
    destFlashAddr = 0x20000UL; //pflashTotalSize / 2;
    status = FLASH_Erase(&s_flashCfg, destFlashAddr, pflashTotalSize - destFlashAddr, kFLASH_ApiEraseKey);
    if (status != kStatus_Success) {
        error_trap();
    }
    PRINTF("\r\n FLASH erased");

    //face_det();

    while(1) {
        ch = GETCHAR();
        PUTCHAR(ch);
        if (ch == 'f') {
            destFlashAddr = writeToFlash(&s_flashCfg, destFlashAddr);
            if (destFlashAddr == 0) {
                error_trap();
            } else {
                PRINTF("\r\n SNAPSHOT stored into flash");
            }
        }
    }
}
