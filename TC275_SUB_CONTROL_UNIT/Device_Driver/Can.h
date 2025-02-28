#ifndef CAN_H_
#define CAN_H_

#include "Platform_Types.h"


#define SRC_MESSAGE_OBJECT_ID       (IfxMultican_MsgObjId)0     /* Source message object ID                          */
#define DST_MESSAGE_OBJECT_ID       (IfxMultican_MsgObjId)1     /* Destination message object ID                     */
#define CAN_MESSAGE_ID              0x777                       /* Message ID that will be used in arbitration phase */
#define TX_INTERRUPT_SRC_ID         IfxMultican_SrcId_0         /* Transmit interrupt service request ID             */
#define RX_INTERRUPT_SRC_ID         IfxMultican_SrcId_1         /* Receive interrupt service request ID              */
#define PIN5                        5                           /* LED1 used in TX ISR is connected to this pin      */
#define PIN6                        6                           /* LED2 used in RX ISR is connected to this pin      */
#define INVALID_DATA_VALUE          (uint32)0xDEADBEEF          /* Used to invalidate RX message data content        */
#define INVALID_ID_VALUE            (uint32)0xFFFFFFFF          /* Used to invalidate RX message ID value            */
#define ISR_PRIORITY_CAN_TX         2                           /* Define the CAN TX interrupt priority              */
#define ISR_PRIORITY_CAN_RX         1                           /* Define the CAN RX interrupt priority              */
#define ECU1_TX_OBJ_NUM             10u
#define ECU1_RX_OBJ_NUM             10u
#define CAN_QUEUE_SIZE              256
#define CAN_TX_QUEUE_SIZE           256

#define CAN_CH0                     0
#define CAN_CH1                     1



typedef enum {
    TOF_MESSAGE = 0x100,
    MOTOR_CONTROL=0x001,
    MOTOR_INFO_SEND_TO_CCU = 0x002,
    DOOR_MESSAGE =0x300,
    DOOR_OPEN_COMPLETE = 0x301,
    SEND_TOF_DUMMY = 0x401,
    DOOR_CLOSE_COMPLETE =0x302
} CanMessageId;

typedef enum
{
    ECU=0,
    TOFSENSOR=1
}Channel;

typedef struct
{
    CanMessageId msg;
    uint32 id;
    uint8 dlc;
    uint32 dataLow;
    uint32 dataHigh;
} CanMessage;


typedef struct
{
    CanMessage messages[CAN_QUEUE_SIZE];
    uint32 head;
    uint32 tail;
}CanQueue;


void Can_Init(void);
void Can_Message_Init(CanMessage *message,CanMessageId msg, uint32 id, uint8 dlc);
void Can_Message_AddSignal(CanMessage *message, uint8 startBit, uint8 length, uint32 value);
void Can_Message_Send(Channel ch,CanMessage *message);
uint32 Can_Message_Parse(uint32 dataLow, uint32 dataHigh, uint8 startBit, uint8 length);
int Can_RxQueue_Push(Channel Channel,const CanMessage *message);
int Can_RxQueue_Pop(Channel Channel,CanMessage *message);
int Can_TxQueue_Push(Channel Channel, const CanMessage *message);
int Can_TxQueue_Pop(Channel Channel,CanMessage *message);
CanMessage Can_Get_Message(Channel Channel);


#endif
