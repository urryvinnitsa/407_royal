
//**************************************************
//! Файл                                    : main.c
//**************************************************
#include "main.h"

// оценка бездействия мк
uint32_t idle_count = 0;
//-----------------------------------------------
// объявляем события, передаваемые между потоками
process_event_t event_button;
process_event_t event_1ms;
process_event_t event_kill;
// ----------------------------------------------
volatile send_free_t send_free;
// ----------------------------------------------

PROCESS(led_process, "Led");
PROCESS_THREAD(led_process, ev, data)
{
    static int rez = 0;
    static int cnt = 0;
    PROCESS_BEGIN();
    //
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 ; //
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    while (1)
    {
        static struct etimer et;
        etimer_set(&et, 1);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        //process_post(&task_adc_process, event_1ms, &rez);
        cnt++;
        if (cnt >= 500) // период мигания светика индикации работы
        {
            cnt = 0;
            GPIO_ToggleBits(GPIOC, GPIO_Pin_13);
            IWDG_ReloadCounter();
        }
    }
    PROCESS_END();
}
//==============================================================================
int main(void)
{
    SystemInit();
    clock_init(); // запуск контики
    rtimer_init();
    process_init();
    // создаем события-сигналы до потоков
    event_button = process_alloc_event();
    event_1ms = process_alloc_event();
    event_kill = process_alloc_event();
    process_start(&etimer_process, NULL);
    //-----------------------------------
    // запуск светика индикации работы
    process_start(&led_process, NULL);
    // процесс заполнения структуры и создания мавлинк пакета
    process_start(&task_adc_process, NULL);
    // запуск  юдп процесса на w5500, должен пинговаться 192.168.0.20
    process_start(&task_udp_process, NULL);
    //    // запуск клавиатуры
    process_start(&button_process, NULL);
    //    // запуск светиков
    process_start(&leds_process, NULL);
    //
    // cтендовый режим - напрямую с пиксой без радиоканала
    process_start(&link_rf_process, NULL);
    //
    while (1)
    {
        do {}
        while (process_run() > 0);
        idle_count++;
    }
}
//==============================================================================




