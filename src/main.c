
//**************************************************
//! Файл                : main.c
//**************************************************
#include "main.h"
// пробуем одновременно стендовый и рабочий режимы
// выделяем 2 сокета - 0 сокет используется для обмена
// юдп пакетами между мишин планнером и пиксой
// 1 сокет для выдачи пакетов на планку
//-------------------------------------------------
//флаги
volatile flags_t flags;
// ----------------------------------------------
volatile send_free_t send_free;
// ----------------------------------------------
void TIM2_Init(void);
void   fnLedInit(void);
// ----------------------------------------------
void TIM2_IRQHandler(void)
{
    static uint16_t iCnt = 0;
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        flags.ms_1 = 1;
        iCnt++;
        if (iCnt >= 250)
        {
            iCnt = 0;
            flags.ms_250 = 1;
            GPIO_ToggleBits(GPIOC, GPIO_Pin_13);
        }
    }
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}
//--------------------------------------------------
//==============================================================================
int main(void)
{
   SystemInit();
    TIM2_Init();
    // тактируем все порты
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |
                           RCC_AHB1Periph_GPIOB |
                           RCC_AHB1Periph_GPIOC |
                           RCC_AHB1Periph_GPIOD |
                           RCC_AHB1Periph_GPIOE |
                           RCC_AHB1Periph_GPIOF |
                           RCC_AHB1Periph_GPIOG, ENABLE);
    //-----------------------------------
    // запуск светика индикации работы
    fnLedInit();
    //    // процесс заполнения структуры и создания мавлинк пакета, init
    fnAdcInit();
    //    // запуск  юдп процесса на w5500, , init должен пинговаться 192.168.0.20
    //    //    // запуск init клавиатуры
    fnButtInit();
    //    //    // запуск  init светиков
    fnLedsInit();
    //    //
    //    // cтендовый режим - напрямую с пиксой без радиоканала
    fnDMAInit(); // uART1
    fnBuferInit();
    //
		
		
			IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
			IWDG_SetPrescaler(IWDG_Prescaler_32);
			IWDG_SetReload(5000);
			IWDG_Enable();
    //--------------------------------------------------------
    while (1)
    {
        if (flags.ms_1)
        {
            flags.ms_1 = 0;
            fnProcessUDP();
            fnProcessButton();
            fnProcessLeds();
            fnProcessLink();
            fnProcessAdc();
					  IWDG_ReloadCounter();
        }
			#if 0	
        if (fnGetModeETH() == WORK_ETH)// 5500 в работе
        {
            if (fnGetBufer_Rf() == BUFER_OK)
            {
                sendto(0, (uint8_t *) arr_rf, len_n, (uint8_t *) adr_all_comp, 14550);
            }
        }
        if (send_free.u1 == 0)// дма отработал и свободен
        {
            if (fnGetBufer_Ser()  == BUFER_OK)
            {
                USART1SendDMA((uint8_t *)arr_ser, len_s);
            }
        }
				#endif
    }
}
//--------------------------------------------------------
void TIM2_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_DeInit(TIM2);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / 200000); // -> 200KHz
    TIM_TimeBaseStructure.TIM_Period = 200 - 1; // 1 ms
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    TIM_Cmd(TIM2, ENABLE);
}
//--------------------------------------------------------
void   fnLedInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 ; //
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
}
//--------------------------------------------------------
//==============================================================================



