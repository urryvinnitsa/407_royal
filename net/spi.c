#include "main.h"
#include "wizchip_conf.h"
#include "spi.h"

void W5500_CS_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    // SPI-CS (PB6)
    GPIO_InitStruct.GPIO_Pin = SPI1_CS_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(SPI1_CS_PORT, &GPIO_InitStruct);
    GPIO_SetBits(SPI1_CS_PORT, SPI1_CS_PIN);
}

void W5500_RST_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    // SPI-RES (PB7)
    GPIO_InitStruct.GPIO_Pin = SPI1_RESET_PIN ;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(SPI1_RESET_PORT, &GPIO_InitStruct);
    GPIO_SetBits(SPI1_RESET_PORT, SPI1_RESET_PIN);
}
//--------------------------------------------------
void SPI1_WriteByte(uint8_t TxData)
{
    while (SPI_I2S_GetFlagStatus(SPI1,  SPI_I2S_FLAG_TXE) == 0);
    SPI1->DR = TxData;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == 0);
    SPI1->DR;
}
//--------------------------------------------------
uint8_t SPI1_ReadByte(void)
{
    while ((SPI1->SR & SPI_I2S_FLAG_TXE) == 0); //
    SPI1->DR = 0xFF;
    while ((SPI1->SR & SPI_I2S_FLAG_RXNE) == 0); //
    return SPI1->DR;
}
//--------------------------------------------------
/**
  * @brief
  * @retval None
  */
void SPI_CrisEnter(void)
{
    __set_PRIMASK(1);
}
/**
  * @brief
  * @retval None
  */
void SPI_CrisExit(void)
{
    __set_PRIMASK(0);
}

/**
  * @brief
  * @retval None
  */
void SPI_CS_Select(void)
{
    GPIO_ResetBits(SPI1_CS_PORT, SPI1_CS_PIN);
}
/**
  * @brief
  * @retval None
  */
void SPI_CS_Deselect(void)
{
    GPIO_SetBits(SPI1_CS_PORT, SPI1_CS_PIN);
}
//-------------------------------------------
void register_wizchip()
{
    // First of all, Should register SPI callback functions implemented by user for accessing WIZCHIP
    /* Critical section callback */
    reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);
    /* Chip selection call back */
    reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);
    /* SPI Read & Write callback function */
    reg_wizchip_spi_cbfunc(SPI1_ReadByte, SPI1_WriteByte);
}
//-------------------------------------------
void SPI1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    //PB3, PB4, PB5
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, ENABLE); // reset SPI1
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, DISABLE);// stop reset SPI1
    SPI_InitTypeDef SPI_InitStructure;
    SPI_StructInit(&SPI_InitStructure); // set default settings
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;// SPI_BaudRatePrescaler_8;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; // data sampled at first edge
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;   // clock is low when idle
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; // one packet of data is 8 bits wide
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // set to full duplex mode, seperate MOSI and MISO lines
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; // data is transmitted MSB first
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master; // transmit in master mode, NSS pin has to be always high
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; // set the NSS management to internal and pull internal NSS high
    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_Cmd(SPI1, ENABLE);
}
//-------------------------------------------

























