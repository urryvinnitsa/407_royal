//**************************************************
// file                     : link_rf.c
//**************************************************
#include "main.h"
// uart1 - link rf
//
//--------------------------------------------------
void fnDMAInit(void);
#define ARRAY_LEN(x)            (sizeof(x) / sizeof((x)[0]))
uint8_t usart_rx_dma_buffer[512];
uint8_t usart_tx_dma_buffer[512];
void usart_rx_check(void);
//--------------------------------------------------
void fnProcessLink(void)
{
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        USART_ClearFlag(USART1, USART_IT_IDLE);
        usart_rx_check();
    }
    if (send_free.u1 == 0)// дма отработал и свободен
    {
        if (fnGetBufer_Ser()  == BUFER_OK)
        {
            USART1SendDMA((uint8_t *)arr_ser, len_s);
        }
    }
}
//--------------------------------------------------
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        USART_ClearFlag(USART1, USART_IT_IDLE);
        usart_rx_check();
    }
}
//--------------------------------------------------
void fnDMAInit(void)
{
    USART_InitTypeDef USART_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    DMA_InitTypeDef DMA_InitStructureRX;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    //
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    USART_DeInit(USART1);
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStructure);
    USART_ClearFlag(USART1, USART_FLAG_CTS);
    USART_Cmd(USART1, ENABLE);
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    DMA_DeInit(DMA2_Stream2);
    DMA_Cmd(DMA2_Stream2, DISABLE);
    //
    DMA_StructInit(&DMA_InitStructureRX);
    DMA_InitStructureRX.DMA_PeripheralBaseAddr = (uint32_t) & (USART1->DR);
    DMA_InitStructureRX.DMA_Memory0BaseAddr = (uint32_t)(&usart_rx_dma_buffer[0]);
    DMA_InitStructureRX.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructureRX.DMA_BufferSize = ARRAY_LEN(usart_rx_dma_buffer);//512;//max_size;
    DMA_InitStructureRX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructureRX.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructureRX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructureRX.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructureRX.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructureRX.DMA_Priority = DMA_Priority_High;
    DMA_InitStructureRX.DMA_Channel = DMA_Channel_4;
    DMA_Init(DMA2_Stream2, &DMA_InitStructureRX);
    //
    DMA_ITConfig(DMA2_Stream2, DMA_IT_TC, ENABLE);
    DMA_ITConfig(DMA2_Stream2, DMA_IT_HT, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    //
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_EnableIRQ(DMA2_Stream2_IRQn);
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    DMA_Cmd(DMA2_Stream2, ENABLE); // RUN
    //
    DMA_DeInit(DMA2_Stream7);
    DMA_Cmd(DMA2_Stream7, DISABLE);
    //
    DMA_StructInit(&DMA_InitStructureRX);
    DMA_InitStructureRX.DMA_PeripheralBaseAddr = (uint32_t) & (USART1->DR);
    DMA_InitStructureRX.DMA_Memory0BaseAddr = (uint32_t)(&usart_tx_dma_buffer[0]);
    DMA_InitStructureRX.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructureRX.DMA_BufferSize = 512;//max_size;
    DMA_InitStructureRX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructureRX.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructureRX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructureRX.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructureRX.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructureRX.DMA_Priority = DMA_Priority_High;
    DMA_InitStructureRX.DMA_Channel = DMA_Channel_4;
    DMA_Init(DMA2_Stream7, &DMA_InitStructureRX);
    //
    DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    //
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    //
    NVIC_EnableIRQ(DMA2_Stream7_IRQn);
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    DMA_Cmd(DMA2_Stream7, ENABLE); // RUN
}
//--------------------------------------------------
void usart_rx_check(void)
{
    static size_t old_pos;
    size_t pos;
    /* Calculate current position in buffer and check for new data available */
    pos = ARRAY_LEN(usart_rx_dma_buffer) - DMA_GetCurrDataCounter(DMA2_Stream2);
    if (pos != old_pos)
    {
        /* Check change in received data */
        if (pos > old_pos)
        {
            /*
             * [   0   ]
             * [   1   ] <- old_pos |------------------------------------|
             * [   2   ]            |                                    |
             * [   3   ]            | Single block (len = pos - old_pos) |
             * [   4   ]            |                                    |
             * [   5   ]            |------------------------------------|
             * [   6   ] <- pos
             * [   7   ]
             * [ N - 1 ]
             */
            //   usart_process_data(&usart_rx_dma_buffer[old_pos], pos - old_pos);
            fnAddBufer_Rf((uint8_t *) &usart_rx_dma_buffer[old_pos], pos - old_pos);
        }
        else
        {
            /*
             * Processing is done in "overflow" mode..
             *
             * Application must process data twice,
             * since there are 2 linear memory blocks to handle
             *
             * [   0   ]            |---------------------------------|
             * [   1   ]            | Second block (len = pos)        |
             * [   2   ]            |---------------------------------|
             * [   3   ] <- pos
             * [   4   ] <- old_pos |---------------------------------|
             * [   5   ]            |                                 |
             * [   6   ]            | First block (len = N - old_pos) |
             * [   7   ]            |                                 |
             * [ N - 1 ]            |---------------------------------|
             */
            fnAddBufer_Rf((uint8_t *) &usart_rx_dma_buffer[old_pos], ARRAY_LEN(usart_rx_dma_buffer) - old_pos);
            if (pos > 0)
            {
                fnAddBufer_Rf((uint8_t *) &usart_rx_dma_buffer[0], pos);
            }
        }
        old_pos = pos;                          /* Save current position as old for next transfers */
    }
}
//--------------------------------------------------
void DMA2_Stream2_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA2_Stream2, DMA_IT_TCIF2))
    {
        DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_TCIF2);
        usart_rx_check();
    }
    if (DMA_GetITStatus(DMA2_Stream2, DMA_IT_HTIF2))
    {
        DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_HTIF2);
        usart_rx_check();
    }
}
//--------------------------------------------------
void DMA2_Stream7_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA2_Stream7, DMA_IT_TCIF7))
    {
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);
        DMA_Cmd(DMA1_Stream7, DISABLE);
        send_free.u1 = 0;
    }
}
//--------------------------------------------------
void USART1SendDMA(uint8_t *arr, uint16_t len)
{
    /* Restart DMA Channel*/
    DMA_Cmd(DMA2_Stream7, DISABLE);
    DMA2_Stream7->NDTR = len;
    DMA2_Stream7->M0AR = (uint32_t)&arr[0];
    DMA_Cmd(DMA2_Stream7, ENABLE);
    send_free.u1 = 1;
}
//--------------------------------------------------




