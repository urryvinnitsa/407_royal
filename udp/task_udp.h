//**************************************************
// Файл                 : task_udp.h

#ifndef TASK_UDP_H
#define TASK_UDP_H
#include <stdint.h>
extern struct process task_udp_process;
extern  uint8_t buff_rf[];
extern  const uint8_t adr_all_comp[];
extern const uint8_t adr_477[];
extern const uint8_t adr_868[];
char fnGetLinkEth(void);
typedef enum {INIT_ETH = 0, WORK_ETH} MODE_ETH;
//extern  uint8_t buff_vw[];
MODE_ETH fnGetModeETH(void);
void fnClearInit(void);
#endif //TASK_UDP_H


