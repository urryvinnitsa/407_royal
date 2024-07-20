//**************************************************
// Файл                     : adc_j.c
//**************************************************
#include "main.h"
#include <math.h>
#include <stdlib.h>

// это для 411, для 407 уточнить в скаттер файле !!!
#define ADR_MIN     0x080E0000
#define ADR_MAX     0x080E0040
#define ADR_MIDL    0x080E0080

FLASH_Status st;
uint16_t size_r[6][2];
volatile sbus_msg_t sbus_msg;
mavlink_message_t message;
uint8_t buffer_sbus[256];
uint8_t arr_s[256];
void fnClearLed5(void);
void fnSendBinUart1(const uint8_t *s, int16_t len);
int len;
volatile uint16_t ADCConvertedValue[24];//384
__IO uint32_t ADC1ConvertedVoltage[6];
typedef enum {S_WORK = 0, S_CAL} MODE_DATA;
typedef enum {GET_MIN = 0, GET_MAX, GET_MIDDL} GET_DATA;
static GET_DATA get_data = GET_MIN;
static MODE_DATA mode_data = S_WORK;
// сюда приходят данные от потока кнопок, код кнопки и состояние - нажата или отжата
static union
{
    uint16_t arr[2];
    uint32_t i;
} un;
//-----------------------------------------------------------------------------------
static uint16_t iCntCl = 0;
static uint16_t min_val[6];
static uint16_t max_val[6];
static uint16_t midl_val[6];
//
static uint16_t min_val_r[6];
static uint16_t max_val_r[6];
static uint16_t midl_val_r[6];
uint16_t len_real;
static uint8_t cnt_err = 0; //  счетчик ошибок при передаче юдп пакета,
// рестартовать все, нет смысла возиться только с 1 потоком
//------------------------------------
void fnInitSize(void);
void fnNormalize(void);
uint16_t fnCalcValue(uint8_t index);
void fnProcessAdc(void); // вызывается с периодичностью 1 мс
void fnDMA(void);
void fnADC(void);
void fnCalcButtons(void);
void fnCalibr(void);
void fnSetMin(void);
void fnSetMax(void);
void fnSetMiddl(void);
void fnGetMinVal(void);
void fnGetMaxVal(void);
void fnGetMiddlVal(void);
void fnSaveY(void);
void fnReadY(void);
uint16_t fnUsred(uint8_t index);
//--------------------------------------------------
#pragma inline
void delay_us(uint16_t delay)
{
    uint16_t summa;
    summa = delay * 16;
    do
    {
        __NOP();
    }
    while (--summa);
}
//--------------------------------------------------
void delay_ms(uint16_t delay)
{
    do
    {
        delay_us(1000);
    }
    while (--delay);
}
//--------------------------------------------------

