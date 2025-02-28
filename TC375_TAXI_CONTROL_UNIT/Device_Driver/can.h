///**********************************************************************************************************************
// * \file can.h
// * \copyright Copyright (C) Infineon Technologies AG 2019
// *
// * Use of this file is subject to the terms of use agreed between (i) you or the company in which ordinary course of
// * business you are acting and (ii) Infineon Technologies AG or its licensees. If and as long as no such terms of use
// * are agreed, use of this file is subject to following:
// *
// * Boost Software License - Version 1.0 - August 17th, 2003
// *
// * Permission is hereby granted, free of charge, to any person or organization obtaining a copy of the software and
// * accompanying documentation covered by this license (the "Software") to use, reproduce, display, distribute, execute,
// * and transmit the Software, and to prepare derivative works of the Software, and to permit third-parties to whom the
// * Software is furnished to do so, all subject to the following:
// *
// * The copyright notices in the Software and this entire statement, including the above license grant, this restriction
// * and the following disclaimer, must be included in all copies of the Software, in whole or in part, and all
// * derivative works of the Software, unless such copies or derivative works are solely in the form of
// * machine-executable object code generated by a source language processor.
// *
// * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
// * COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN
// * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// * IN THE SOFTWARE.
// *********************************************************************************************************************/
//
//#ifndef CAN_H_
//#define CAN_H_
//
//
//
///*********************************************************************************************************************/
///*-----------------------------------------------------Includes------------------------------------------------------*/
///*********************************************************************************************************************/
//#include <stdio.h>
//#include <string.h>
//#include "Ifx_Types.h"
//#include "IfxCan_Can.h"
//#include "IfxCan.h"
//#include "IfxCpu_Irq.h"
//#include "IfxPort.h"                                        /* For GPIO Port Pin Control                            */
//
///*********************************************************************************************************************/
///*------------------------------------------------------Macros-------------------------------------------------------*/
///*********************************************************************************************************************/
//#define CAN_MESSAGE_ID              (uint32)0x777           /* Message ID that will be used in arbitration phase    */
//#define MAXIMUM_CAN_DATA_PAYLOAD    2                       /* Define maximum classical CAN payload in 4-byte words */
//
///*********************************************************************************************************************/
///*--------------------------------------------------Data Structures--------------------------------------------------*/
///*********************************************************************************************************************/
//typedef struct
//{
//    IfxCan_Can_Config canConfig;                            /* CAN module configuration structure                   */
//    IfxCan_Can canModule;                                   /* CAN module handle                                    */
//    IfxCan_Can_Node canSrcNode;                             /* CAN source node handle data structure                */
//    IfxCan_Can_Node canDstNode;                             /* CAN destination node handle data structure           */
//    IfxCan_Can_NodeConfig canNodeConfig;                    /* CAN node configuration structure                     */
//    IfxCan_Filter canFilter;                                /* CAN filter configuration structure                   */
//    IfxCan_Message txMsg;                                   /* Transmitted CAN message structure                    */
//    IfxCan_Message rxMsg;                                   /* Received CAN message structure                       */
//    uint8 txData[8];                                        /* Transmitted CAN data array                           */
//    uint8 rxData[8];                                        /* Received CAN data array                              */
//} McmcanType;
//
//typedef enum {
//    BD_NOUSE = 0,
//    BD_500K = 1,
//    BD_1M = 2
//} CAN_BAUDRATES;
//
//
//typedef enum {
//    DOOR_CONTROL_BCU_to_FDCU = 0x001,
//    DOOR_CONTROL_BCU_to_RDCU = 0x002,
//    DOOR_STATUS_FDCU_to_BCU  = 0x003,
//    DOOR_STATUS_RDCU_to_BCU  = 0x004,
//    DOOR_RETURN_OK_FDCU_to_BCU = 0x005,
//    DOOR_RETURN_OK_RDCU_to_BCU = 0x006,
//    RFID_CARD_INFO_FDCU_to_BCU = 0x101,
//    RFID_CARD_INFO_RDCU_to_BCU = 0x102,
//    RFID_CARD_RESPONSE_BCU_to_FDCU = 0x103,
//    RFID_CARD_RESPONSE_BCU_to_RDCU = 0x104
//} CanMessageId;
//
//
//typedef struct {
//    CanMessageId msg;
//    uint32 id;
//    uint8 dlc;
//    uint32 dataLow;
//    uint32 dataHigh;
//} CanMessage;
//
///*********************************************************************************************************************/
///*-----------------------------------------------Function Prototypes-------------------------------------------------*/
///*********************************************************************************************************************/
//void Can_Init(CAN_BAUDRATES ls_baudrate);
//void Can_SetFilterRange(uint32 start, uint32 end);
//void Can_SetFilterMask(uint32 id, uint32 mask);
//
//void Can_SendMsg(unsigned int id, const unsigned char *txData, int len);
//int Can_RecvMsg(unsigned int *id, unsigned char *rxData, int *len);
//
//void CanMessage_Init(CanMessage *message,CanMessageId msg, uint32 id, uint8 dlc);
//void CanMessage_AddSignal(CanMessage *message, uint8 startBit, uint8 length, uint32 value);
//void Convert_CanMessage_To_ByteArray(const CanMessage *message, unsigned char *txData, int len);
//
//uint32 CanMessage_Parse(uint32 dataLow, uint32 dataHigh, uint8 startBit, uint8 length);
//void Can_ReceiveAndParseMsg(void);
//
//#endif /* CAN_H_ */



