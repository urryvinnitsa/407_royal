
//**************************************************
//! Файл                                    : leds.h
//**************************************************
#ifndef __LEDS_H
#define __LEDS_H
#include "stm32f4xx.h"

#define LED_MAX 16
typedef struct
{
    GPIO_TypeDef *Port_L;
    uint16_t Pin_L;
} leds_t;
extern const leds_t leds[LED_MAX];

typedef enum {LED_OFF = 1, LED_ON = 0, LED_BLINK = 2} LED_MODE;
extern LED_MODE led_arr[LED_MAX];
void fnLedsInit(void);
void fnProcessLeds(void);


#endif //__LEDS_H
