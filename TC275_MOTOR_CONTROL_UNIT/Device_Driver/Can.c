
#include "IfxMultican.h"
#include "./Libraries/iLLD/TC27D/Tricore/Multican/Can/IfxMultican_Can.h"
#include "IfxMultican_PinMap.h"
#include "IfxMultican_cfg.h"
#include "IfxCan_reg.h"
#include "IfxCan_reg.h"
#include "IfxCan_regdef.h"
#include "Ifx_Types.h"
#include "IfxPort.h"
#include "Can.h"


typedef struct
{
    IfxMultican_Can        CanEcu1;
    IfxMultican_Can_Node   CanEcu1Node;
    IfxMultican_Can_MsgObj CanEcu1MsgTxObj[ECU1_TX_OBJ_NUM];
    IfxMultican_Can_MsgObj CanEcu1MsgRxObj[ECU1_RX_OBJ_NUM];
} Ecu1Can;

void Can_EnrollObject(Channel ch,sint32 msgObjId,  uint32 msgId, uint8 frameType, uint8 msgDlc,  uint32 extendedFrame, IfxMultican_Can_MsgObj* pArrObjNum);

static CanQueue canRxQueueCh0;
static CanQueue canTxQueueCh0;
static CanQueue canRxQueueCh1;
static CanQueue canTxQueueCh1;

static Ecu1Can stEcu1CanCh0;
static IfxMultican_Message stRxMsgDataCh0[10];

static Ecu1Can stEcu1CanCh1;
static IfxMultican_Message stRxMsgDataCh1[10];

IFX_INTERRUPT(CAN_RxIn0Handler_Ch0,0,30);
void CAN_RxIn0Handler_Ch0(void)
{
    IfxMultican_Status readStatus;
    IfxMultican_Status readStatus2;
    readStatus = IfxMultican_Can_MsgObj_readMessage(&stEcu1CanCh0.CanEcu1MsgRxObj[0], &stRxMsgDataCh0[0]);
    readStatus2 = IfxMultican_Can_MsgObj_readMessage(&stEcu1CanCh0.CanEcu1MsgRxObj[1], &stRxMsgDataCh0[1]);

    if (readStatus == IfxMultican_Status_newData)
    {
         CanMessage rxMessageTemp;
         rxMessageTemp.id = stRxMsgDataCh0[0].id;
         rxMessageTemp.dlc = stRxMsgDataCh0[0].lengthCode;
         rxMessageTemp.dataLow=stRxMsgDataCh0[0].data[0];
         rxMessageTemp.dataHigh=stRxMsgDataCh0[0].data[1];

         Can_RxQueue_Push(ECU,&rxMessageTemp);
    }

    if (readStatus2 == IfxMultican_Status_newData)
    {
         CanMessage rxMessageTemp2;
         rxMessageTemp2.id = stRxMsgDataCh0[1].id;
         rxMessageTemp2.dlc = stRxMsgDataCh0[1].lengthCode;
         rxMessageTemp2.dataLow=stRxMsgDataCh0[1].data[0];
         rxMessageTemp2.dataHigh=stRxMsgDataCh0[1].data[1];

         Can_RxQueue_Push(ECU,&rxMessageTemp2);
    }
}


IFX_INTERRUPT(CAN_RxIn0Handler_Ch1,0,31);
void CAN_RxIn0Handler_Ch1(void)
{
    IfxMultican_Status readStatus;
    IfxMultican_Status readStatus2;
    readStatus = IfxMultican_Can_MsgObj_readMessage(&stEcu1CanCh1.CanEcu1MsgRxObj[0], &stRxMsgDataCh1[0]);
    readStatus2 = IfxMultican_Can_MsgObj_readMessage(&stEcu1CanCh1.CanEcu1MsgRxObj[1], &stRxMsgDataCh1[1]);


    if (readStatus == IfxMultican_Status_newData)
    {
         CanMessage rxMessageTemp;
         rxMessageTemp.id = stRxMsgDataCh1[0].id;
         rxMessageTemp.dlc = stRxMsgDataCh1[0].lengthCode;
         rxMessageTemp.dataLow=stRxMsgDataCh1[0].data[0];
         rxMessageTemp.dataHigh=stRxMsgDataCh1[0].data[1];

         Can_RxQueue_Push(1,&rxMessageTemp);
    }

    if (readStatus2 == IfxMultican_Status_newData)
    {
         CanMessage rxMessageTemp2;
         rxMessageTemp2.id = stRxMsgDataCh1[1].id;
         rxMessageTemp2.dlc = stRxMsgDataCh1[1].lengthCode;
         rxMessageTemp2.dataLow=stRxMsgDataCh1[1].data[0];
         rxMessageTemp2.dataHigh=stRxMsgDataCh1[1].data[1];

         Can_RxQueue_Push(1,&rxMessageTemp2);
    }

}
void Can_Message_Init(CanMessage *message,CanMessageId msg, uint32 id, uint8 dlc)
{
    message->msg= msg;
    message->id = id;
    message->dlc = dlc;
    message->dataLow = 0;
    message->dataHigh = 0;
}

