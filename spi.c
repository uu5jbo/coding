
#include "spi.h"

static const uint16_t speeds[] =
{
    [SPI_SLOW] = SPI_BaudRatePrescaler_64,
    [SPI_MEDIUM] = SPI_BaudRatePrescaler_8,
    [SPI_FAST] = SPI_BaudRatePrescaler_2
};


void csInit(void)
{
    // SS Chip select
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    GPIOC->CRH |= GPIO_CRH_MODE10;
    GPIOC->CRH &= ~GPIO_CRH_CNF10;   // Output PP 50Mhz
    GPIOC->BSRR |= GPIO_BSRR_BS10; // Set High on PC10
    /*
    	GPIOC->CRL |= GPIO_CRL_MODE6;
        GPIOC->CRL &= ~GPIO_CRL_CNF6;   // Output PP 50Mhz
        GPIOC->BSRR |= GPIO_BSRR_BS6;
    */
}

//============== SPI Init ==================== //
void spiInit(SPI_TypeDef *SPIx)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit (& GPIO_InitStructure);
    SPI_StructInit (& SPI_InitStructure);
    if (SPIx == SPI2)
    {
        RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; // Clock to GPIO Port B
       // RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;  //Clock to SPI2 Module on APB1 bus1

        RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; // Clock to alternative functions
     

        //SCK
        GPIOB->CRH |= GPIO_CRH_MODE13; // Output 50 MHz
        GPIOB->CRH &= ~GPIO_CRH_CNF13;  //Reset both bits
        GPIOB->CRH |= GPIO_CRH_CNF13_1; // PB13 Alternat. func, PP

        //MOSI
        GPIOB->CRH |= GPIO_CRH_MODE15; // Output 50 MHz
        GPIOB->CRH &= ~GPIO_CRH_CNF15;  //Reset both bits
        GPIOB->CRH |= GPIO_CRH_CNF15_1; // PB15 Alternat. func, PP

        //MISO
        GPIOB->CRH &= ~GPIO_CRH_MODE14;
        GPIOB->CRH &= ~GPIO_CRH_CNF14;
        GPIOB->CRH |= GPIO_CRH_CNF14_1;  // PB14 Input Pull up/down
        GPIOB->BSRR = GPIO_BSRR_BS14;    // PB14 Pull Up
        
         RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;  //Clock to SPI2 Module on APB1 bus1

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
// ============== End of Initialization SPI =================== //

int spiReadWrite(SPI_TypeDef* SPIx, uint8_t *rbuf,
                 const uint8_t *tbuf, int cnt, enum spiSpeed speed)
{
    int i;
    SPIx->CR1 = (SPIx->CR1 & ~SPI_BaudRatePrescaler_256) | speeds[speed];

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

void eepromInit(void)
{   
    csInit();   //Enabling SS pin PC10
    
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit (& GPIO_InitStructure);
    SPI_StructInit (& SPI_InitStructure);
   
        RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; // Clock to GPIO Port B
       // RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;  //Clock to SPI2 Module on APB1 bus1

        RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; // Clock to alternative functions
     

        //SCK
        GPIOB->CRH |= GPIO_CRH_MODE13; // Output 50 MHz
        GPIOB->CRH &= ~GPIO_CRH_CNF13;  //Reset both bits
        GPIOB->CRH |= GPIO_CRH_CNF13_1; // PB13 Alternat. func, PP

        //MOSI
        GPIOB->CRH |= GPIO_CRH_MODE15; // Output 50 MHz
        GPIOB->CRH &= ~GPIO_CRH_CNF15;  //Reset both bits
        GPIOB->CRH |= GPIO_CRH_CNF15_1; // PB15 Alternat. func, PP

        //MISO
        GPIOB->CRH &= ~GPIO_CRH_MODE14;
        GPIOB->CRH &= ~GPIO_CRH_CNF14;
        GPIOB->CRH |= GPIO_CRH_CNF14_1;  // PB14 Input Pull up/down
        GPIOB->BSRR = GPIO_BSRR_BS14;    // PB14 Pull Up
        
         RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;  //Clock to SPI2 Module on APB1 bus1
         
        SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
        SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
        SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
        SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
        SPI_InitStructure.SPI_BaudRatePrescaler = speeds[SPI_SLOW ];
        SPI_InitStructure.SPI_CRCPolynomial = 7;
        SPI_Init(SPI2 , &SPI_InitStructure);
        
        SPI_Cmd(SPI2 , ENABLE);
 }


void eepromWriteEnable()
{
    uint8_t cmd = cmdWREN;
    while (WIP(eepromReadStatus() ));
    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, Bit_RESET);
    spiReadWrite(EEPROM_SPI, 0, &cmd, 1, EEPROM_SPEED);
    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, Bit_SET);
}

void eepromWriteDisable(void)
{
    uint8_t cmd = cmdWRDI;
    while (WIP(eepromReadStatus() ));
    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, Bit_RESET);
    spiReadWrite(EEPROM_SPI, 0, &cmd, 1, EEPROM_SPEED);
    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, Bit_SET);
}

uint8_t eepromReadStatus(void)
{
    uint8_t cmd[] = {cmdRDSR, 0xff};
    uint8_t res[2];
    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, Bit_RESET); // Enable spi
    spiReadWrite(EEPROM_SPI, res, cmd, 2, EEPROM_SPEED); 
    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, Bit_SET);     //Disable spi
    return res[1];
}

void eepromWriteStatus(uint8_t status)
{
    uint8_t cmd[] = {cmdWRSR, status};  //Write to status register CMD
    eepromWriteEnable();    //First enable writes
    //Start writing to Status Register
    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, Bit_RESET); // Enable spi transaction
    spiReadWrite(EEPROM_SPI, 0, cmd, 2, EEPROM_SPEED); // Write a command
    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, Bit_SET); //Disable transaction
}
