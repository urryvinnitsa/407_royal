//**************************************************
//! Файл                                    : hard_config.h
//**************************************************
#ifndef HARD_CONFIG_H
#define HARD_CONFIG_H

#define BIT_SET(var,bitno)  ((var) |= 1 << (bitno))
#define BIT_CLR(var,bitno)  ((var) &= ~(1 << (bitno)))
#define BIT_TST1(var,bitno) ((var) & (1 << (bitno)))


// кнопки
#define B0_PORT GPIOB
#define B0_PIN  GPIO_Pin_0
#define B1_PORT GPIOB
#define B1_PIN  GPIO_Pin_0
#define B2_PORT GPIOB
#define B2_PIN  GPIO_Pin_0
#define B3_PORT GPIOB
#define B3_PIN  GPIO_Pin_0
#define B4_PORT GPIOB
#define B4_PIN  GPIO_Pin_0
#define B5_PORT GPIOB
#define B5_PIN  GPIO_Pin_0
#define B6_PORT GPIOB
#define B6_PIN  GPIO_Pin_0
#define B7_PORT GPIOB
#define B7_PIN  GPIO_Pin_0
#define B8_PORT GPIOB
#define B8_PIN  GPIO_Pin_0
#define B9_PORT GPIOB
#define B9_PIN  GPIO_Pin_0
#define B10_PORT GPIOB
#define B10_PIN  GPIO_Pin_0
#define B11_PORT GPIOB
#define B11_PIN  GPIO_Pin_0
#define B12_PORT GPIOB
#define B12_PIN  GPIO_Pin_0
#define B13_PORT GPIOB
#define B13_PIN  GPIO_Pin_0
#define B14_PORT GPIOB
#define B14_PIN  GPIO_Pin_0
#define B15_PORT GPIOB
#define B15_PIN  GPIO_Pin_0
// светодиоды
#define L0_PORT GPIOB
#define L0_PIN  GPIO_Pin_0
#define L1_PORT GPIOB
#define L1_PIN  GPIO_Pin_0
#define L2_PORT GPIOB
#define L2_PIN  GPIO_Pin_0
#define L3_PORT GPIOB
#define L3_PIN  GPIO_Pin_0
#define L4_PORT GPIOB
#define L4_PIN  GPIO_Pin_0
#define L5_PORT GPIOB
#define L5_PIN  GPIO_Pin_0
#define L6_PORT GPIOB
#define L6_PIN  GPIO_Pin_0
#define L7_PORT GPIOB
#define L7_PIN  GPIO_Pin_0
#define L8_PORT GPIOB
#define L8_PIN  GPIO_Pin_0
#define L9_PORT GPIOB
#define L9_PIN  GPIO_Pin_0
#define L10_PORT GPIOB
#define L10_PIN  GPIO_Pin_0
#define L11_PORT GPIOB
#define L11_PIN  GPIO_Pin_0
#define L12_PORT GPIOB
#define L12_PIN  GPIO_Pin_0
#define L13_PORT GPIOB
#define L13_PIN  GPIO_Pin_0
#define L14_PORT GPIOB
#define L14_PIN  GPIO_Pin_0
#define L15_PORT GPIOB
#define L15_PIN  GPIO_Pin_0
//
// для w5500 -----------------------
// spi1 sck B3
// spi1 miso B4
// spi1 mosi B5
#define SPI1_CS_PIN  GPIO_Pin_6
#define SPI1_CS_PORT  GPIOB
#define SPI1_RESET_PIN  GPIO_Pin_7
#define SPI1_RESET_PORT  GPIOB
//----------------------------------
#endif // HARD_CONFIG_H

