
//**************************************************
//! Файл                                    : main.h
//**************************************************
#ifndef __MAIN_H
#define __MAIN_H


#include "stm32f4xx.h"

#include <string.h>
#include <stdint.h>
#include <stdio.h>

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
#include "bufer.h"


typedef struct
{
    bool u1;
    bool u2;
    bool u6;
} send_free_t;

extern volatile send_free_t send_free;

typedef struct
{
    bool ms_1;
    bool ms_250;
} flags_t;
extern volatile flags_t flags;
//----------------------
#endif /* __MAIN_H */


