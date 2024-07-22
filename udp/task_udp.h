//**************************************************
// Файл                 : task_udp.h

#ifndef TASK_UDP_H
#define TASK_UDP_H
#include <stdint.h>
extern  uint8_t buff_rf[];
extern  const uint8_t adr_all_comp[];
extern const uint8_t adr_477[];
extern const uint8_t adr_868[];
char fnGetLinkEth(void);
typedef enum {INIT_ETH = 0, WORK_ETH} MODE_ETH;
MODE_ETH fnGetModeETH(void);
void fnClearInit(void);
void fnProcessUDP(void);
#endif //TASK_UDP_H


