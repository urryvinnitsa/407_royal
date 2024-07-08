
#include "stm32f4xx.h"
#include <nvic.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/process.h>
#include <sys/procinit.h>
#include <etimer.h>
#include <sys/autostart.h>
#include <clock.h>

//#include "main.h"


unsigned int idle_count = 0;

int main(void)
{
#if 0
    /*
     * make full erase if forget readout protection bit
     */
    if (OB->RDP != 0xff00)
    {
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
        FLASH->CR |= FLASH_CR_MER; /* (1) */
        FLASH->CR |= FLASH_CR_STRT; /* (2) */
        while ((FLASH->SR & FLASH_SR_BSY) != 0); /* (3) */
        if ((FLASH->SR & FLASH_SR_EOP) != 0)   /* (4)*/
        {
            FLASH->SR |= FLASH_SR_EOP; /* (5) */
        }
        FLASH->CR &= ~FLASH_CR_MER;
    }
    //Enabling IWDT
    IWDG->KR = 0xAAAA; //reset
    IWDG->KR = 0x5555; //enable access
    IWDG->RLR = 0xfff; //set reload register to max
    IWDG->PR = 0x07; // set divider /4
    IWDG->KR = 0xCCCC; //enable
#endif
    clock_init();
    process_init();
    process_start(&etimer_process, NULL);
    autostart_start(autostart_processes);
    while (1)
    {
        do
        {
        }
        while (process_run() > 0);
        idle_count++;
        /* Idle! */
        /* Stop processor clock */
        /* asm("wfi"::); */
    }
    return 0;
}




