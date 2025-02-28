#include "can.h"
#include <IfxPort.h>
#include <IfxPort_Pinmap.h>
/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
McmcanType  g_mcmcan;                       /* Global MCMCAN configuration and control structure    */
CanQueue canRxQueue;
CanQueue canTxQueue;
uint8 can_rx_flag = 0;
/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/
/* Default CAN Tx Handler */
IFX_INTERRUPT(CanTxIsrHandler, 0, ISR_PRIORITY_CAN_TX);
void CanTxIsrHandler(void)
{
    /* Clear the "Transmission Completed" interrupt flag */
    IfxCan_Node_clearInterruptFlag(g_mcmcan.canSrcNode.node, IfxCan_Interrupt_transmissionCompleted);
}



/* Default CAN Rx Handler */
IFX_INTERRUPT(CanRxIsrHandler, 0, ISR_PRIORITY_CAN_RX);
void CanRxIsrHandler(void)
{
   //IfxPort_togglePin(IfxPort_P00_5.port,IfxPort_P00_5.pinIndex);
   unsigned int rxID;
   unsigned char rxData[8] = {0,};
   int rxLen;

   Can_RecvMsg(&rxID, rxData, &rxLen);
   CanMessage RxMessageTemp;
   RxMessageTemp.id = rxID;
   RxMessageTemp.dlc = rxLen;
   RxMessageTemp.dataLow = (rxData[0] << 0) | (rxData[1] << 8) | (rxData[2] << 16) | (rxData[3] << 24);
   RxMessageTemp.dataHigh = (rxData[4] << 0) | (rxData[5] << 8) | (rxData[6] << 16) | (rxData[7] << 24);
   CanRxQueue_Push(&RxMessageTemp);

}

uint8 is_rx_flag(){
    return can_rx_flag;
}

void reset_rx_flag(){
    can_rx_flag = 0;
}


