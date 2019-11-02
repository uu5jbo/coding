
#include "spi.h"

uint8_t txbuf[4], rxbuf[4];
uint16_t txbuf16[4], rxbuf16[4];
int main()
{
    eepromInit();
    eepromReadStatus();
    eepromWriteEnable();
    eepromReadStatus();
    eepromWriteDisable();
     eepromReadStatus();
    //eepromWriteStatus(cmdWRITE);
      
}



