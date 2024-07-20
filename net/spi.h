#ifndef __SPI_H
#define __SPI_H
void SPI1_Init(void);
void register_wizchip(void);
void W5500_CS_Init(void);
void W5500_RST_Init(void);
#define W5500_RESET(x) x ? GPIO_SetBits(SPI1_RESET_PORT,SPI1_RESET_PIN) : GPIO_ResetBits(SPI1_RESET_PORT,SPI1_RESET_PIN);
#endif














