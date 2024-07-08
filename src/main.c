
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
    while (1)
    {
       
    }
}
//==============================================================================




