
#include "spi.h"

static const uint16_t speeds [] =
{
    [SPI_SLOW] = SPI_BaudRatePrescaler_64 ,
    [SPI_MEDIUM] = SPI_BaudRatePrescaler_8 ,
    [SPI_FAST] = SPI_BaudRatePrescaler_2
};

void csInit(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    GPIOC->CRL |= GPIO_CRL_MODE3;
    GPIOC->CRL &= ~GPIO_CRL_CNF3;   // Output PP 50Mhz

    GPIOC->BSRR |= GPIO_BSRR_BS3; // Set High on PC3
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

        GPIOB->CRH |= GPIO_CRH_MODE13; // Output 50 MHz
        GPIOB->CRH |= GPIO_CRH_CNF13_1;
        GPIOB->CRH &= ~GPIO_CRH_CNF13_0; // PB13 Alternat. func, PP

        GPIOB->CRH |= GPIO_CRH_MODE15; // Output 50 MHz
        GPIOB->CRH |= GPIO_CRH_CNF15_1;
        GPIOB->CRH &= ~GPIO_CRH_CNF15_0; // PB15 Alternat. func, PP

        GPIOB->CRH &= ~GPIO_CRH_CNF14;
        GPIOB->CRH |= GPIO_CRH_CNF14_1; // PB14 Input Pull up/down
        GPIOB->ODR |= GPIO_ODR_ODR14; // PB14 Pull Up

                /* Enable clocks , configure pins
        ...
        */
    }
    else
    {
        return;
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

