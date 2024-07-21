//**************************************************
// Файл  : bufers.c
//--------------------------------------------------
#include "main.h"
#include "task_udp.h"

#define BUF_SIZE_READ 64
volatile  uint8_t arr_rf[512];
volatile  uint8_t arr_ser[512];
uint16_t len_n;
uint16_t len_s;
uint16_t len_real_m;
//--------------------------------------------------------------
void fnBuferInit(void);
//--------------------------------------------------------------
#pragma pack(1)
volatile    uint16_t strBuffData_cRead_Rf ; /*!<указатель чтения*/
volatile    uint16_t strBuffData_cWrite_Rf;/*!<указатель записи*/
volatile    uint16_t NumberOfItems_Rf; // количество записей в буфере
volatile  un_packet un_rf[BUF_SIZE_READ];

volatile    uint16_t strBuffData_cRead_Ser ; /*!<указатель чтения*/
volatile    uint16_t strBuffData_cWrite_Ser;/*!<указатель записи*/
volatile    uint16_t NumberOfItems_Ser; // количество записей в буфере

volatile  un_packet un_ser[BUF_SIZE_READ];

#pragma pack()
static uint8_t cnt_rf = 0;
PROCESS(bufer_process, "Bufer");

PROCESS_THREAD(bufer_process, ev, data)
{
    PROCESS_BEGIN();
    fnBuferInit();
    while (1)
    {
        static struct etimer et;
        etimer_set(&et, 1);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        //
        if (fnGetBufer_Rf() == BUFER_OK)
        {
            len_real_m =  sendto(1, (uint8_t *) arr_rf, len_n, (uint8_t *) adr_all_comp, 14550);
//            if (len_real_m != len_n)
//            {
//                cnt_rf++;
//                if (cnt_rf > 2)
//                {
//                    cnt_rf = 0;
//                    fnClearInit();
//                }
//            }
//            else
//            {
//                cnt_rf = 0;
//            }
        }
    }
    PROCESS_END();
}
//--------------------------------------------------------------
void fnBuferInit(void)
{
    // устанавливаем (обнуляем) стартовый указатель
    strBuffData_cRead_Rf = strBuffData_cWrite_Rf = 0;
    NumberOfItems_Rf = 0;
    strBuffData_cRead_Ser = strBuffData_cWrite_Ser = 0;/*!<указатель записи*/
    NumberOfItems_Ser = 0; // количество записей в буфере
}
//--------------------------------------------------------------
REZ_BUFER fnAddBufer_Rf(uint8_t *arr_start, uint16_t len)// добавить  в буфер
{
    if (NumberOfItems_Rf == BUF_SIZE_READ)  // buffer is full
    {
        return BUFER_BUSY; //
    }
    un_rf[strBuffData_cWrite_Rf].len = len;
    for (int i = 0; i < len; i++)
    {
        un_rf[strBuffData_cWrite_Rf].arr[i] = arr_start[i];
    }
    strBuffData_cWrite_Rf = (strBuffData_cWrite_Rf + 1) % BUF_SIZE_READ;
    NumberOfItems_Rf++;
    return BUFER_OK;
}
////--------------------------------------------------------------
REZ_BUFER fnGetBufer_Rf(void)// взять из буфера
{
    if (NumberOfItems_Rf == 0)  // buffer is EMPTY
    {
        return BUFER_EMPTY;
    }
    uint16_t len = un_rf[strBuffData_cRead_Rf].len;
    len_n = len;
    for (uint16_t i = 0; i < len; i++)
    {
        arr_rf[i] = un_rf[strBuffData_cRead_Rf].arr[i];
    }
    strBuffData_cRead_Rf = (strBuffData_cRead_Rf + 1) % BUF_SIZE_READ;
    NumberOfItems_Rf--;
    return BUFER_OK;
}
////--------------------------------------------------------------
REZ_BUFER fnAddBufer_Ser(uint8_t *arr_start, uint16_t len)// добавить  в буфер
{
    if (NumberOfItems_Ser == BUF_SIZE_READ)  // buffer is full
    {
        return BUFER_BUSY; //msgmav_rf.msg
    }
    un_ser[strBuffData_cWrite_Ser].len = len;
    for (int i = 0; i < len; i++)
    {
        un_ser[strBuffData_cWrite_Ser].arr[i] = arr_start[i];
    }
    strBuffData_cWrite_Ser = (strBuffData_cWrite_Ser + 1) % BUF_SIZE_READ;
    NumberOfItems_Ser++;
    return BUFER_OK;
}
////--------------------------------------------------------------
REZ_BUFER fnGetBufer_Ser(void)// взять из буфера
{
    if (NumberOfItems_Ser == 0)  // buffer is EMPTY
    {
        return BUFER_EMPTY;
    }
    //  uint16_t len_r = un_rf[strBuffData_cRead_Rf].len;
    uint16_t len = un_ser[strBuffData_cRead_Ser].len;
    len_s = len;
    for (uint16_t i = 0; i < len; i++)
    {
        arr_ser[i] = un_ser[strBuffData_cRead_Ser].arr[i];
    }
    strBuffData_cRead_Ser = (strBuffData_cRead_Ser + 1) % BUF_SIZE_READ;
    NumberOfItems_Ser--;
    return BUFER_OK;
}
////--------------------------------------------------------------



