//**************************************************
//! Файл                                    : buttons.h
//**************************************************
#ifndef BUTTONS_H
#define BUTTONS_H
#include "stm32f4xx.h"

extern struct process button_process;
#define BUTTON_MAX 16
typedef struct
{
    GPIO_TypeDef *Port_B;
    uint16_t Pin_B;
} buttons_t;
extern const buttons_t buttons[BUTTON_MAX];

#endif //BUTTONS_H

