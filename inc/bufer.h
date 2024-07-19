//**************************************************
// Файл                     : bufer.h
//**************************************************

#ifndef BUFERS_H
#define BUFERS_H
#include <stdint.h>
//==================================================
//работа с кольцевым буфером
//состояние буфера - перечисление всех возможных ответов

typedef enum
{
    BUFER_OK = 0,/*!<структура удачно положена или извлечена из буфера*/\
    BUFER_BUSY,/*!<буфер переполнен (для функции - добавить)*/\
    BUFER_EMPTY/*!<буфер пустой, команда не принята (для функции - взять)*/\
} REZ_BUFER;

#define ARR_SIZE_511 512

#pragma pack(1)

typedef struct
{
    uint16_t len;
    uint8_t arr[ARR_SIZE_511];
} un_packet;

#pragma pack()

REZ_BUFER fnAddBufer_Rf(uint8_t *arr_start, uint16_t len);
REZ_BUFER fnGetBufer_Rf(void);
REZ_BUFER fnAddBufer_Ser(uint8_t *arr_start, uint16_t len);
REZ_BUFER fnGetBufer_Ser(void);

extern struct process bufer_process;
extern uint16_t len_s;
extern volatile  uint8_t arr_ser[];

#endif //BUFERS_H

