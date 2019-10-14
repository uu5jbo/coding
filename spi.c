
#include "spi.h"

static const uint16_t speeds [] =
{
    [SPI_SLOW] = SPI_BaudRatePrescaler_64 ,
    [SPI_MEDIUM] = SPI_BaudRatePrescaler_8 ,
    [SPI_FAST] = SPI_BaudRatePrescaler_2
};

void csInit(void)
{
    // SS Chip select
    /*
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    GPIOC->CRL |= GPIO_CRL_MODE3;
    GPIOC->CRL &= ~GPIO_CRL_CNF3;   // Output PP 50Mhz
    GPIOC->BSRR |= GPIO_BSRR_BS3; // Set High on PC3
    */
    __NOP();
}

void spiInit(SPI_TypeDef *SPIx)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit (& GPIO_InitStructure);
    SPI_StructInit (& SPI_InitStructure);
    if (SPIx == SPI2)
    {
        RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; // Clock to GPIO Port B
        RCC->APB2ENR |= RCC_APB1ENR_SPI2EN;  //Clock to SPI2 Module on APB1 bus

        RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; // Clock to alternative functions
        // SS Chip select
        RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
        GPIOC->CRL |= GPIO_CRL_MODE3;
        GPIOC->CRL &= ~GPIO_CRL_CNF3;   // Output PP 50Mhz
        GPIOC->BSRR |= GPIO_BSRR_BS3; // Set High on PC3

        //SCK
        GPIOB->CRH |= GPIO_CRH_MODE13; // Output 50 MHz
        GPIOB->CRH &= ~GPIO_CRH_CNF13;  //Reset both bits
        GPIOB->CRH |= GPIO_CRH_CNF13_1; // PB13 Alternat. func, PP

        //MOSI
        GPIOB->CRH |= GPIO_CRH_MODE15; // Output 50 MHz
        GPIOB->CRH &= ~GPIO_CRH_CNF15;  //Reset both bits
        GPIOB->CRH |= GPIO_CRH_CNF15_1; // PB13 Alternat. func, PP

        //MISO
        GPIOB->CRH &= ~GPIO_CRH_MODE14;
        GPIOB->CRH &= ~GPIO_CRH_CNF14;
        GPIOB->CRH |= GPIO_CRH_CNF14_1;  // PB14 Input Pull up/down
        GPIOB->BSRR = GPIO_BSRR_BS14;    // PB14 Pull Up

        /* Enable clocks , configure pins
        ...
        */
    }
    else if (SPIx == SPI1)
    {
        //Clock to SPI1 and GPIOA
        RCC->APB2ENR |= RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN;
        RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; //Clk to alternative fncs.

        //Slave Select SS: Out, PP, 50MHz                             /
        GPIOA->CRL   |=  GPIO_CRL_MODE4;    //  PA4
        GPIOA->CRL   &= ~GPIO_CRL_CNF4;     //
        GPIOA->BSRR   =  GPIO_BSRR_BS4;     //

        //SCK: PP, alternative function, 50MHz
        GPIOA->CRL   |=  GPIO_CRL_MODE5;    //  PA5
        GPIOA->CRL   &= ~GPIO_CRL_CNF5;     //
        GPIOA->CRL   |=  GPIO_CRL_CNF5_1;   //

        //вывод MISO: Input Pull-Up.
        GPIOA->CRL   &= ~GPIO_CRL_MODE6;    //  PA6
        GPIOA->CRL   &= ~GPIO_CRL_CNF6;     //
        GPIOA->CRL   |=  GPIO_CRL_CNF6_1;   //
        GPIOA->BSRR   =  GPIO_BSRR_BS6;     //

        //вывод MOSI: OUT Push-Pull, alternative function, 50MHz
        GPIOA->CRL   |=  GPIO_CRL_MODE7;    //  PA7
        GPIOA->CRL   &= ~GPIO_CRL_CNF7;     //
        GPIOA->CRL   |=  GPIO_CRL_CNF7_1;   //
    }

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = speeds[SPI_SLOW ];
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPIx , &SPI_InitStructure);
    SPI_Cmd(SPIx , ENABLE);
}

int spiReadWrite(SPI_TypeDef* SPIx, uint8_t *rbuf,
                 const uint8_t *tbuf, int cnt, enum spiSpeed speed)
{
    int i;
    SPIx->CR1 = (SPIx->CR1 & ~SPI_BaudRatePrescaler_256) |
                speeds[speed];

    for(i = 0; i < cnt; i++ )
    {
        if(tbuf)
        {
            SPI_I2S_SendData(SPIx, *tbuf);
        }
        else
        {
            SPI_I2S_SendData(SPIx, 0xff);
        }
        while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
        if(rbuf)
            *rbuf++ = SPI_I2S_ReceiveData(SPIx);
        else
            SPI_I2S_ReceiveData(SPIx);
    }
    return i;
}

int spiReadWrite16(SPI_TypeDef* SPIx, uint16_t *rbuf,
                   const uint16_t *tbuf, int cnt, enum spiSpeed speed)
{
    int i;

    SPI_DataSizeConfig(SPIx, SPI_DataSize_16b);

    SPIx->CR1 = (SPIx->CR1 & ~SPI_BaudRatePrescaler_256) |
                speeds[speed];

    for(i = 0; i < cnt; i++ )
    {
        if(tbuf)
        {
            SPI_I2S_SendData(SPIx, *tbuf);
        }
        else
        {
            SPI_I2S_SendData(SPIx, 0xff);
        }
        while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
        if(rbuf)
            *rbuf++ = SPI_I2S_ReceiveData(SPIx);
        else
            SPI_I2S_ReceiveData(SPIx);
    }
    SPI_DataSizeConfig(SPIx, SPI_DataSize_8b);
    return i;
}

void spi_send(SPI_TypeDef* SPIx, uint8_t data, uint8_t *rbuf,
              enum spiSpeed speed)
{

    SPIx->CR1 = (SPIx->CR1 & ~SPI_BaudRatePrescaler_256) |
                speeds[speed];      //Choose prescaler SPI

    SPIx->DR = data;       //data to transmit
    while (!(SPIx->SR & SPI_SR_TXE));   //waiting for TX to finish
    *rbuf =  SPIx->DR;     //reading data

}