/**********************************************************************************************************************
 * \file can.h
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

#ifndef CAN_H_
#define CAN_H_



/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include "Ifx_Types.h"
#include "IfxCan_Can.h"
#include "IfxCan.h"
#include "IfxCpu_Irq.h"
#include "IfxPort.h"                                        /* For GPIO Port Pin Control                            */

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define CAN_MESSAGE_ID              (uint32)0x777           /* Message ID that will be used in arbitration phase    */
#define MAXIMUM_CAN_DATA_PAYLOAD    2                       /* Define maximum classical CAN payload in 4-byte words */
#define CAN_RX_QUEUE_SIZE   256
#define CAN_TX_QUEUE_SIZE   256
#define ISR_PRIORITY_CAN_TX         2       /* Define the CAN TX interrupt priority */
#define ISR_PRIORITY_CAN_RX         1       /* Define the CAN RX interrupt priority */
/*********************************************************************************************************************/
/*--------------------------------------------------Data Structures--------------------------------------------------*/
/*********************************************************************************************************************/
typedef struct
{
    IfxCan_Can_Config canConfig;                            /* CAN module configuration structure                   */
    IfxCan_Can canModule;                                   /* CAN module handle                                    */
    IfxCan_Can_Node canSrcNode;                             /* CAN source node handle data structure                */
    IfxCan_Can_Node canDstNode;                             /* CAN destination node handle data structure           */
    IfxCan_Can_NodeConfig canNodeConfig;                    /* CAN node configuration structure                     */
    IfxCan_Filter canFilter;                                /* CAN filter configuration structure                   */
    IfxCan_Message txMsg;                                   /* Transmitted CAN message structure                    */
    IfxCan_Message rxMsg;                                   /* Received CAN message structure                       */
    uint8 txData[8];                                        /* Transmitted CAN data array                           */
    uint8 rxData[8];                                        /* Received CAN data array                              */
} McmcanType;

typedef enum {
    BD_NOUSE = 0,
    BD_500K = 1,
    BD_1M = 2
} CAN_BAUDRATES;


typedef enum {
    TOF_MESSAGE = 0x100,
    MOTOR_CONTROL=0x001,
    MOTOR_INFO_SEND_TO_CCU = 0x002,
    DOOR_MESSAGE =0x300,
    DOOR_OPEN_COMPLETE = 0x301,
    POWER_BRAKE =0x003,
    DOOR_CLOSE_COMPLETE =0x302
} CanMessageId;


typedef struct {
    CanMessageId msg;
    uint32 id;
    uint8 dlc;
    uint32 dataLow;
    uint32 dataHigh;
    uint8 data[8];
} CanMessage;

typedef struct
{
    CanMessage messages[CAN_RX_QUEUE_SIZE];
    uint32 head;
    uint32 tail;
}CanQueue;


/*********************************************************************************************************************/
/*-----------------------------------------------Function Prototypes-------------------------------------------------*/
/*********************************************************************************************************************/
void Can_Init(CAN_BAUDRATES ls_baudrate);
void Can_SetFilterRange(uint32 start, uint32 end);
void Can_SetFilterMask(uint32 id, uint32 mask);

void Can_SendMsg(unsigned int id, const unsigned char *txData, int len);
int Can_RecvMsg(unsigned int *id, unsigned char *rxData, int *len);

void CanMessage_Init(CanMessage *message,CanMessageId msg, uint32 id, uint8 dlc);
void CanMessage_AddSignal(CanMessage *message, uint8 startBit, uint8 length, uint32 value);
void Convert_CanMessage_To_ByteArray( CanMessage *message, unsigned char *txData, int len);

uint32 CanMessage_Parse(uint32 dataLow, uint32 dataHigh, uint8 startBit, uint8 length);
void Can_ReceiveAndParseMsg(void);

uint8 is_rx_flag();
void reset_rx_flag();
int CanRxQueue_Push(const CanMessage *message);
int CanRxQueue_Pop(CanMessage *message);
int CanTxQueue_Push(const CanMessage *message);

int CanTxQueue_Pop(CanMessage *message);



#endif /* CAN_H_ */
