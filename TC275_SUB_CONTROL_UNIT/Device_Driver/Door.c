/**********************************************************************************************************************
 * \file Door.c
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
#include "Door.h"
#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "IfxGtm.h"
#include "IfxPort.h"
#include "stdbool.h"
#include "IfxGtm_Atom_Pwm.h"

IfxGtm_Atom_Pwm_Driver g_atomDriver;          /* PWM 드라이버 */
IfxGtm_Atom_Pwm_Config g_atomConfig;          /* PWM 설정 */

/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
#define PWM_PERIOD 20000  /* 20ms = 50Hz (서보모터 기본 주파수) */
#define PORT_PWM IfxGtm_ATOM0_0_TOUT0_P02_0_OUT
#define CLOSE_DUTY  500
#define OPEN_DUTY   1600

#define MAX(a,b) a>b?a:b
#define MIN(a,b) a<b?a:b
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
static uint32 cur_duty;

int dutyIncrement  = 0;        // 현재 변화 속도 (마이크로초/업데이트 주기)
int maxStep = 50;           // 최대 변화 속도 (예시 값, 상황에 맞게 조정)
int stepIncrement = 10;       // 가속도 (업데이트 당 변화량, 예시 값)


/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*--------------------------------------------Private Variables/Constants--------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
static void setDutyCycle(uint32 dutyCycle);
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
void Door_Init(void)
{
        IfxGtm_enable(&MODULE_GTM); /* GTM 활성화 */


        IfxGtm_Cmu_setClkFrequency(&MODULE_GTM, IfxGtm_Cmu_Clk_0, 1000000.0f);  /* 클럭 주파수 설정 */
        IfxGtm_Cmu_enableClocks(&MODULE_GTM, IFXGTM_CMU_CLKEN_CLK0); /* 클럭 활성화 */
        IfxGtm_Atom_Pwm_initConfig(&g_atomConfig, &MODULE_GTM);

        g_atomConfig.atom = PORT_PWM.atom;                        /* ATOM 채널 0 설정 */
        g_atomConfig.atomChannel = PORT_PWM.channel;              /* ATOM 채널 0 설정 */
        g_atomConfig.dutyCycle = 0;
        g_atomConfig.period = PWM_PERIOD;     /* 20ms 주기 설정 (50Hz 주파수) */
        g_atomConfig.pin.outputPin = &PORT_PWM; /* 서보를 위한 출력 핀 설정 */
        g_atomConfig.synchronousUpdateEnabled = TRUE;

        IfxGtm_Atom_Pwm_init(&g_atomDriver, &g_atomConfig);
        IfxGtm_Atom_Pwm_start(&g_atomDriver, TRUE);

        cur_duty=CLOSE_DUTY;
        setDutyCycle(cur_duty);
}

void setDutyCycle(uint32 dutyCycle)
{
    g_atomConfig.period = PWM_PERIOD;
    g_atomConfig.dutyCycle = dutyCycle;
    IfxGtm_Atom_Pwm_init(&g_atomDriver, &g_atomConfig);
}

void Door_Open(void)
{
        if(abs(OPEN_DUTY - cur_duty) > 1)
        {
            uint32 remainingDuty = abs(OPEN_DUTY - cur_duty);

            if ((uint32)(dutyIncrement*dutyIncrement) / (2 * stepIncrement) >= remainingDuty)
            {
                //만약
                dutyIncrement= MAX(dutyIncrement-stepIncrement,0);
            }

            else
            {
                dutyIncrement= MIN(dutyIncrement+stepIncrement,maxStep);
            }

            if (OPEN_DUTY > cur_duty)
            {
                cur_duty += dutyIncrement;
            }

            else
            {
                cur_duty -= dutyIncrement;
            }
            setDutyCycle(cur_duty);
        }
}

void Door_Close(void)
{
        if(abs(CLOSE_DUTY - cur_duty) > 1)
        {
            uint32 remainingDuty = abs(CLOSE_DUTY - cur_duty);

            // 감속 시작 조건: 남은 거리로 감속할 수 있는지 판단 (수학적으로: d = v² / (2a))
            if ((dutyIncrement  * dutyIncrement ) / (2 * stepIncrement) >= remainingDuty)
            {
                dutyIncrement  = MAX(dutyIncrement  - stepIncrement, 0);
            }
            else
            {
                dutyIncrement  = MIN(dutyIncrement  + stepIncrement, maxStep);
            }

            // 목표 방향으로 PWM 값 업데이트
            if (CLOSE_DUTY < cur_duty)
            {
                cur_duty -= dutyIncrement ;
            }
            else
            {
                cur_duty += dutyIncrement ;
            }

            // PWM 신호 출력 (여기서는 예시로 setPWM 함수를 호출)
            setDutyCycle(cur_duty);
        }
}

boolean Door_Open_Completed(void)
{
    return (abs(OPEN_DUTY - cur_duty) <=1);
}
boolean Door_Close_Completed(void)
{
    return (abs(CLOSE_DUTY - cur_duty) <=1);
}
/*********************************************************************************************************************/
