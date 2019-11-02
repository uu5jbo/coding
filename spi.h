#ifndef SPI_H_INCLUDED
#define SPI_H_INCLUDED


#include "stm32f10x_conf.h"

#define WIP(x) ((x) & 1)    //EEPROM Macro Write In Progress
#define EEPROM_PORT GPIOC
#define EEPROM_CS GPIO_Pin_10
#define EEPROM_SPI SPI2
#define EEPROM_SPEED SPI_SLOW


enum spiSpeed { SPI_SLOW, SPI_MEDIUM, SPI_FAST};    // SPI Speeds

enum eepromCMD { cmdREAD = 0x03, cmdWRITE = 0x02,
                 cmdWREN = 0x06, cmdWRDI = 0x04,
                 cmdRDSR = 0x05, cmdWRSR = 0x01 
};  //EEPROM commands

void csInit(void);
void spiInit(SPI_TypeDef* SPIx);
int spiReadWrite(SPI_TypeDef* SPIx, uint8_t *rbuf,
                 const uint8_t *tbuf, int cnt,
                 enum spiSpeed speed);
int spiReadWrite16(SPI_TypeDef* SPIx, uint16_t *rbuf,
                   const uint16_t *tbuf, int cnt,
                   enum spiSpeed speed);
                   
void spi2_init(void);    

//EEPROM interface
void eepromInit(void);
void eepromWriteEnable(void);
void eepromWriteDisable(void);
uint8_t eepromReadStatus(void);
void eepromWriteStatus(uint8_t status);
int eepromWrite(uint8_t *buf, uint8_t cnt, uint16_t offset);
int eepromRead(uint8_t *buf, uint8_t cnt, uint16_t offset);                   


#endif /* SPI_H_INCLUDED */
