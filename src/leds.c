
//**************************************************
//! Файл                               : leds.c
//**************************************************
#include "main.h"
static void fnLedInit(void);
static void fnSetLed(void);
//---------------------------------
static struct
{
    bool blink;
    uint16_t cnt;
} str_led;
LED_MODE led_arr[LED_MAX];
//---------------------------------
const leds_t leds[LED_MAX] = {\
                              [0].Port_L = L0_PORT, [0].Pin_L = L0_PIN, \
                              [1].Port_L = L1_PORT, [1].Pin_L = L1_PIN, \
                              [2].Port_L = L2_PORT, [2].Pin_L = L2_PIN, \
                              [3].Port_L = L3_PORT, [3].Pin_L = L3_PIN, \
                              [4].Port_L = L4_PORT, [4].Pin_L = L4_PIN, \
                              [5].Port_L = L5_PORT, [5].Pin_L = L5_PIN, \
                              [6].Port_L = L6_PORT, [6].Pin_L = L6_PIN, \
                              [7].Port_L = L7_PORT, [7].Pin_L = L7_PIN, \
                              [8].Port_L = L8_PORT, [8].Pin_L = L8_PIN, \
                              [9].Port_L = L9_PORT, [9].Pin_L = L9_PIN, \
                              [10].Port_L = L10_PORT, [10].Pin_L = L10_PIN, \
                              [11].Port_L = L11_PORT, [11].Pin_L = L11_PIN, \
                              [12].Port_L = L12_PORT, [12].Pin_L = L12_PIN, \
                              [13].Port_L = L13_PORT, [13].Pin_L = L13_PIN, \
                              [14].Port_L = L14_PORT, [14].Pin_L = L14_PIN, \
                              [15].Port_L = L15_PORT, [15].Pin_L = L15_PIN, \
                             };

PROCESS(leds_process, "LED");
//---------------------------------------------------
PROCESS_THREAD(leds_process, ev, data)
{
    PROCESS_BEGIN();
    fnLedInit();
    while (1)
    {
        static struct etimer et;
        etimer_set(&et, 10);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        str_led.cnt++;
        if (str_led.cnt >= 30)
        {
            str_led.cnt = 0;
            str_led.blink ^= 1;
        }
        fnSetLed();
    }
    PROCESS_END();
}
//----------------------------------------------------
static void fnLedInit(void)
{
    uint8_t i;
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    for (i = 0; i < LED_MAX; i++)
    {
        GPIO_InitStruct.GPIO_Pin = leds[i].Pin_L ;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(leds[i].Port_L, &GPIO_InitStruct);
        GPIO_ResetBits(leds[i].Port_L, leds[i].Pin_L);
    }
}
//----------------------------------------------------
static void fnSetLed(void)
{
    uint8_t i;
    LED_MODE mode;
    for (i = 0; i < LED_MAX; i++)
    {
        mode = led_arr[i];
        switch (mode)
        {
        case LED_OFF:
            GPIO_ResetBits(leds[i].Port_L, leds[i].Pin_L);
            break;
        case LED_ON:
            GPIO_SetBits(leds[i].Port_L, leds[i].Pin_L);
            break;
        case LED_BLINK:
            if (str_led.blink)
            {
                GPIO_SetBits(leds[i].Port_L, leds[i].Pin_L);
            }
            else
            {
                GPIO_ResetBits(leds[i].Port_L, leds[i].Pin_L);
            }
            break;
        }
    }
}
//----------------------------------------------------




