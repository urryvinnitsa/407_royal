//**************************************************
//! Файл                                    : hard_config.h
//**************************************************
#ifndef HARD_CONFIG_H
#define HARD_CONFIG_H

#define BIT_SET(var,bitno)  ((var) |= 1 << (bitno))
#define BIT_CLR(var,bitno)  ((var) &= ~(1 << (bitno)))
#define BIT_TST1(var,bitno) ((var) & (1 << (bitno)))


// кнопки
#define B0_PORT GPIOC
#define B0_PIN  GPIO_Pin_3
#define B1_PORT GPIOC
#define B1_PIN  GPIO_Pin_2
#define B2_PORT GPIOC
#define B2_PIN  GPIO_Pin_1
#define B3_PORT GPIOC
#define B3_PIN  GPIO_Pin_0
#define B4_PORT GPIOF
#define B4_PIN  GPIO_Pin_9
#define B5_PORT GPIOF
#define B5_PIN  GPIO_Pin_10
#define B6_PORT GPIOF
#define B6_PIN  GPIO_Pin_7
#define B7_PORT GPIOF
#define B7_PIN  GPIO_Pin_8
#define B8_PORT GPIOB
#define B8_PIN  GPIO_Pin_2
#define B9_PORT GPIOF
#define B9_PIN  GPIO_Pin_11
#define B10_PORT GPIOF
#define B10_PIN  GPIO_Pin_12
#define B11_PORT GPIOF
#define B11_PIN  GPIO_Pin_15
#define B12_PORT GPIOG
#define B12_PIN  GPIO_Pin_1
#define B13_PORT GPIOG
#define B13_PIN  GPIO_Pin_0
#define B14_PORT GPIOE
#define B14_PIN  GPIO_Pin_8
#define B15_PORT GPIOE
#define B15_PIN  GPIO_Pin_7
// светодиоды
#define L0_PORT GPIOG
#define L0_PIN  GPIO_Pin_3
#define L1_PORT GPIOG
#define L1_PIN  GPIO_Pin_2
#define L2_PORT GPIOD
#define L2_PIN  GPIO_Pin_15
#define L3_PORT GPIOD
#define L3_PIN  GPIO_Pin_14
#define L4_PORT GPIOD
#define L4_PIN  GPIO_Pin_11
#define L5_PORT GPIOD
#define L5_PIN  GPIO_Pin_10
#define L6_PORT GPIOD
#define L6_PIN  GPIO_Pin_13
#define L7_PORT GPIOD
#define L7_PIN  GPIO_Pin_12
#define L8_PORT GPIOD
#define L8_PIN  GPIO_Pin_9
#define L9_PORT GPIOD
#define L9_PIN  GPIO_Pin_8
#define L10_PORT GPIOB
#define L10_PIN  GPIO_Pin_15
#define L11_PORT GPIOB
#define L11_PIN  GPIO_Pin_12
#define L12_PORT GPIOB
#define L12_PIN  GPIO_Pin_13
#define L13_PORT GPIOG
#define L13_PIN  GPIO_Pin_4
#define L14_PORT GPIOE
#define L14_PIN  GPIO_Pin_15
#define L15_PORT GPIOE
#define L15_PIN  GPIO_Pin_13
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

