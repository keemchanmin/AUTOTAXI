/**********************************************************************************************************************
 * \file AppScheduler.c
 * \copyright Copyright (C) Infineon Technologies AG 2019
 * 
 * Use of this file is subject to the terms of use agreed between (i) you or the company in which ordinary course of 
 * business you are acting and (ii) Infineon Technologies AG or its licensees. If and as long as no such terms of use
 * are agreed, use of this file is subject to following:
 * 
 * Boost Software License - Version 1.0 - August 17th, 2003
 * 
 * Permission is hereby granted, free of charge, to any person or organization obtaining a copy of the software and 
 * accompanying documentation covered by this license (the "Software") to use, reproduce, display, distribute, execute,
 * and transmit the Software, and to prepare derivative works of the Software, and to permit third-parties to whom the
 * Software is furnished to do so, all subject to the following:
 * 
 * The copyright notices in the Software and this entire statement, including the above license grant, this restriction
 * and the following disclaimer, must be included in all copies of the Software, in whole or in part, and all 
 * derivative works of the Software, unless such copies or derivative works are solely in the form of 
 * machine-executable object code generated by a source language processor.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE 
 * COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN 
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 *********************************************************************************************************************/


/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "IfxCpu.h"
#include "IfxPort.h"
#include "Bsp.h"
#include "AppScheduler.h"
/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define LED         &MODULE_P10,2                                           /* LED: Port, Pin definition            */
#define WAIT_TIME   500

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/

Ifx_TickTime g_ticksFor10us;
uint32 g_appTickCount;
volatile uint32 g_appFlag = 0;
IfxCpu_spinLock spinlock;
/*********************************************************************************************************************/
/*--------------------------------------------Private Variables/Constants--------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/



/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/
void App_Set_Flag(uint32 flag)
{
    boolean is_enable = IfxCpu_setSpinLock(&spinlock, 0x0fff);  // Spinlock 획득
    if(is_enable == TRUE){
        g_appFlag |= flag;                     // 플래그 설정 (OR 연산)
        IfxCpu_resetSpinLock(&spinlock); // Spinlock 해제
    }
}

void App_Clear_Flag(uint32 flag)
{
    boolean is_enable = IfxCpu_setSpinLock(&spinlock, 0x0fff);  // Spinlock 획득
    if(is_enable == TRUE){
        g_appFlag &= ~flag;                // 플래그 해제 (AND 연산)
        IfxCpu_resetSpinLock(&spinlock); // Spinlock 해제
    }
}

boolean App_Check_Flag(uint32 flag)
{
    boolean result;
    boolean is_enable = IfxCpu_setSpinLock(&spinlock, 0x0fff);  // Spinlock 획득
    if(is_enable == TRUE){
        result = (g_appFlag & flag) ? TRUE : FALSE;  // 플래그 체크
        IfxCpu_resetSpinLock(&spinlock); // Spinlock 해제
    }
    return result;
}

void App_Init(void)
{
    g_ticksFor10us = IfxStm_getTicksFromMicroseconds(BSP_DEFAULT_TIMER, 10);
    g_appTickCount = 0;
}

void App_Scheduling(void){
    while(1){
        waitTime(g_ticksFor10us);
        g_appTickCount++;
        if(g_appTickCount % 2 == 0){
            App_Set_Flag(FLAG_20US);
        }
        if(g_appTickCount % 10 == 0){
            App_Set_Flag(FLAG_100US);
        }
        if(g_appTickCount % 100 == 0){
            App_Set_Flag(FLAG_1MS);
        }
        if(g_appTickCount % 1000 == 0){
            App_Set_Flag(FLAG_10MS);
        }
        if(g_appTickCount % 5000 == 0){
            App_Set_Flag(FLAG_50MS);
        }
        if(g_appTickCount % 10000 == 0){
            App_Set_Flag(FLAG_100MS);
        }
        if(g_appTickCount % 50000 == 0){
            App_Set_Flag(FLAG_500MS);
        }
        if(g_appTickCount % 100000 == 0){
            App_Set_Flag(FLAG_1000MS);
            g_appTickCount = 0;
        }
    }
}
