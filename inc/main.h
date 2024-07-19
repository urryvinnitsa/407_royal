
//**************************************************
//! Файл                                    : main.h
//**************************************************
#ifndef __MAIN_H
#define __MAIN_H


#include "stm32f4xx.h"

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "contiki.h"
#include "mavlink.h"
//
#include "spi.h"
#include "dhcp.h"
#include "socket.h"
#include "task_udp.h"
//
#include "adc_j.h"
#include "hard_config.h"
#include "buttons.h"
#include "leds.h"
#include "link_rf.h"

// сигналы до потоков ---
extern process_event_t event_button;
extern process_event_t event_1ms;
extern process_event_t event_kill;

typedef struct
{
    bool u1;
    bool u2;
    bool u6;
} send_free_t;

extern volatile send_free_t send_free;


//----------------------
#endif /* __MAIN_H */


