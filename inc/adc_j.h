//**************************************************
// Файл                     : adc_j.h
//**************************************************

#ifndef ADC_J_H
#define ADC_J_H
#include <stdint.h>
#include <stdbool.h>

#define VALUE_MIN 1000
#define VALUE_MAX 2000
#pragma pack(1)//выравниваем по 1 байту
typedef struct
{
    uint16_t channels[16];
    bool digital_channel_1;
    bool digital_channel_2;
    bool frame_lost;
    bool failsafe;
} sbus_msg_t;
#pragma pack()
void fnDMA(void);
void fnADC(void);
void fnSaveY(void);
void fnAdcInit(void);
void fnProcessAdc(void);
extern volatile uint16_t ADCConvertedValue[24];
// сюда приходят данные от потока кнопок, код кнопки и состояние - нажата или отжата
typedef union
{
    uint16_t arr[2];
    uint32_t i;
} un_t;
extern un_t un;
#endif //ADC_J





















