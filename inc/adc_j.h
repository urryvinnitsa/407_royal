//**************************************************
// Файл                     : adc_j.h
//**************************************************

#ifndef ADC_J_H
#define ADC_J_H
#include <stdint.h>
#include <stdbool.h>
extern struct process task_adc_process;
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
extern volatile uint16_t ADCConvertedValue[24];
#endif //ADC_J





