/* Function to initialize MCMCAN module and nodes related for this application use case */
void Can_Init(CAN_BAUDRATES ls_baudrate)
{
    /* wake up transceiver (node 0) */
    IfxPort_setPinModeOutput(&MODULE_P20,6, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    MODULE_P20.OUT.B.P6 = 0;

    IfxCan_Can_initModuleConfig(&g_mcmcan.canConfig, &MODULE_CAN0);
    IfxCan_Can_initModule(&g_mcmcan.canModule, &g_mcmcan.canConfig);
    IfxCan_Can_initNodeConfig(&g_mcmcan.canNodeConfig, &g_mcmcan.canModule);

    switch (ls_baudrate)
    {
        case BD_NOUSE:
            g_mcmcan.canNodeConfig.busLoopbackEnabled = TRUE;
            break;
        case BD_500K:
            g_mcmcan.canNodeConfig.baudRate.baudrate = 500000;
            break;
        case BD_1M:
            g_mcmcan.canNodeConfig.baudRate.baudrate = 1000000;
            break;
    }

//#define TX_PIN IfxCan_TXD02_P15_0_OUT /* TX Pin for mikrobus (can node 2) */
//#define RX_PIN IfxCan_RXD02A_P15_1_IN /* RX Pin for mikrobus (can node 2) */
#define TX_PIN IfxCan_TXD00_P20_8_OUT /* TX Pin for lite kit (can node 0) */
#define RX_PIN IfxCan_RXD00B_P20_7_IN /* RX Pin for lite kit (can node 0) */

    g_mcmcan.canNodeConfig.busLoopbackEnabled = FALSE;
//    g_mcmcan.canNodeConfig.nodeId = IfxCan_NodeId_2; /* node config for mikrobus */
    g_mcmcan.canNodeConfig.nodeId = IfxCan_NodeId_0; /* node config for lite kit */
    const IfxCan_Can_Pins pins =
    {
            &TX_PIN, IfxPort_OutputMode_pushPull,
            &RX_PIN, IfxPort_InputMode_pullUp,
            IfxPort_PadDriver_cmosAutomotiveSpeed1
    };
    g_mcmcan.canNodeConfig.pins = &pins;
    g_mcmcan.canNodeConfig.frame.type = IfxCan_FrameType_transmitAndReceive;
    g_mcmcan.canNodeConfig.interruptConfig.transmissionCompletedEnabled = TRUE;
    g_mcmcan.canNodeConfig.interruptConfig.traco.priority = ISR_PRIORITY_CAN_TX;
    g_mcmcan.canNodeConfig.interruptConfig.traco.interruptLine = IfxCan_InterruptLine_0;
    g_mcmcan.canNodeConfig.interruptConfig.traco.typeOfService = IfxSrc_Tos_cpu0;
    IfxCan_Can_initNode(&g_mcmcan.canSrcNode, &g_mcmcan.canNodeConfig);

    /* Reception handling configuration */
    g_mcmcan.canNodeConfig.rxConfig.rxMode = IfxCan_RxMode_sharedFifo0;
    g_mcmcan.canNodeConfig.rxConfig.rxBufferDataFieldSize = IfxCan_DataFieldSize_8;
    g_mcmcan.canNodeConfig.rxConfig.rxFifo0DataFieldSize = IfxCan_DataFieldSize_8;
    g_mcmcan.canNodeConfig.rxConfig.rxFifo0Size = 15;
    /* General filter configuration */
    g_mcmcan.canNodeConfig.filterConfig.messageIdLength = IfxCan_MessageIdLength_standard;
    g_mcmcan.canNodeConfig.filterConfig.standardListSize = 8;
    g_mcmcan.canNodeConfig.filterConfig.standardFilterForNonMatchingFrames = IfxCan_NonMatchingFrame_reject;
    g_mcmcan.canNodeConfig.filterConfig.rejectRemoteFramesWithStandardId = TRUE;
    /* Interrupt configuration */
    g_mcmcan.canNodeConfig.interruptConfig.rxFifo0NewMessageEnabled = TRUE;
    g_mcmcan.canNodeConfig.interruptConfig.rxf0n.priority = ISR_PRIORITY_CAN_RX;
    g_mcmcan.canNodeConfig.interruptConfig.rxf0n.interruptLine = IfxCan_InterruptLine_1;
    g_mcmcan.canNodeConfig.interruptConfig.rxf0n.typeOfService = IfxSrc_Tos_cpu0;
    IfxCan_Can_initNode(&g_mcmcan.canDstNode, &g_mcmcan.canNodeConfig);

    /* Rx filter configuration (default: all messages accepted) */
    Can_SetFilterRange(0x0, 0x7FF);
}

void Can_SetFilterRange(uint32 start, uint32 end)
{
    g_mcmcan.canFilter.number = 0;
    g_mcmcan.canFilter.type = IfxCan_FilterType_range;
    g_mcmcan.canFilter.elementConfiguration = IfxCan_FilterElementConfiguration_storeInRxFifo0;
    g_mcmcan.canFilter.id1 = start;
    g_mcmcan.canFilter.id2 = end;
    IfxCan_Can_setStandardFilter(&g_mcmcan.canDstNode, &g_mcmcan.canFilter);
}

void Can_SetFilterMask(uint32 id, uint32 mask)
{
    g_mcmcan.canFilter.number = 0;
    g_mcmcan.canFilter.type = IfxCan_FilterType_classic;
    g_mcmcan.canFilter.elementConfiguration = IfxCan_FilterElementConfiguration_storeInRxFifo0;
    g_mcmcan.canFilter.id1 = id;
    g_mcmcan.canFilter.id2 = mask;
    IfxCan_Can_setStandardFilter(&g_mcmcan.canDstNode, &g_mcmcan.canFilter);
}

void Can_SendMsg(unsigned int id, const unsigned char *txData, int len)
{
    /* Initialization of the TX message with the default configuration */
    IfxCan_Can_initMessage(&g_mcmcan.txMsg);

    g_mcmcan.txMsg.messageId = id;
    g_mcmcan.txMsg.dataLengthCode = len;

    /* Define the content of the data to be transmitted */
    for (int i = 0; i < 8; i++) {
        g_mcmcan.txData[i] = txData[i];
    }

    /* Send the CAN message with the previously defined TX message content */
    while( IfxCan_Status_notSentBusy ==
           IfxCan_Can_sendMessage(&g_mcmcan.canSrcNode, &g_mcmcan.txMsg, (uint32 *)&g_mcmcan.txData[0]) )
    {
    }
}

int Can_RecvMsg(unsigned int *id, unsigned char *rxData, int *len)
{
    int err = 0;
    /* Clear the "RX FIFO 0 new message" interrupt flag */
    IfxCan_Node_clearInterruptFlag(g_mcmcan.canDstNode.node, IfxCan_Interrupt_rxFifo0NewMessage);

    /* Received message content should be updated with the data stored in the RX FIFO 0 */
    g_mcmcan.rxMsg.readFromRxFifo0 = TRUE;
    g_mcmcan.rxMsg.readFromRxFifo1 = FALSE;

    /* Read the received CAN message */
    IfxCan_Can_readMessage(&g_mcmcan.canDstNode,
                           &g_mcmcan.rxMsg,
                           (uint32*) &g_mcmcan.rxData);

    *id = g_mcmcan.rxMsg.messageId;
    for (int i = 0; i < 8; i++) {
        rxData[i] = g_mcmcan.rxData[i];
    }
    *len = g_mcmcan.rxMsg.dataLengthCode;

    return err;
}


void CanMessage_Init(CanMessage *message,CanMessageId msg, uint32 id, uint8 dlc)
{
    message->msg= msg;
    message->id = id;
    message->dlc = dlc;
    message->dataLow = 0;
    message->dataHigh = 0;
}

void CanMessage_AddSignal(CanMessage *message, uint8 startBit, uint8 length, uint32 value)
{
    uint32 mask = (1<<length) -1;
    uint32 encodedValue = value & mask;
    if (startBit < 32)
    {
        message->dataLow |= (encodedValue << startBit);
    }

    else
    {
        uint8 shiftedStartBit = startBit - 32;
        message->dataHigh |= (encodedValue << shiftedStartBit);
    }
}


void Convert_CanMessage_To_ByteArray( CanMessage *message, unsigned char *txData, int len)
{
    for (int i = 0; i < len; i++)
    {
        message->data[i]=0;
    }

    for (int i = 0; i < 4 && i < len; i++)
    {
        message->data[i] = (message->dataLow >> (i * 8)) & 0xFF;
    }

    for (int i = 0; i < 4 && (i + 4) < len; i++)
    {
        message->data[i + 4] = (message->dataHigh >> (i * 8)) & 0xFF;
    }
}



uint32 CanMessage_Parse(uint32 dataLow, uint32 dataHigh, uint8 startBit, uint8 length)
{
    uint64 combinedData = ((uint64)dataHigh << 32) | dataLow;
    uint64 mask = (1ULL << length) - 1;
    return (combinedData >> startBit) & mask;
}





int CanRxQueue_Push(const CanMessage *message)
{
    uint32 nextHead = (canRxQueue.head + 1) % CAN_RX_QUEUE_SIZE;
    if (nextHead == canRxQueue.tail)
    {
        return 0;
    }

    canRxQueue.messages[canRxQueue.head] = *message;
    canRxQueue.head = nextHead;

    return 1;
}


int CanRxQueue_Pop(CanMessage *message)
{
    if (canRxQueue.head == canRxQueue.tail)
    {
        return 0;
    }

    *message = canRxQueue.messages[canRxQueue.tail];
    canRxQueue.tail = (canRxQueue.tail + 1) % CAN_RX_QUEUE_SIZE;

    return 1;
}

int CanTxQueue_Push(const CanMessage *message)
{
    uint32 nextHead = (canTxQueue.head + 1) % CAN_TX_QUEUE_SIZE;
    if (nextHead == canTxQueue.tail)
    {
        return 0;
    }

    canTxQueue.messages[canTxQueue.head] = *message;
    canTxQueue.head = nextHead;

    return 1;
}


int CanTxQueue_Pop(CanMessage *message)
{
    if (canTxQueue.head == canTxQueue.tail)
    {
        return 0;
    }

    *message = canTxQueue.messages[canTxQueue.tail];
    canTxQueue.tail = (canTxQueue.tail + 1) % CAN_TX_QUEUE_SIZE;

    return 1;
}
