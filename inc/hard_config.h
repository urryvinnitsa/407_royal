//**************************************************
//! Файл                                    : hard_config.h
//**************************************************
#ifndef HARD_CONFIG_H
#define HARD_CONFIG_H

#define BIT_SET(var,bitno)  ((var) |= 1 << (bitno))
#define BIT_CLR(var,bitno)  ((var) &= ~(1 << (bitno)))
#define BIT_TST1(var,bitno) ((var) & (1 << (bitno)))



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

#endif // HARD_CONFIG_H

