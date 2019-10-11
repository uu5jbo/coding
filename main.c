/*
**
**                           Main.c
**
**
**********************************************************************/
/*
   Last committed:     $Revision: 00 $
   Last changed by:    $Author: $
   Last changed date:  $Date:  $
   ID:                 $Id:  $

**********************************************************************/
//#include "stm32f10x_conf.h"
#include "spi.h"
//#include "spi.c"

uint8_t txbuf[4], rxbuf[4];
uint16_t txbuf16[4], rxbuf16[4];

int main(void)
{
    int i, j;

    csInit(); // Init chip select PC03
    spiInit(SPI2);

    for(i = 0; i < 8; i++)
    {
        for(j = 0; j < 4; j++)
            txbuf[j] = i * 4 + j;
        GPIOC->BSRR |= GPIO_BSRR_BR3;
        spiReadWrite(SPI2, rxbuf, txbuf, 4, SPI_SLOW);
        GPIOC->BSRR |= GPIO_BSRR_BS3;
        /*
        for(j = 0; j < 4; j++)
            if(rxbuf[j] != txbuf[j])
                assert_failed(__FILE__, __LINE__);
         */
    }

    for(i = 0; i < 8; i++)
    {
        for(j = 0; j < 4; j++)
            txbuf[j] = i * 4 + j + (i<<8);
        GPIOC->BSRR |= GPIO_BSRR_BR3;
        spiReadWrite16(SPI2, rxbuf16, txbuf16, 4, SPI_SLOW);
        GPIOC->BSRR |= GPIO_BSRR_BS3;
        /*
        for(j = 0; j < 4; j++)
            if(rxbuf16[j] != txbuf16[j])
                assert_failed(__FILE__, __LINE__);
        */
    }
}