void Can_Message_AddSignal(CanMessage *message, uint8 startBit, uint8 length, uint32 value)
{
    uint32 mask = (1<<length) -1;
    uint32 maskedvalue = value & mask;
    if (startBit < 32)
    {
        message->dataLow |= (maskedvalue << startBit);
    }

    else
    {
        uint8 shiftedStartBit = startBit - 32;
        message->dataHigh |= (maskedvalue << shiftedStartBit);
    }
}
void Can_Message_Send(Channel ch, CanMessage *message)
{
    IfxMultican_Message msg;
    IfxMultican_Message_init(&msg, message->id, message->dataLow, message->dataHigh, message->dlc);

    switch(ch)
    {
        case ECU:
        {
            while (IfxMultican_Can_MsgObj_sendMessage(&stEcu1CanCh0.CanEcu1MsgTxObj[0], &msg)
                    == IfxMultican_Status_notSentBusy) {}
        }
        break;

        case TOFSENSOR :
        {
            while (IfxMultican_Can_MsgObj_sendMessage(&stEcu1CanCh1.CanEcu1MsgTxObj[0], &msg)
                    == IfxMultican_Status_notSentBusy) {}
        }
        break;
    }


}

uint32 Can_Message_Parse(uint32 dataLow, uint32 dataHigh, uint8 startBit, uint8 length)
{
    uint64 data = ((uint64)dataHigh << 32) | dataLow;
    uint64 mask = (1 << length) - 1;
    return (data >> startBit) & mask;
}

void Can_Init(void)
{

    IfxMultican_Can_Config canConfig;
    IfxMultican_Can_initModuleConfig(&canConfig, &MODULE_CAN);

    IfxMultican_Can_initModule(&stEcu1CanCh0.CanEcu1, &canConfig);
    IfxMultican_Can_initModule(&stEcu1CanCh1.CanEcu1, &canConfig);


    IfxMultican_Can_NodeConfig canNodeConfig;
    IfxMultican_Can_Node_initConfig(&canNodeConfig, &stEcu1CanCh0.CanEcu1);

    IfxMultican_Can_NodeConfig canNodeConfig2;
    IfxMultican_Can_Node_initConfig(&canNodeConfig2, &stEcu1CanCh1.CanEcu1);

#ifdef CAN_CH0  //COM
    canNodeConfig.baudrate = 500000UL;
    canNodeConfig.nodeId    = (IfxMultican_NodeId)((int)IfxMultican_NodeId_0);
    canNodeConfig.rxPin     = &IfxMultican_RXD0B_P20_7_IN;
    canNodeConfig.rxPinMode = IfxPort_InputMode_pullUp;
    canNodeConfig.txPin     = &IfxMultican_TXD0_P20_8_OUT;
    canNodeConfig.txPinMode = IfxPort_OutputMode_pushPull;

    IfxMultican_Can_Node_init(&stEcu1CanCh0.CanEcu1Node, &canNodeConfig);


    Can_EnrollObject(ECU,1u, 0x005, IfxMultican_Frame_transmit,  IfxMultican_DataLengthCode_8, FALSE, &stEcu1CanCh0.CanEcu1MsgTxObj[0]);
    Can_EnrollObject(ECU,11u, 0x001, IfxMultican_Frame_receive, IfxMultican_DataLengthCode_8, FALSE, &stEcu1CanCh0.CanEcu1MsgRxObj[0]);
    Can_EnrollObject(ECU,12u, 0x003, IfxMultican_Frame_receive, IfxMultican_DataLengthCode_8, FALSE, &stEcu1CanCh0.CanEcu1MsgRxObj[1]);

#endif



#ifdef CAN_CH1  //TOF
    canNodeConfig2.baudrate = 500000UL;
    canNodeConfig2.nodeId    = (IfxMultican_NodeId)((int)IfxMultican_NodeId_2);
    canNodeConfig2.rxPin     = &IfxMultican_RXD2A_P15_1_IN;
    canNodeConfig2.rxPinMode = IfxPort_InputMode_pullUp;
    canNodeConfig2.txPin     = &IfxMultican_TXD2_P15_0_OUT;
    canNodeConfig2.txPinMode = IfxPort_OutputMode_pushPull;

    IfxMultican_Can_Node_init(&stEcu1CanCh1.CanEcu1Node, &canNodeConfig2);


    Can_EnrollObject(TOFSENSOR,2u, 0x004, IfxMultican_Frame_transmit,  IfxMultican_DataLengthCode_8, FALSE, &stEcu1CanCh1.CanEcu1MsgTxObj[0]);
    Can_EnrollObject(TOFSENSOR,22u, 0x201, IfxMultican_Frame_receive, IfxMultican_DataLengthCode_8, FALSE, &stEcu1CanCh1.CanEcu1MsgRxObj[0]);
    Can_EnrollObject(TOFSENSOR,23u, 0x202, IfxMultican_Frame_receive, IfxMultican_DataLengthCode_8, FALSE, &stEcu1CanCh1.CanEcu1MsgRxObj[1]);

#endif
}


