//**************************************************
// Файл                 : task_udp.c
//**************************************************
/* инициатором обмена всегда является клиент, мишин планер - это сервер,
поэтому нужно долбить пакетами до тех пор, пока не появится ответ
*/

#include "main.h"
#include "..\net\socket.h"
#include "..\net\spi.h"

static MODE_ETH mode_eth = INIT_ETH;
typedef enum {INIT_PIN = 0, PAUSE_0, PAUSE_1, END_INIT} MODE_INIT;
static MODE_INIT mode_init = INIT_PIN;
static uint16_t delay_pause = 0;
int iCntUart;
int32_t len_udp;
int32_t len_udp2;

static char start_eth = 0;
wiz_NetInfo gWIZNETINFO;
const uint8_t adr_all[4] = {255, 255, 255, 255};
const uint8_t adr_my[4] = {192, 168, 0, 20};
uint8_t memsize[2][8] = { {4, 4, 4, 4, 0, 0, 0, 0}, {4,4, 4, 4, 0,0, 0,0}};
uint8_t buff_rf[512];
const uint8_t adr_all_comp[4] = {192, 168, 0, 6};
const uint8_t adr_868[4] = {192, 168, 0, 6};
uint8_t addr[4];
static int32_t len;
uint16_t port;
//--------------------------------------------------------------------------
void fnClearInit(void)
{
    mode_init = INIT_PIN;
    mode_eth = INIT_ETH;
}
MODE_ETH fnGetModeETH(void)
{
    return mode_eth;
}
char fnGetLinkEth(void)
{
    return start_eth;
}
//--------------------------------------------------------------------------
void Load_Net_Parameters(void)
{
    gWIZNETINFO.gw[0] = 192; //Gateway
    gWIZNETINFO.gw[1] = 168;
    gWIZNETINFO.gw[2] = 0;
    gWIZNETINFO.gw[3] = 1;
    gWIZNETINFO.sn[0] = 255; //Mask
    gWIZNETINFO.sn[1] = 255;
    gWIZNETINFO.sn[2] = 255;
    gWIZNETINFO.sn[3] = 0;
    gWIZNETINFO.mac[0] = 0x0c; //MAC
    gWIZNETINFO.mac[1] = 0x29;
    gWIZNETINFO.mac[2] = 0xab;
    gWIZNETINFO.mac[3] = 0x7c;
    gWIZNETINFO.mac[4] = 0x12;
    gWIZNETINFO.mac[5] = 0x34;
    gWIZNETINFO.ip[0] = 192; //IP
    gWIZNETINFO.ip[1] = 168;
    gWIZNETINFO.ip[2] = 0;
    gWIZNETINFO.ip[3] = 20;
    gWIZNETINFO.dns[0] = gWIZNETINFO.dns[1] = gWIZNETINFO.dns[2] = gWIZNETINFO.dns[3] = 8;
    gWIZNETINFO.dhcp = NETINFO_STATIC;
}
//--------------------------------------------------------------------------
void network_init(void)
{
    wiz_NetTimeout gWIZNETTIME = {.retry_cnt = 3, .time_100us = 2000};
    ctlnetwork(CN_SET_TIMEOUT, (void *)&gWIZNETTIME);
    ctlnetwork(CN_GET_TIMEOUT, (void *)&gWIZNETTIME);
    ctlnetwork(CN_SET_NETINFO, (void *)&gWIZNETINFO);
    ctlnetwork(CN_GET_NETINFO, (void *)&gWIZNETINFO);
    uint8_t tmpstr[6];
    ctlwizchip(CW_GET_ID, (void *)tmpstr);
}
//--------------------------------------------------------------------------
void fnProcessUDP()
{
    switch (mode_eth)
    {
    case INIT_ETH:
        switch (mode_init)
        {
        case INIT_PIN:
            //
            SPI1_Init(); // SPI2: b3 b4 b5
            register_wizchip();
            W5500_CS_Init(); // Initialize W5500,  RST INT
            W5500_RST_Init();
            W5500_RESET(0);
            mode_init = PAUSE_0;
            delay_pause = 0;
            break;
        case PAUSE_0:
            delay_pause++;
            if (delay_pause == 5)
            {
                W5500_RESET(1);
                delay_pause = 0;
                mode_init = PAUSE_1;
            }
            break;
        case PAUSE_1:
            delay_pause++;
            if (delay_pause == 5)
            {
                delay_pause = 0;
                mode_init = END_INIT;
            }
            break;
        case END_INIT:
            Load_Net_Parameters();
            if (ctlwizchip(0, (void *)memsize) == -1)
            {
                while (1);
            }
            network_init();
            setSHAR(gWIZNETINFO.mac);
            ctlnetwork(CN_GET_NETINFO, (void *)&gWIZNETINFO);
            socket(0, Sn_MR_UDP, 8888, SF_IO_NONBLOCK);// для мп передача прием
            socket(1, Sn_MR_UDP, 8888, SF_IO_NONBLOCK);// для планки
            mode_eth = WORK_ETH;
            break;
        }
        break;
    case WORK_ETH:
        len = recvfrom(0, buff_rf, 4000, (uint8_t *) addr, &port);
        if (len > 0)
        {
            if (len != SOCK_BUSY)
            {
                if (len != SOCKERR_SOCKMODE)
                {
                    if (port == 14550)
                    {
                        IWDG_ReloadCounter();
                        start_eth = 1;
                        fnAddBufer_Ser((uint8_t *)buff_rf,  len);
                    }
                }
            }
        }// end len >0
        break;
    }
    //-----------------------------------------------
    //    close(0);
}
//--------------------------------------------------------------------------



