/**********************************************************************************************************************
 * \file Tof.c
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
#include "Tof.h"
#include "Can.h"
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
volatile CanMessage frontTofMsg;
volatile CanMessage tofMsg;
volatile CanMessage SendFrontTofMsg;
volatile CanMessage SendLeftTofMsg;


volatile static uint32 rawFrontDistance,frontDistance;
volatile static uint32 rawRearDistance,rearDistance;

static MovingAverageFilter frontTofFilter,rearTofFilter;
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*--------------------------------------------Private Variables/Constants--------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
void Tof_Init(void)
{
    Can_Message_Init(&SendLeftTofMsg, SEND_TOF_DUMMY, 0x401, 8); //REAR TOF
    Can_Message_AddSignal(&SendLeftTofMsg, 0, 24, 0xFFFFFF);
    Can_Message_AddSignal(&SendLeftTofMsg, 24,8, 1);
    Can_Message_AddSignal(&SendLeftTofMsg, 32, 32, 0xFFFFFFFF);

    Can_Message_Init(&SendFrontTofMsg, SEND_TOF_DUMMY, 0x402, 8); //FRONT TOF
    Can_Message_AddSignal(&SendFrontTofMsg, 0, 24, 0xFFFFFF);
    Can_Message_AddSignal(&SendFrontTofMsg, 24,8, 2);
    Can_Message_AddSignal(&SendFrontTofMsg, 32, 32, 0xFFFFFFFF);
}


uint32 Tof_Read(TofChannel ch)
{
    if(Can_RxQueue_Pop(1,&tofMsg))
    {
        switch(ch)
        {
            case FRONT_TOF :
            {
                if(tofMsg.id==0x202)
                {

                    rawFrontDistance=Can_Message_Parse(tofMsg.dataLow, tofMsg.dataHigh, 0, 24);
                    return rawFrontDistance;
                }
            }
            break;

            case REAR_TOF :
            {
                if(tofMsg.id==0x201)
                {
                    rawRearDistance =Can_Message_Parse(tofMsg.dataLow, tofMsg.dataHigh, 0, 24);
                    return rawRearDistance;
                }
            }
            break;
        }
    }
    return 0;
}
void Tof_Write_Dummy(TofChannel ch) // TOF Inquire Can Message
{
    switch(ch)
    {
        case FRONT_TOF :
        {
            Can_TxQueue_Push(TOFSENSOR,&SendFrontTofMsg);
        }
        break;

        case REAR_TOF :
        {
            Can_TxQueue_Push(TOFSENSOR,&SendLeftTofMsg);
        }
        break;
    }
}



// 필터 초기화 함수
void Tof_FilterInit(MovingAverageFilter* filter)
{
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        filter->buffer[i] = 0;
    }
    filter->index = 0;
    filter->sum = 0;
}

// 새로운 샘플을 추가하고 평균값을 계산하는 함수
uint32 addSample(MovingAverageFilter* filter, uint32 sample)
{
    // 기존 샘플에서 제거할 값
    uint32 removed = filter->buffer[filter->index];

    // 새로운 샘플을 버퍼에 추가
    filter->buffer[filter->index] = sample;

    // 합에 새로운 샘플을 더하고, 이전 샘플을 뺀다
    filter->sum += sample - removed;

    // 인덱스를 순환시킨다
    filter->index = (filter->index + 1) % SAMPLE_SIZE;

    // 평균값을 리턴
    return filter->sum / SAMPLE_SIZE;
}

/*********************************************************************************************************************/
