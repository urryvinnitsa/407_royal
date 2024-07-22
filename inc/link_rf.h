//**************************************************
// Файл                     : link_rf.h
//**************************************************
// cвязь с autopilot - 1 уарт
#include <stdint.h>
#ifndef LINK_RF_H
#define LINK_RF_H
extern volatile uint8_t buf_uart[];
void USART1SendDMA(uint8_t *arr, uint16_t len);
void fnDMAInit(void);
void fnProcessLink(void);
#endif //LINK_RF_H



