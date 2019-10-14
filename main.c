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
    uint8_t data = 0xf5;

    //csInit(); // Init chip select PC03
    spiInit(SPI1);
    spiInit(SPI2);

    while(1)
    {

        GPIOC->BSRR |= GPIO_BSRR_BR3 ;
        GPIOA->BSRR |= GPIO_BSRR_BR4;
        spi_send(SPI1, data, rxbuf, SPI_SLOW );
        spi_send(SPI2, data, rxbuf, SPI_SLOW );
        GPIOC->BSRR |= GPIO_BSRR_BS3;
        GPIOA->BSRR |= GPIO_BSRR_BS4;
    }
}