void Can_EnrollObject(Channel ch, sint32 msgObjId,  uint32 msgId, uint8 frameType, uint8 msgDlc,  uint32 extendedFrame, IfxMultican_Can_MsgObj* pArrObjNum)
{
    /* create message object config */

    switch(ch)
    {
        case 0 :
        {
            IfxMultican_Can_MsgObjConfig canMsgObjConfig;
            IfxMultican_Can_MsgObj_initConfig(&canMsgObjConfig, &stEcu1CanCh0.CanEcu1Node);

            canMsgObjConfig.msgObjId              = msgObjId;
            canMsgObjConfig.messageId             = msgId;
            canMsgObjConfig.frame                 = frameType;
            canMsgObjConfig.control.messageLen    = msgDlc;
            canMsgObjConfig.control.extendedFrame = extendedFrame;
            canMsgObjConfig.acceptanceMask        = 0x00000000;
            canMsgObjConfig.control.matchingId    = TRUE;

            if(frameType==IfxMultican_Frame_receive)
            {
                canMsgObjConfig.acceptanceMask        = 0x7FFFFFFF;
               // canMsgObjConfig.acceptanceMask        = 0x00000000;
                canMsgObjConfig.control.matchingId    = TRUE;
                canMsgObjConfig.rxInterrupt.enabled=TRUE;
                canMsgObjConfig.rxInterrupt.srcId=0;
                SRC_CAN_CAN0_INT0.B.SRPN =30u;
                SRC_CAN_CAN0_INT0.B.TOS =0u;
                SRC_CAN_CAN0_INT0.B.SRE =1u;

            }
            /* initialize message object */
            IfxMultican_Can_MsgObj_init(pArrObjNum, &canMsgObjConfig);
        }
        break;

        case 1 :
        {
            IfxMultican_Can_MsgObjConfig canMsgObjConfig2;
            IfxMultican_Can_MsgObj_initConfig(&canMsgObjConfig2, &stEcu1CanCh1.CanEcu1Node);

            canMsgObjConfig2.msgObjId              = msgObjId;
            canMsgObjConfig2.messageId             = msgId;
            canMsgObjConfig2.frame                 = frameType;
            canMsgObjConfig2.control.messageLen    = msgDlc;
            canMsgObjConfig2.control.extendedFrame = extendedFrame;
            canMsgObjConfig2.acceptanceMask        = 0x00000000;
            canMsgObjConfig2.control.matchingId    = TRUE;

            if(frameType==IfxMultican_Frame_receive)
            {
                canMsgObjConfig2.acceptanceMask        = 0x7FFFFFFF;
                canMsgObjConfig2.control.matchingId    = TRUE;
                canMsgObjConfig2.rxInterrupt.enabled=TRUE;
                canMsgObjConfig2.rxInterrupt.srcId=1;
                SRC_CAN_CAN0_INT1.B.SRPN =31u;
                SRC_CAN_CAN0_INT1.B.TOS =0u;
                SRC_CAN_CAN0_INT1.B.SRE =1u;

            }
            /* initialize message object */
            IfxMultican_Can_MsgObj_init(pArrObjNum, &canMsgObjConfig2);
        }

        break;
    }

}


