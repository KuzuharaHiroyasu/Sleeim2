/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "rm_ob1203_common.h"
extern void delay_wait(uint32_t count);

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define OB1203_PRV_OVERHEAD_CYCLES      (1000)    /* R_BSP_SoftwareDelay() overhead per call */
#define OB1203_PRV_OVERHEAD_CYCLES_64   (1000)    /* R_BSP_SoftwareDelay() overhead per call using 64-bit ints */
#define FRQSEL                          (*(volatile __near unsigned char  *)0xC2)
#define CPU_CYCLES_PER_LOOP             (6)

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
/* 遅延時間定義テーブル */
const unsigned long long bsp_delay_time[] = {
    1,
    1000,
    1000000
};

/***********************************************************************************************************************
* Function Name: delay_wait
* Description  : This asm loop executes a known number (5) of CPU cycles. If a value of '4' is passed
*                in as an argument, then this function would consume 20 CPU cycles before returning.
* Arguments    : loop_cnt - A single 32-bit value is provided as the number of loops to execute.
*              :
* Return Value : None
***********************************************************************************************************************/
uint32_t R_OB1203_GetFclkFreqHz(void)
{
    uint32_t    sys_clock_src_freq;

    /* fCLKのクロックソースが、fSUB */
    if (1U == CLS)
    {
        /* fCLKのクロックソースが、fSUB */
        sys_clock_src_freq = OB1203_SUB_CLOCK_HZ;
    }
    /* fCLKのクロックソースが、fMAIN */
    else
    {
        /* fMAINのクロックソースが、fMX */
        if (1U == MCS)
        {
            sys_clock_src_freq = OB1203_CFG_FMX_HZ;
        }
        /* fMAINのクロックソースが、fOCO */
        else
        {
            sys_clock_src_freq = OB1203_CFG_FIH_HZ;
        }
    }

    return sys_clock_src_freq;
} /* End of function R_BSP_GetFclkFreqHz() */


/*******************************************************************************************************************//**
 * @brief Delay by the specified time.
 *
 * @retval FSP_SUCCESS              successfully configured.
 ***********************************************************************************************************************/
uint8_t R_OB1203_SoftwareDelay(uint32_t delay, ob1203_delay_units_t units)
{
    volatile uint32_t fclk_rate;
    volatile uint32_t delay_cycles;
    volatile uint32_t loop_cnt;
    volatile unsigned long long loop_cnt_64;
    volatile unsigned long long delay_cycles_64;

    fclk_rate = R_OB1203_GetFclkFreqHz();  /* Get the current ICLK frequency */

    /*
     * In order to handle all possible combinations of delay/ICLK it is necessary to use 64-bit
     * integers (not all MCUs have floating point support). However, there is no native hw support
     * for 64 bit integers so it requires many more clock cycles. This is not an issue if the
     * requested delay is long enough and the ICLK is fast, but for delays in the low microseconds
     * and/or a slow ICLK we use 32 bit integers to reduce the overhead cycles of this function
     * by approximately a third and stand the best chance of achieving the requested delay.
     */
    if ((units == OB1203_DELAY_MICROSECS) &&
        (delay <= (0xFFFFFFFFUL / fclk_rate)) )  /* Ensure (fclk_rate * delay) will not exceed 32 bits */
    {
        delay_cycles = ((fclk_rate * delay) / bsp_delay_time[units]);

        if (delay_cycles > OB1203_PRV_OVERHEAD_CYCLES)
        {
            delay_cycles -= OB1203_PRV_OVERHEAD_CYCLES;
        }
        else
        {
            delay_cycles = 0;
        }

        loop_cnt = delay_cycles / CPU_CYCLES_PER_LOOP;

        if (0 == loop_cnt)
        {
            /* The requested delay is too large/small for the current ICLK. Return false which
             * also results in the minimum possible delay. */
            return 0;
        }
    }
    else
    {
        /* Casting is valid because it matches the type to the right side or argument. */
        delay_cycles_64 = (((unsigned long long)fclk_rate * (unsigned long long)delay) / bsp_delay_time[units]);

        if (delay_cycles_64 > OB1203_PRV_OVERHEAD_CYCLES_64)
        {
            delay_cycles_64 -= OB1203_PRV_OVERHEAD_CYCLES_64;
        }
        else
        {
            delay_cycles = 0;
        }

        loop_cnt_64 = delay_cycles_64 / CPU_CYCLES_PER_LOOP;

        if ((loop_cnt_64 > 0xFFFFFFFFUL) || (0 == loop_cnt_64))
        {
            /* The requested delay is too large/small for the current ICLK. Return false which
             * also results in the minimum possible delay. */
            return 0;
        }

        /* Casting is valid because it matches the type to the right side or argument. */
        loop_cnt = (uint32_t)loop_cnt_64;
    }

    delay_wait(loop_cnt);

    return 1;
} /* End of function R_BSP_SoftwareDelay() */
