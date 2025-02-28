/**********************************************************************************************************************
 * \file CCU.c
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
#include "Application/CCU.h"
#include "Application/Vehicle_State_Controller.h"
#include "Application/AppScheduler.h"
#include "Device_Driver/can.h"
#include "Device_Driver/UartPacket.h"
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/

/******stateflow variable********/

/*******input********/
bool isStart;
bool isArrived;
OperationMode opMode;
uint16_t left_speed, right_speed;
uint32_t r_distance=150, f_distance=150; // Can RX
OperationDoorState opDoorState;
bool isOpenComplete;
bool isCloseComplete;
/*******************/

/*******output********/
bool isEnd;
bool obstacle_detected;
DW_State_Control cur_state;
Vehicle_State vehicle_state ;
Gear_State gear_state;
Door_State door_state;
/*******input********/

/*******************************/
uint16_t target_speed_m_s;
uint16_t yaw_rate;
boolean sign_yaw_rate;
boolean power_brake_flag;
TxPacket txpacket;
RxPacket rxpacket;
uint32_t isUart_Stable_count;
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*--------------------------------------------Private Variables/Constants--------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
static void Event_10ms (void);
static void Can_Handler (void);
static void Send_Motor_Control_Msg (void);
static void Send_Power_Brake(void);
static void Receive_Rpi_Handler(void);
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
void CCU_Init(void)
{
    UartPacket_Init();
    Can_Init(BD_500K);
    App_Init();

}
void CCU_Main(void)
{
    while (1)
    {
       Receive_Rpi_Handler();
       Can_Handler();

       State_Control(isStart, isArrived, opMode, left_speed, right_speed, r_distance,f_distance
               ,opDoorState,isOpenComplete,isCloseComplete,&isEnd,&vehicle_state,&gear_state
               ,&obstacle_detected,&door_state,&cur_state);

       txpacket.obstacle_detected = obstacle_detected;

       if(isOpenComplete)
       {
           txpacket.door_status_for_rpi=1;
       }

       else if(isCloseComplete)
       {
           txpacket.door_status_for_rpi=0;
       }

       if(vehicle_state==IDLE)
       {
           txpacket.is_End=1;
       }

       else if(vehicle_state==WORK)
       {
           txpacket.is_End=0;
       }

       if(opDoorState==OPEN &&door_state==CLOSED)
       {
           Send_Door_Control_Msg(OPEN);
       }
       else if(opDoorState==CLOSE && door_state==OPENED)
       {
           Send_Door_Control_Msg(CLOSE);
       }

        if (App_Check_Flag(FLAG_10MS))
        {
            App_Clear_Flag(FLAG_10MS);
            Event_10ms();
        }

    }

}


static void Event_10ms (void)
{
    UartPacket_to_Rpi(txpacket);

    Send_Power_Brake();
    if(!obstacle_detected)
    {
        Send_Motor_Control_Msg();
    }
    CanMessage txMsg;
    if (CanTxQueue_Pop(&txMsg) > 0)
    {
        Can_SendMsg(txMsg.id, txMsg.data, txMsg.dlc);
    }
}
static void Can_Handler (void)
{
    CanMessage rxMsg;
    CanMessage txMsg;
    if (CanTxQueue_Pop(&txMsg) > 0)
    {
        Can_SendMsg(txMsg.id, txMsg.data, txMsg.dlc);
    }
    if (CanRxQueue_Pop(&rxMsg) > 0)
    {
        switch (rxMsg.id)
        {
            case TOF_MESSAGE :
            {
                f_distance = (uint32_t) CanMessage_Parse(rxMsg.dataLow, rxMsg.dataHigh, 0, 24);
                r_distance = (uint32_t) CanMessage_Parse(rxMsg.dataLow, rxMsg.dataHigh, 32, 24);
            }
                break;

            case MOTOR_INFO_SEND_TO_CCU :
            {
                left_speed = (uint16) CanMessage_Parse(rxMsg.dataLow, rxMsg.dataHigh, 0, 16);
                right_speed = (uint16) CanMessage_Parse(rxMsg.dataLow, rxMsg.dataHigh, 16, 16);
                txpacket.yaw_rate_fdb =(uint16) CanMessage_Parse(rxMsg.dataLow, rxMsg.dataHigh, 32, 16);
                txpacket.yaw_rate_sign_fdb =(uint8) CanMessage_Parse(rxMsg.dataLow, rxMsg.dataHigh, 48, 1);
                txpacket.speed_fdb = (uint16)(left_speed+right_speed)/2;

                txpacket.left_wheel_speed =left_speed;
                txpacket.right_wheel_speed =right_speed;

            }
                break;

            case DOOR_OPEN_COMPLETE :
            {
                isOpenComplete = CanMessage_Parse(rxMsg.dataLow, rxMsg.dataHigh, 0, 1);

            }
                break;

            case DOOR_CLOSE_COMPLETE :
            {
                isCloseComplete = CanMessage_Parse(rxMsg.dataLow, rxMsg.dataHigh, 0, 1);
            }
                break;
        }
    }
}

static void Send_Motor_Control_Msg (void)
{

    uint8_t canData[8];
    CanMessage canMessage;
    CanMessage_Init(&canMessage, MOTOR_CONTROL, 0x001, 5u);

    CanMessage_AddSignal(&canMessage, 0, 16, yaw_rate);
    CanMessage_AddSignal(&canMessage, 16, 16, target_speed_m_s);
    CanMessage_AddSignal(&canMessage, 32, 2, gear_state);
    CanMessage_AddSignal(&canMessage, 34, 1, vehicle_state);
    CanMessage_AddSignal(&canMessage, 35, 1, sign_yaw_rate);

    Convert_CanMessage_To_ByteArray(&canMessage, canData, 5u);

    CanTxQueue_Push(&canMessage);

}
void Send_Door_Control_Msg (OperationDoorState cmd)
{
    uint8_t canData[8];
    CanMessage canMessage;
    CanMessage_Init(&canMessage, DOOR_MESSAGE, 0x300, 1u);
    CanMessage_AddSignal(&canMessage, 0, 1, cmd);
    Convert_CanMessage_To_ByteArray(&canMessage, canData, 1u);
    CanTxQueue_Push(&canMessage);
}

void Send_Power_Brake(void)
{
    uint8_t canData[8];
    CanMessage canMessage;
    CanMessage_Init(&canMessage, POWER_BRAKE, 0x003, 1u);
    CanMessage_AddSignal(&canMessage, 0, 1, obstacle_detected);
    Convert_CanMessage_To_ByteArray(&canMessage, canData, 1u);
    CanTxQueue_Push(&canMessage);
}
static void Receive_Rpi_Handler(void)
{
    rxpacket = UartPacket_from_Rpi();
    isStart =rxpacket.taxi_working;
    isArrived = rxpacket.is_Arrived;
    opMode =rxpacket.operation;
    target_speed_m_s =rxpacket.target_speed;
    yaw_rate =rxpacket.yaw_rate;
    sign_yaw_rate =rxpacket.yaw_rate_sign;
    opDoorState =rxpacket.door_request_from_app;
}
/*********************************************************************************************************************/
