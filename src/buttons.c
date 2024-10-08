//**************************************************
//! Файл                                : buttons.c
//**************************************************
#include "main.h"

typedef struct
{
    bool blok;
    bool naj;
    bool otj; //
    signed char st;//
} but_t ;
static  but_t but[BUTTON_MAX];
#define BUT_TIME_MAX 10
#define kn_ot(x) {but[x].otj=1;but[x].st =0;but[x].blok=0; but[x].naj=0;}
#define kn_nj(x)  {but[x].naj=1;but[x].st =BUT_TIME_MAX;but[x].otj=0;}
uint8_t new_button;
void fnButtInit(void);
static void fnButtRead(void);
static void fnSetRegButt(void);
static uint16_t rez_butt = 0; // new
static uint16_t rez_butt_old = 0; // old

const buttons_t buttons[BUTTON_MAX] = {\
                                       [0].Port_B = B0_PORT, [0].Pin_B = B0_PIN, \
                                       [1].Port_B = B1_PORT, [1].Pin_B = B1_PIN, \
                                       [2].Port_B = B2_PORT, [2].Pin_B = B2_PIN, \
                                       [3].Port_B = B3_PORT, [3].Pin_B = B3_PIN, \
                                       [4].Port_B = B4_PORT, [4].Pin_B = B4_PIN, \
                                       [5].Port_B = B5_PORT, [5].Pin_B = B5_PIN, \
                                       [6].Port_B = B6_PORT, [6].Pin_B = B6_PIN, \
                                       [7].Port_B = B7_PORT, [7].Pin_B = B7_PIN, \
                                       [8].Port_B = B8_PORT, [8].Pin_B = B8_PIN, \
                                       [9].Port_B = B9_PORT, [9].Pin_B = B9_PIN, \
                                       [10].Port_B = B10_PORT, [10].Pin_B = B10_PIN, \
                                       [11].Port_B = B11_PORT, [11].Pin_B = B11_PIN, \
                                       [12].Port_B = B12_PORT, [12].Pin_B = B12_PIN, \
                                       [13].Port_B = B13_PORT, [13].Pin_B = B13_PIN, \
                                       [14].Port_B = B14_PORT, [14].Pin_B = B14_PIN, \
                                       [15].Port_B = B15_PORT, [15].Pin_B = B15_PIN, \
                                      };
//---------------------------------------------------
void fnProcessButton(void)
{
    fnButtRead();
    fnSetRegButt();
}
//---------------------------------------------------
void fnButtInit(void)
{
    uint8_t i;
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    for (i = 0; i < BUTTON_MAX; i++)
    {
        GPIO_InitStruct.GPIO_Pin = buttons[i].Pin_B ;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_Init(buttons[i].Port_B, &GPIO_InitStruct);
    }
}
//---------------------------------------------------
static void fnButtRead(void)
{
    uint8_t i;
    for (i = 0; i < BUTTON_MAX; i++)
    {
        if ((!GPIO_ReadInputDataBit(buttons[i].Port_B, buttons[i].Pin_B)))
        {
            but[i].st++;
            if (but[i].st >= BUT_TIME_MAX)
            {
                kn_nj(i);
            }
        }
        else
        {
            but[i].st--;
            if (but[i].st <= 0)
            {
                kn_ot(i);
            }
        }
    }
}
//---------------------------------------------------
static void fnSetRegButt(void)
{
    for (int i = 0; i < BUTTON_MAX; i++)
    {
        if (but[i].naj)
        {
            BIT_SET(rez_butt, i);
        }
        else
        {
            BIT_CLR(rez_butt, i);
        }
    }
    // передаем только в том случае, когда состояние кнопки изменилось
    uint16_t rez = rez_butt_old ^ rez_butt;
    if (rez)
    {
        for (int i = 0; i < BUTTON_MAX; i++)
        {
            if (BIT_TST1(rez, i))
            {
                un.arr[0] = i;// забираем номер кнопки
                if ((BIT_TST1(rez_butt, i))) // новое значение - нажата или отпущена
                {
                    un.arr[1] = 1; // press
                }
                else
                {
                    un.arr[1] = 0; //unpress
                }
                break;
            }
        }
        //---------------------------------------------------
               if (rez_butt == 0x1c)
                {
                    un.i = 0xAAAA;
                    new_button = 1;
									 return;
                }

       rez_butt_old = rez_butt;
        new_button = 1;
    }
}
//----------------------------------------------------------------