PROCESS(task_adc_process, "T_adc");
PROCESS_THREAD(task_adc_process, ev, data)
{
    PROCESS_BEGIN();
    for (int i = 0; i < LED_MAX; i++)
    {
        led_arr[i] = LED_OFF;
    }
    //    led_arr[12] = LED_ON;
    //    fnReadY();
    //    fnInitSize();
    fnDMA();
    fnADC();
    for (int i = 0; i < 16; i++)
    {
        sbus_msg.channels[i] = 1500;
    }
    sbus_msg.channels[4] = 1000;
    sbus_msg.channels[5] = 1000;
    while (1)
    {
        PROCESS_WAIT_EVENT_UNTIL(ev == event_1ms || ev == event_button);
        un.i = (*(uint32_t *)data);
        if (ev == event_1ms)
        {
            if (mode_data == S_WORK)
            {
                fnProcessAdc();
            }
            else if (mode_data == S_CAL)
            {
                fnCalibr();
            }
        }
        if (ev == event_button)
        {
            fnCalcButtons();
        }
    }
    PROCESS_END();
}
//--------------------------------------------------
void fnProcessAdc(void) // вызывается с периодичностью 1 мс
{
    static uint8_t iCnt = 0;
    iCnt++;
    if (iCnt >= 40)
    {
        iCnt = 0;
        fnNormalize();
        sbus_msg.channels[0] = ADC1ConvertedVoltage[0];
        sbus_msg.channels[1] = 3000 - ADC1ConvertedVoltage[1];
        sbus_msg.channels[2] = 3000 - ADC1ConvertedVoltage[2];
        sbus_msg.channels[3] = ADC1ConvertedVoltage[3];
        sbus_msg.channels[8] = ADC1ConvertedVoltage[4];
        sbus_msg.channels[9] = 3000 - ADC1ConvertedVoltage[5];
        mavlink_msg_rc_channels_override_pack(0xff, 158, &message, 1, 1,
                                              sbus_msg.channels[0], sbus_msg.channels[1], sbus_msg.channels[2], sbus_msg.channels[3],
                                              sbus_msg.channels[4], sbus_msg.channels[5], sbus_msg.channels[6],
                                              sbus_msg.channels[7], sbus_msg.channels[8], sbus_msg.channels[9], sbus_msg.channels[10],
                                              sbus_msg.channels[11], sbus_msg.channels[12], sbus_msg.channels[13], sbus_msg.channels[14],
                                              sbus_msg.channels[15],
                                              1000, 2500);
        message.magic = 0xFD;//
        len = mavlink_msg_to_send_buffer((uint8_t *)buffer_sbus, &message);
        // отправляем пакеты в пиксу по уарту
        fnAddBufer_Ser((uint8_t *) buffer_sbus,  len);
        // отправляем пакеты на планку по юдп
        len_real =   sendto(1, (uint8_t *) buffer_sbus, len, (uint8_t *) adr_all_comp, 14555);
        if (len_real != len)
        {
            cnt_err++;
            if (cnt_err > 3)
            {
                cnt_err = 0;
                fnClearInit();
            }
        }
        else
        {
            cnt_err = 0;
        }
    }
}
//--------------------------------------------------
void fnADC(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_4 | GPIO_Pin_3 | GPIO_Pin_2 | GPIO_Pin_1 | GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//
    GPIO_Init(GPIOA, &GPIO_InitStructure);//
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);
    ADC_InitStructure.ADC_Resolution           = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode         = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode   = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 6;
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_112Cycles);// ADC_SampleTime_56Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_112Cycles);// ADC_SampleTime_56Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_112Cycles);// ADC_SampleTime_56Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_112Cycles);// ADC_SampleTime_56Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_112Cycles);// ADC_SampleTime_56Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 6, ADC_SampleTime_112Cycles);// ADC_SampleTime_56Cycles);
    /* Enable DMA request after last transfer (Single-ADC mode) */
    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
    /* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);
    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);
    ADC_SoftwareStartConv(ADC1);
}
//--------------------------------------------------
void fnDMA(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    DMA_InitStructure.DMA_Channel = DMA_Channel_0;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (ADC1->DR);
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)ADCConvertedValue;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = 24;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream0, ENABLE);
}
//--------------------------------------------------
void fnNormalize(void)
{
    for (int i = 0; i < 6; i++)
    {
        ADC1ConvertedVoltage[i] = fnCalcValue(i);
    }
}
//--------------------------------------------------
uint16_t fnCalcValue(uint8_t index)
{
    uint16_t temp;
    uint8_t curr;
    float delta; // разница между ближайшим табличным значением и текущим показанием
    // обрезаем сверху и снизу и уходим, если данные меньше или больше табличных значений
    uint32_t summa = 0;
    summa = ADCConvertedValue[index];
    summa += ADCConvertedValue[index + 6];
    summa /= 2;
    summa += ADCConvertedValue[index + 12];
    summa /= 2;
    summa += ADCConvertedValue[index + 18];
    summa /= 4;
    if (summa < (min_val_r[index]))
    {
        return VALUE_MIN;
    }
    if (summa >= (max_val_r[index]))
    {
        return VALUE_MAX;
    }
    //----------------------------------------------
    //получение ближайшего значения
    if (summa < midl_val_r[index])
    {
        curr = 0;
    }
    else
    {
        curr = 1;
    }
    //-----------------------------------------------
    if (curr == 0)
        // первая половина
    {
        delta = (float)summa - (float)(min_val_r[index]);
        float val = (float)1000.0 + (float)500.0 * ((float)delta / (float)size_r[index][0]);
        // добавляем экспоненту на 4 и 5 каналы
        if (index == 4  || index == 5)
        {
            if (abs(1500 - val) < 50)
            {
                temp = abs(1500 - val);
                temp /= 6;
                if (val > 1500)
                {
                    val = 1500 + temp;
                }
                else
                {
                    val = 1500 - temp;
                }
            }
        }
        if (val >  VALUE_MAX)
        {
            return   VALUE_MAX;
        }
        if (val <  VALUE_MIN)
        {
            return  VALUE_MIN;
        }
        return (uint32_t) val;
    }
    else if (curr == 1)
    {
        // вторая половина
        delta = (float)summa - (float)(midl_val_r[index]);
        float val = (float)1500.0 + (float)500.0 * ((float)delta / (float)size_r[index][1]);
        if (index == 4  || index == 5)
        {
            if (abs(1500 - val) < 50)
            {
                temp = abs(1500 - val);
                temp /= 6;
                if (val > 1500)
                {
                    val = 1500 + temp;
                }
                else
                {
                    val = 1500 - temp;
                }
            }
        }
        if (val >  VALUE_MAX)
        {
            return  VALUE_MAX;
        }
        if (val <  VALUE_MIN)
        {
            return  VALUE_MIN;
        }
        return (uint32_t) val;
    }
    return VALUE_MIN;
}
//--------------------------------------------------
void fnInitSize(void)
{
    for (int i = 0; i < 6; i++)
    {
        size_r[i][0] = ((midl_val_r[i] - min_val_r[i]));
        size_r[i][1] = ((max_val_r[i] - midl_val_r[i]));
    }
}
//--------------------------------------------------
void fnSendBinUart1(const uint8_t *s, int16_t len)
{
    while (len--)
    {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) {}
        USART_SendData(USART1, *s++);
    }
}
//--------------------------------------------------
void fnClearLed5(void)
{
    led_arr[0] = led_arr[4] = led_arr[8] = led_arr[12] = led_arr[13] = LED_OFF;
}
//--------------------------------------------------
void fnSetMin(void)
{
    led_arr[12] = led_arr[8] = led_arr[4] = LED_BLINK;
    led_arr[0] = led_arr[13] =  LED_OFF;
    get_data = GET_MIN;
    min_val[0] = min_val[1] = min_val[2] = 4095;
    min_val[3] = min_val[4] = min_val[5] = 4095;
}
//--------------------------------------------------
void fnSetMax(void)
{
    led_arr[0] = led_arr[13]  = LED_BLINK;
    led_arr[12] = led_arr[8] = led_arr[4] = LED_OFF;
    get_data = GET_MAX;
    max_val[0] = max_val[1] = max_val[2] = 0;
    max_val[3] = max_val[4] = max_val[5] = 0;
}
//--------------------------------------------------
void fnSetMiddl(void)
{
    led_arr[0] = led_arr[13] = led_arr[12] = LED_BLINK;
    led_arr[8] = led_arr[4]  = LED_BLINK;
    get_data = GET_MIDDL;
}
//--------------------------------------------------
void fnCalcButtons(void)
{
    static char gps = 0;
    static char rec = 0;
    uint16_t butt = un.arr[0];
    uint16_t press = un.arr[1];
    if (un.arr[0] == 0xAAAA)
    {
        mode_data = S_CAL;
        iCntCl = 0;
        fnSetMin();
        return;
    }
    switch (butt)
    {
    case 0:// F0
        if (press)
        {
            if (mode_data == S_WORK)
            {
                fnClearLed5();
                sbus_msg.channels[4] = 1000;
                led_arr[12] = LED_ON;
            }
        }
        break;
    case 0x01:// F1
        if (press)
        {
            if (mode_data == S_WORK)
            {
                fnClearLed5();
                sbus_msg.channels[4] = 1295;
                led_arr[8] = LED_ON;
            }
        }
        break;
    case 0x02:// F2
        if (press)
        {
            if (mode_data == S_WORK)
            {
                fnClearLed5();
                sbus_msg.channels[4] = 1425;
                led_arr[4] = LED_ON;
            }
        }
        break;
    case 0x03:// F3
        if (press)
        {
            if (mode_data == S_WORK)
            {
                fnClearLed5();
                sbus_msg.channels[4] = 1555;
                led_arr[0] = LED_ON;
            }
        }
        break;
    case 0x04: // F4
        if (press)
        {
            if (mode_data == S_WORK)
            {
                fnClearLed5();
                sbus_msg.channels[4] = 1685;
                led_arr[13] = LED_ON;
            }
        }
        break;
    case 0x06: // gps
        if (press)
        {
            if (mode_data == S_WORK)
            {
                if (!gps)
                {
                    gps = 1;
                    led_arr[5] = LED_ON;
                    sbus_msg.channels[5] = 2000;
                }
                else
                {
                    gps = 0;
                    led_arr[5] = LED_OFF;
                    sbus_msg.channels[5] = 1000;
                }
            }
        }
        break;
    case 0x0B://+ ch11
        if (mode_data == S_WORK)
        {
            if (press)
            {
                sbus_msg.channels[11] = 2000;
            }
            else
            {
                sbus_msg.channels[11] = 1500;
            }
        }
        break;
    case 0x09://track on
        if (mode_data == S_WORK)
        {
            if (press)
            {
                sbus_msg.channels[11] = 1000;
            }
            else
            {
                sbus_msg.channels[11] = 1500;
            }
        }
        break;
    case 0x0A: //ch12 snap
        if (mode_data == S_WORK)
        {
            if (press)
            {
                sbus_msg.channels[12] = 2000;
            }
            else
            {
                sbus_msg.channels[12] = 1500;
            }
        }
        break;
    case 5:// record
        if (mode_data == S_WORK)
        {
            if (press)
            {
                sbus_msg.channels[13] = 2000;
                if (!rec)
                {
                    rec = 1;
                    led_arr[9] = LED_ON;
                }
                else
                {
                    rec = 0;
                    led_arr[9] = LED_OFF;
                }
            }
            else
            {
                sbus_msg.channels[13] = 1500;
            }
        }
        break;
    case 0x8://ch10+ зум-
        if (mode_data == S_WORK)
        {
            if (press)
            {
                sbus_msg.channels[10] = 2000;
            }
            else
            {
                sbus_msg.channels[10] = 1500;
            }
        }
        break;
    case 0x07: // ch10-
        if (mode_data == S_WORK)
        {
            if (press)
            {
                sbus_msg.channels[10] = 1000;
            }
            else
            {
                sbus_msg.channels[10] = 1500;
            }
        }
        break;
    case 0xC://ch14+ sl gimb
        if (mode_data == S_WORK)
        {
            if (press)
            {
                sbus_msg.channels[14] = 2000;
            }
            else
            {
                sbus_msg.channels[14] = 1500;
            }
        }
        break;
    case 0xE://DROP1
        if (mode_data == S_WORK)
        {
            if (press)
            {
                sbus_msg.channels[6] = 1900;
            }
            else
            {
                sbus_msg.channels[6] = 1500;
            }
        }
        break;
    case 0xF://DROP2
        if (mode_data == S_WORK)
        {
            if (press)
            {
                sbus_msg.channels[7] = 1200;
            }
            else
            {
                sbus_msg.channels[7] = 1500;
            }
        }
        break;
    }
}
//--------------------------------------------------
void fnCalibr(void)
{
    iCntCl++;
    if (iCntCl == 8000)
    {
        fnSetMax();
    }
    if (iCntCl == 16000)
    {
        fnSetMiddl();
    }
    if (iCntCl == 24000)
    {
        fnClearLed5();
        fnSaveY();
        mode_data = S_WORK;
        while (1) // reset
        {
        }
    }
    if (iCntCl % 10 == 0)
    {
        switch (get_data)
        {
        case GET_MIN:
            fnGetMinVal();
            break;
        case GET_MAX:
            fnGetMaxVal();
            break;
        case GET_MIDDL:
            fnGetMiddlVal();
            break;
        }
    }
}
//--------------------------------------------------
uint16_t fnUsred(uint8_t index)
{
    uint32_t summa = 0;
    summa = ADCConvertedValue[index];
    summa += ADCConvertedValue[index + 6];
    summa /= 2;
    summa += ADCConvertedValue[index + 12];
    summa /= 2;
    summa += ADCConvertedValue[index + 18];
    summa /= 4;
    return (uint16_t)summa;
}
//--------------------------------------------------
void fnGetMinVal(void)
{
    if (fnUsred(0) < min_val[0])
    {
        min_val[0] = fnUsred(0);
    }
    if (fnUsred(1) < min_val[1])
    {
        min_val[1] = fnUsred(1);
    }
    if (fnUsred(2) < min_val[2])
    {
        min_val[2] = fnUsred(2);
    }
    if (fnUsred(3) < min_val[3])
    {
        min_val[3] = fnUsred(3);
    }
    if (fnUsred(4) < min_val[4])
    {
        min_val[4] = fnUsred(4);
    }
    if (fnUsred(5) < min_val[5])
    {
        min_val[5] = fnUsred(5);
    }
}
//--------------------------------------------------
void fnGetMaxVal(void)
{
    if (fnUsred(0) > max_val[0])
    {
        max_val[0] = fnUsred(0);
    }
    if (fnUsred(1) > max_val[1])
    {
        max_val[1] = fnUsred(1);
    }
    if (fnUsred(2) > max_val[2])
    {
        max_val[2] = fnUsred(2);
    }
    if (fnUsred(3) > max_val[3])
    {
        max_val[3] = fnUsred(3);
    }
    if (fnUsred(4) > max_val[4])
    {
        max_val[4] = fnUsred(4);
    }
    if (fnUsred(5) > max_val[5])
    {
        max_val[5] = fnUsred(5);
    }
}
//--------------------------------------------------
void fnGetMiddlVal(void)
{
    midl_val[0] = fnUsred(0);
    midl_val[1] = fnUsred(1);
    midl_val[2] = fnUsred(2);
    midl_val[3] = fnUsred(3);
    midl_val[4] = fnUsred(4);
    midl_val[5] = fnUsred(5);
}
//--------------------------------------------------
void fnSaveY(void)
{
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
    st = FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3);
    delay_ms(50);
    uint16_t *pArr = (uint16_t *)&min_val[0];
    uint32_t start_adr = ADR_MIN;
    for (int i = 0; i < 6; i++)
    {
        FLASH_ProgramHalfWord(start_adr, *pArr++);
        start_adr += 2;
    }
    pArr = (uint16_t *)&max_val[0];
    start_adr = ADR_MAX;
    for (int i = 0; i < 6; i++)
    {
        FLASH_ProgramHalfWord(start_adr, *pArr++);
        start_adr += 2;
    }
    pArr = (uint16_t *)&midl_val[0];
    start_adr = ADR_MIDL;
    for (int i = 0; i < 6; i++)
    {
        FLASH_ProgramHalfWord(start_adr, *pArr++);
        start_adr += 2;
    }
    FLASH_Lock();
}
//--------------------------------------------------
void fnReadY(void)
{
    uint16_t *pArr = (uint16_t *)&min_val_r[0];
    uint32_t start_adr = ADR_MIN;
    for (int i = 0; i < 6; i++)
    {
        *pArr++ = (*(__IO uint16_t *)(start_adr));
        start_adr += 2;
    }
    pArr = (uint16_t *)&max_val_r[0];
    start_adr = ADR_MAX;
    for (int i = 0; i < 6; i++)
    {
        *pArr++ = (*(__IO uint16_t *)(start_adr));
        start_adr += 2;
    }
    pArr = (uint16_t *)&midl_val_r[0];
    start_adr = ADR_MIDL;
    for (int i = 0; i < 6; i++)
    {
        *pArr++ = (*(__IO uint16_t *)(start_adr));
        start_adr += 2;
    }
    for (int i = 0; i < 6; i++)
    {
        min_val_r[i] += 40;
        max_val_r[i] -= 40;
    }
}
//--------------------------------------------------