int Can_RxQueue_Push(Channel ch, const CanMessage *message)
{
    switch(ch)
    {
        case ECU:
        {
            uint32 nextHead = (canRxQueueCh0.head + 1) % CAN_QUEUE_SIZE;
            if (nextHead == canRxQueueCh0.tail)
            {
                return 0;
            }

            canRxQueueCh0.messages[canRxQueueCh0.head] = *message;
            canRxQueueCh0.head = nextHead;

            return 1;
        }
        break;

        case TOFSENSOR:
        {
            uint32 nextHead = (canRxQueueCh1.head + 1) % CAN_QUEUE_SIZE;
            if (nextHead == canRxQueueCh1.tail)
            {
                return 0;
            }

            canRxQueueCh1.messages[canRxQueueCh1.head] = *message;
            canRxQueueCh1.head = nextHead;

            return 1;
        }
        break;
    }

}


int Can_RxQueue_Pop(Channel ch,CanMessage *message)
{
    switch(ch)
    {
        case ECU :
        {
            if (canRxQueueCh0.head == canRxQueueCh0.tail)
            {
                return 0;
            }

            *message = canRxQueueCh0.messages[canRxQueueCh0.tail];
            canRxQueueCh0.tail = (canRxQueueCh0.tail + 1) % CAN_QUEUE_SIZE;

            return 1;
        }
        break;

        case TOFSENSOR :
        {
            if (canRxQueueCh1.head == canRxQueueCh1.tail)
            {
                return 0;
            }

            *message = canRxQueueCh1.messages[canRxQueueCh1.tail];
            canRxQueueCh1.tail = (canRxQueueCh1.tail + 1) % CAN_QUEUE_SIZE;

            return 1;
        }
        break;

    }
}

int Can_TxQueue_Push(Channel ch,const CanMessage *message)
{
    switch(ch)
    {
        case ECU :
        {
            uint32 nextHead = (canTxQueueCh0.head + 1) % CAN_QUEUE_SIZE;
            if (nextHead == canTxQueueCh0.tail)
            {
                return 0;
            }

            canTxQueueCh0.messages[canTxQueueCh0.head] = *message;
            canTxQueueCh0.head = nextHead;

            return 1;
        }
        break;

        case TOFSENSOR :
        {
            uint32 nextHead2 = (canTxQueueCh1.head + 1) % CAN_QUEUE_SIZE;
            if (nextHead2 == canTxQueueCh1.tail)
            {
                return 0;
            }

            canTxQueueCh1.messages[canTxQueueCh1.head] = *message;
            canTxQueueCh1.head = nextHead2;

            return 1;
        }
        break;

    }

}


int Can_TxQueue_Pop(Channel ch,CanMessage *message)
{
    switch(ch)
    {
        case ECU :
        {
            if (canTxQueueCh0.head == canTxQueueCh0.tail)
            {
                return 0;
            }

            *message = canTxQueueCh0.messages[canTxQueueCh0.tail];
            canTxQueueCh0.tail = (canTxQueueCh0.tail + 1) % CAN_QUEUE_SIZE;

            return 1;
        }
        break;

        case TOFSENSOR :
        {
            if (canTxQueueCh1.head == canTxQueueCh1.tail)
            {
                return 0;
            }

            *message = canTxQueueCh1.messages[canTxQueueCh1.tail];
            canTxQueueCh1.tail = (canTxQueueCh1.tail + 1) % CAN_QUEUE_SIZE;

            return 1;
        }
        break;
    }
}

CanMessage Can_Get_Message(Channel ch)
{
    CanMessage message;
    if(Can_RxQueue_Pop(ch,&message)) return message;
    else {
        message.id = 0xfff;
        return message; // no can Message
    }
}
