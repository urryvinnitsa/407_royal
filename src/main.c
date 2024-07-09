
//**************************************************
//! Файл                                    : main.c
//! Разработка                              :
//! Дата создания                           :
//! Описание                                :
//**************************************************

/* Includes ------------------------------------------------------------------*/
#include "main.h"
// проверка работы мавлинка
mavlink_message_t message;
typedef struct
{
    uint16_t channels[16];
    bool digital_channel_1;
    bool digital_channel_2;
    bool frame_lost;
    bool failsafe;
} sbus_msg_t;
volatile sbus_msg_t sbus_msg;


PROCESS(led_process, "Led");
PROCESS_THREAD(led_process, ev, data)
{
    //    static int rez = 0;
    static int cnt = 0;
    PROCESS_BEGIN();
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
        cnt++;
        if (cnt >= 500)
        {
            cnt = 0;
            GPIO_ToggleBits(GPIOC, GPIO_Pin_13);
            IWDG_ReloadCounter();
        }
    }
    PROCESS_END();
}
//--------------------------------------------------


//==============================================================================
int main(void)
{
    SystemInit();
    clock_init();
    rtimer_init();
    process_init();
    // only test !!!
    uint8_t buffer_sbus[60];
    // создаем мавлинк пакет
    mavlink_msg_rc_channels_override_pack(0xff, 158, &message, 1, 1,
                                          sbus_msg.channels[0], sbus_msg.channels[1], sbus_msg.channels[2], sbus_msg.channels[3],
                                          sbus_msg.channels[4], sbus_msg.channels[5], sbus_msg.channels[6],
                                          sbus_msg.channels[7], sbus_msg.channels[8], sbus_msg.channels[9], sbus_msg.channels[10],
                                          sbus_msg.channels[11], sbus_msg.channels[12], sbus_msg.channels[13], sbus_msg.channels[14],
                                          sbus_msg.channels[15],
                                          1000, 2500);
    message.magic = 0xFD;// mavlink 2
    // копируем пакет в буфер
    mavlink_msg_to_send_buffer((uint8_t *)buffer_sbus, &message);
    // end test !!
    // запуск светика индикации работы и юдп процесса на w5500, должен пинговаться 192.168.0.20
    process_start(&led_process, NULL);
    process_start(&task_udp_process, NULL);
    while (1)
    {
    }
}
//==============================================================================




