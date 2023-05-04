#include "../Include/AT24C04.h"

//******************************************************************************
//! Variable -------------------------------------------------------------------
//__IO u16 sEEAddress = 0;

__IO u16 sEEAddress = 0X1C;
__IO u32 sEETimeout = sEE_TIMEOUT_MAX;
__IO u16* sEEDataReadPointer;
__IO u8* sEEDataWritePointer;
__IO u8 sEEDataNum;

//******************************************************************************
//! \brief  DeInitializes peripherals used by the I2C EEPROM driver.
//******************************************************************************
void sEE_DeInit(void)
{
    //! sEE_I2C Peripheral Disable 
    I2C_Cmd(sEE_I2C, DISABLE);
    
    //! sEE_I2C DeInit 
    I2C_DeInit(sEE_I2C);
    
    //!< sEE_I2C Periph clock disable 
    CLK_PeripheralClockConfig(sEE_I2C_CLK, DISABLE);
    
    //!< GPIO configuration 
    //!< Configure sEE_I2C pins: SCL
    GPIO_Init(sEE_I2C_SCL_GPIO_PORT, sEE_I2C_SCL_PIN, GPIO_Mode_In_PU_No_IT);
    
    //!< Configure sEE_I2C pins: SDA 
    GPIO_Init(sEE_I2C_SDA_GPIO_PORT, sEE_I2C_SDA_PIN, GPIO_Mode_In_PU_No_IT);
    
    //! Disable and Deinitialize the DMA channels 
    DMA_Cmd(sEE_I2C_DMA_CHANNEL_TX, DISABLE);
    DMA_Cmd(sEE_I2C_DMA_CHANNEL_RX, DISABLE);
    DMA_DeInit(sEE_I2C_DMA_CHANNEL_TX);
    DMA_DeInit(sEE_I2C_DMA_CHANNEL_RX);
}

//******************************************************************************
//! \brief  Initializes peripherals used by the I2C EEPROM driver.
//******************************************************************************
void sEE_Init(void)
{
    //!< sEE_I2C Periph clock enable 
    CLK_PeripheralClockConfig(sEE_I2C_CLK, ENABLE);
    
    //!< Enable the DMA clock 
    CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE);
    
    //! I2C TX DMA Channel configuration 
    DMA_DeInit(sEE_I2C_DMA_CHANNEL_TX);
    DMA_Init(sEE_I2C_DMA_CHANNEL_TX,
             0, //! This parameter will be configured durig communication 
             sEE_I2C_DR_Address,
             0xFF, //! This parameter will be configured durig communication 
             DMA_DIR_PeripheralToMemory,//! This parameter will be configured durig communication 
             DMA_Mode_Normal,
             DMA_MemoryIncMode_Inc,
             DMA_Priority_VeryHigh,
             DMA_MemoryDataSize_Byte);
    
    //! I2C RX DMA Channel configuration 
    DMA_DeInit(sEE_I2C_DMA_CHANNEL_RX);
    DMA_Init(sEE_I2C_DMA_CHANNEL_RX, 0, //! This parameter will be configured durig communication 
             sEE_I2C_DR_Address,
             0xFF, //! This parameter will be configured durig communication 
             DMA_DIR_PeripheralToMemory,//! This parameter will be configured durig communication 
             DMA_Mode_Normal,
             DMA_MemoryIncMode_Inc,
             DMA_Priority_VeryHigh,
             DMA_MemoryDataSize_Byte);
    
    //!< I2C configuration 
    //! sEE_I2C Peripheral Enable 
    I2C_Cmd(sEE_I2C, ENABLE);
    //! sEE_I2C configuration after enabling it 
    I2C_Init(sEE_I2C, I2C_SPEED, I2C_SLAVE_ADDRESS7, I2C_Mode_I2C, I2C_DutyCycle_2,
             I2C_Ack_Enable, I2C_AcknowledgedAddress_7bit);
    
    //! Enable the sEE_I2C peripheral DMA requests 
    I2C_DMACmd(sEE_I2C, ENABLE);
    
    //!< Select the EEPROM address according to the state of E0, E1, E2 pins 
    sEEAddress = sEE_HW_ADDRESS;    

    //! SR2 register flags 
    I2C_ClearFlag( sEE_I2C, I2C_FLAG_SMBALERT );   
    I2C_ClearFlag( sEE_I2C, I2C_FLAG_TIMEOUT );     
    I2C_ClearFlag( sEE_I2C, I2C_FLAG_WUFH );       
    I2C_ClearFlag( sEE_I2C, I2C_FLAG_PECERR );     
    I2C_ClearFlag( sEE_I2C, I2C_FLAG_OVR );        
    I2C_ClearFlag( sEE_I2C, I2C_FLAG_AF );         
    I2C_ClearFlag( sEE_I2C, I2C_FLAG_ARLO );       
    I2C_ClearFlag( sEE_I2C, I2C_FLAG_BERR );      

}

//******************************************************************************
//! \brief  Writes one byte to the I2C EEPROM.
//! \param  pBuffer : pointer to the buffer  containing the data to be written to the EEPROM.
//! \param  WriteAddr : EEPROM's internal address to write to.
//******************************************************************************
void sEE_WriteByte(u8* pBuffer, u16 WriteAddr)
{
  u8 e;
    //!< Send STRAT condition 
    I2C_GenerateSTART(sEE_I2C, ENABLE);
    
    //!< Test on EV5 and clear it 
    while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_MODE_SELECT))
    {}
    
    //!< Send EEPROM address for write 
   I2C_Send7bitAddress(sEE_I2C, sEEAddress, I2C_Direction_Transmitter);
    
    //!< Test on EV6 and clear it 
    while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {}
    
    
    /*
    for(u16 a=0;a<256;a++)
       {
          I2C_Send7bitAddress(sEE_I2C, a, I2C_Direction_Transmitter);
         for(u16 b=0;b<200;b++)
          if(I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
            {
              e=1;       
            }
       }
*/
    //!< Send the EEPROM's internal address to write to : MSB of the address first 
    I2C_SendData(sEE_I2C, (u8)((WriteAddr & 0xFF00) >> 8));
    
    //!< Test on EV8 and clear it 
    while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {}
    
    //!< Send the EEPROM's internal address to write to : LSB of the address 
    I2C_SendData(sEE_I2C, (u8)(WriteAddr & 0x00FF));
    
    //!< Test on EV8 and clear it 
    while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {}
    
    //!< Send the byte to be written 
    I2C_SendData(sEE_I2C, *pBuffer);
    
    //!< Test on EV8 and clear it 
    while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {}
    
    //!< Send STOP condition 
    I2C_GenerateSTOP(sEE_I2C, ENABLE);
}

//******************************************************************************
//! \brief  Reads a block of data from the EEPROM.
//! \param  pBuffer : pointer to the buffer that receives the data read from the EEPROM.
//! \param  ReadAddr : EEPROM's internal address to read from.
//! \param  NumByteToRead : pointer to the variable holding number of bytes to read from the EEPROM.

//! \note The variable pointed by NumByteToRead is reset to 0 when all the
//!   data are read from the EEPROM. Application should monitor this
//!    variable in order know when the transfer is complete.

//! \note When number of data to be read is higher than 1, this function just
//!   configure the communication and enable the DMA channel to transfer data.
//!    Meanwhile, the user application may perform other tasks.
//!     When number of data to be read is 1, then the DMA is not used.
//******************************************************************************
void sEE_ReadBuffer(u8* pBuffer, u16 ReadAddr, u16* NumByteToRead)
{
    __IO u32 timeout = 0xFFFF;
    
    //!< Wait the end of last communication 
    for (;timeout > 0; timeout--);
    
    //! Set the pointer to the Number of data to be read. This pointer will be used
    //!  by the DMA Transfer Complete interrupt Handler in order to reset the 
    //!   variable to 0. User should check on this variable in order to know if the 
    //!    DMA transfer has been completed or not. 
    sEEDataReadPointer = NumByteToRead;
    
    //!< While the bus is busy 
    while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BUSY))
    {}
    
    //!< Send START condition 
    I2C_GenerateSTART(sEE_I2C, ENABLE);
    
    //!< Test on EV5 and clear it 
    while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_MODE_SELECT))
    {}
    
    //!< Send EEPROM address for write 
    I2C_Send7bitAddress(sEE_I2C, (u8)sEEAddress, I2C_Direction_Transmitter);
    
    //!< Test on EV6 and clear it 
    while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {}
    
    //!< Send the EEPROM's internal address to read from: MSB of the address first 
    I2C_SendData(sEE_I2C, (u8)((ReadAddr & 0xFF00) >> 8));
    
    //!< Test on EV8 and clear it 
    while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {}
    
    //!< Send the EEPROM's internal address to read from: LSB of the address 
    I2C_SendData(sEE_I2C, (u8)(ReadAddr & 0x00FF));
    
    //!< Test on EV8 and clear it 
    while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {}
    
    //!< Send STRAT condition a second time 
    I2C_GenerateSTART(sEE_I2C, ENABLE);
    
    //!< Test on EV5 and clear it 
    while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_MODE_SELECT))
    {}
    
    //!< Send EEPROM address for read 
    I2C_Send7bitAddress(sEE_I2C, (u8)sEEAddress, I2C_Direction_Receiver);
    
    //!< Test on EV6 and clear it 
    while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {}
    
    //! If number of data to be read is 1, then DMA couldn't be used 
    if ((u16)(*NumByteToRead) < 2)
    {
        //!< Disable Acknowledgement 
        I2C_AcknowledgeConfig(sEE_I2C, DISABLE);
        
        //!< Send STOP Condition 
        I2C_GenerateSTOP(sEE_I2C, ENABLE);
        
        //!< Test on EV7 and clear it 
        while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))
        {}
        
        //!< Read a byte from the EEPROM 
        *pBuffer = I2C_ReceiveData(sEE_I2C);
        
        //!< Decrement the read bytes counter 
        (u16)(*NumByteToRead)--;
        
        //!< Enable Acknowledgement to be ready for another reception 
        I2C_AcknowledgeConfig(sEE_I2C, ENABLE);
    }
    //! DMA could be used for number of data higher than 1 
    else
    {
        //! Configure the DMA Rx Channel with the buffer address and the buffer size 
        DMA_Init(sEE_I2C_DMA_CHANNEL_RX, (u16)pBuffer, sEE_I2C_DR_Address, (u8)(*NumByteToRead),
                 DMA_DIR_PeripheralToMemory, DMA_Mode_Normal, DMA_MemoryIncMode_Inc,
                 DMA_Priority_VeryHigh, DMA_MemoryDataSize_Byte);
        
        //! Enable the DMA Channels Interrupts 
        DMA_ITConfig(sEE_I2C_DMA_CHANNEL_TX, DMA_ITx_TC, ENABLE);
        DMA_ITConfig(sEE_I2C_DMA_CHANNEL_RX, DMA_ITx_TC, ENABLE);
        
        //! Inform the DMA that the next End Of Transfer Signal will be the last one 
        I2C_DMALastTransferCmd(sEE_I2C, ENABLE);
        
        //! Enable the DMA Rx Channel 
        DMA_Cmd(sEE_I2C_DMA_CHANNEL_RX, ENABLE);
        
        //! Global DMA Enable 
        DMA_GlobalCmd(ENABLE);
    }
}
//******************************************************************************
//! \brief  Writes buffer of data to the I2C EEPROM.
//! \param  pBuffer : pointer to the buffer  containing the data to be written to the EEPROM.
//! \param  WriteAddr : EEPROM's internal address to write to.
//! \param  NumByteToWrite : number of bytes to write to the EEPROM.
//******************************************************************************
void sEE_WriteBuffer( u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite )
{
    u8  NumOfPage=0; 
    u8  NumOfSingle=0; 
    u8  count=0;
    u16 Addr=0;
    
    Addr = WriteAddr % sEE_PAGESIZE;
    count = (u8)(sEE_PAGESIZE - (u16)Addr);
    NumOfPage =  (u8)(NumByteToWrite / sEE_PAGESIZE);
    NumOfSingle = (u8)(NumByteToWrite % sEE_PAGESIZE);
    
    //!< If WriteAddr is sEE_PAGESIZE aligned  
    if (Addr == 0)
    {
        //!< If NumByteToWrite < sEE_PAGESIZE 
        if (NumOfPage == 0)
        {
            //! Store the number of data to be written 
            sEEDataNum = NumOfSingle;
            //! Start writing data 
            sEE_WritePage(pBuffer, WriteAddr, (u8*)(&sEEDataNum));
            //! Wait transfer through DMA to be complete 
            sEETimeout = sEE_TIMEOUT_MAX;
            while ((sEEDataNum > 0) && (sEETimeout-- > 0))
            {}
            sEE_WaitEepromStandbyState();
        }
        //!< If NumByteToWrite > sEE_PAGESIZE 
        else
        {
            while (NumOfPage--)
            {
                //! Store the number of data to be written 
                sEEDataNum = sEE_PAGESIZE;
                sEE_WritePage(pBuffer, WriteAddr, (u8*)(&sEEDataNum));
                //! Wait transfer through DMA to be complete 
                sEETimeout = sEE_TIMEOUT_MAX;
                while ((sEEDataNum > 0) && (sEETimeout-- > 0))
                {}
                sEE_WaitEepromStandbyState();
                WriteAddr +=  sEE_PAGESIZE;
                pBuffer += sEE_PAGESIZE;
            }
            
            if ( NumOfSingle!=0 )
            {
                //! Store the number of data to be written 
                sEEDataNum = NumOfSingle;
                sEE_WritePage(pBuffer, WriteAddr, (u8*)(&sEEDataNum));
                //! Wait transfer through DMA to be complete 
                sEETimeout = sEE_TIMEOUT_MAX;
                while ((sEEDataNum > 0) && (sEETimeout-- > 0))
                {}
                sEE_WaitEepromStandbyState();
            }
        }
    }
    //!< If WriteAddr is not sEE_PAGESIZE aligned  
    else
    {
        //!< If NumByteToWrite < sEE_PAGESIZE 
        if (NumOfPage == 0)
        {
            //!< If the number of data to be written is more than the remaining space in the current page: 
                if (NumByteToWrite > count)
                {
                    //! Store the number of data to be written 
                    sEEDataNum = count;
                    //!< Write the data conained in same page 
                    sEE_WritePage(pBuffer, WriteAddr, (u8*)(&sEEDataNum));
                    //! Wait transfer through DMA to be complete 
                    sEETimeout = sEE_TIMEOUT_MAX;
                    while ((sEEDataNum > 0) && (sEETimeout-- > 0))
                    {}
                    sEE_WaitEepromStandbyState();
                    
                    //! Store the number of data to be written 
                    sEEDataNum = (u8)(NumByteToWrite - count);
                    //!< Write the remaining data in the following page 
                    sEE_WritePage((u8*)(pBuffer + count), (WriteAddr + count), (u8*)(&sEEDataNum));
                    //! Wait transfer through DMA to be complete 
                    sEETimeout = sEE_TIMEOUT_MAX;
                    while ((sEEDataNum > 0) && (sEETimeout-- > 0))
                    {}
                    sEE_WaitEepromStandbyState();
                }
                else
                {
                    //! Store the number of data to be written 
                    sEEDataNum = NumOfSingle;
                    sEE_WritePage(pBuffer, WriteAddr, (u8*)(&sEEDataNum));
                    //! Wait transfer through DMA to be complete 
                    sEETimeout = sEE_TIMEOUT_MAX;
                    while ((sEEDataNum > 0) && (sEETimeout-- > 0))
                    {}
                    sEE_WaitEepromStandbyState();
                }
        }
        //!< If NumByteToWrite > sEE_PAGESIZE 
        else
        {
            NumByteToWrite -= count;
            NumOfPage = (u8)(NumByteToWrite / sEE_PAGESIZE);
            NumOfSingle = (u8)(NumByteToWrite % sEE_PAGESIZE);
            
            if (count != 0)
            {
                //! Store the number of data to be written 
                sEEDataNum = count;
                sEE_WritePage(pBuffer, WriteAddr, (u8*)(&sEEDataNum));
                //! Wait transfer through DMA to be complete 
                sEETimeout = sEE_TIMEOUT_MAX;
                while ((sEEDataNum > 0) && (sEETimeout-- > 0))
                {}
                sEE_WaitEepromStandbyState();
                WriteAddr += count;
                pBuffer += count;
            }
            
            while (NumOfPage--)
            {
                //! Store the number of data to be written 
                sEEDataNum = sEE_PAGESIZE;
                sEE_WritePage(pBuffer, WriteAddr, (u8*)(&sEEDataNum));
                //! Wait transfer through DMA to be complete 
                sEETimeout = sEE_TIMEOUT_MAX;
                while ((sEEDataNum > 0) && (sEETimeout-- > 0))
                {}
                sEE_WaitEepromStandbyState();
                WriteAddr +=  sEE_PAGESIZE;
                pBuffer += sEE_PAGESIZE;
            }
            if (NumOfSingle != 0)
            {
                //! Store the number of data to be written 
                sEEDataNum = NumOfSingle;
                sEE_WritePage(pBuffer, WriteAddr, (u8*)(&sEEDataNum));
                //! Wait transfer through DMA to be complete 
                sEETimeout = sEE_TIMEOUT_MAX;
                while ((sEEDataNum > 0) && (sEETimeout-- > 0))
                {}
                sEE_WaitEepromStandbyState();
            }
        }
    }
}

//******************************************************************************
//! \brief  Writes more than one byte to the EEPROM with a single WRITE cycle.
//! \note   The number of byte can't exceed the EEPROM page size.
//! \param  pBuffer : pointer to the buffer containing the data to be written to the EEPROM.
//! \param  WriteAddr : EEPROM's internal address to write to.
//! \param  NumByteToWrite : pointer to the variable holding number of bytes to written to the EEPROM.

//! \note The variable pointed by NumByteToWrite is reset to 0 when all the
//!   data are read from the EEPROM. Application should monitor this
//!    variable in order know when the transfer is complete.

//! \note When number of data to be written is higher than 1, this function just
//!   configure the communication and enable the DMA channel to transfer data.
//!    Meanwhile, the user application may perform other tasks.
//!     When number of data to be written is 1, then the DMA is not used.
//******************************************************************************
void sEE_WritePage(u8* pBuffer, u16 WriteAddr, u8* NumByteToWrite)
{
    __IO u32 timeout = 0xFFFF;
    
    //!< Wait the end of last communication 
    for (;timeout > 0; timeout--);
    
    //! Set the pointer to the Number of data to be written. This pointer will be used
    //!  by the DMA Transfer Completer interrupt Handler in order to reset the 
    //!   variable to 0. User should check on this variable in order to know if the 
    //!    DMA transfer has been complete or not. 
    sEEDataWritePointer = NumByteToWrite;
    
    //!< While the bus is busy 
    while (I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BUSY))
    {}
    
    //!< Send START condition 
    I2C_GenerateSTART(sEE_I2C, ENABLE);
    
    //!< Test on EV5 and clear it 
    while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_MODE_SELECT))
    {}
    
    //!< Send EEPROM address for write 
    I2C_Send7bitAddress(sEE_I2C, (u8)sEEAddress, I2C_Direction_Transmitter);
    
    //!< Test on EV6 and clear it 
    while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {}
    
    //!< Send the EEPROM's internal address to write to : MSB of the address first */
    I2C_SendData(sEE_I2C, (u8)((WriteAddr & 0xFF00) >> 8));
    
    //!< Test on EV8 and clear it */
    while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {}
    
    //!< Send the EEPROM's internal address to write to : LSB of the address */
    I2C_SendData(sEE_I2C, (u8)(WriteAddr & 0x00FF));
    
    //!< Test on EV8 and clear it */
    while (! I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {}
    
    //! If number of data to be written is 1, then DMA couldn't be used 
    if ((u16)(*NumByteToWrite) < 2)
    {
        //!< Send the byte to be written 
        I2C_SendData(sEE_I2C, *pBuffer);
        
        //!< Test on EV8 and clear it 
        while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {}
        
        //!< Send STOP condition 
        I2C_GenerateSTOP(sEE_I2C, ENABLE);
        
        (u8)(*NumByteToWrite)--;
    }
    //! DMA could be used for number of data higher than 1 
    else
    {
        //! Configure the DMA Tx Channel with the buffer address and the buffer size 
        DMA_Init(sEE_I2C_DMA_CHANNEL_TX, (u16)pBuffer, sEE_I2C_DR_Address, (u8)(*NumByteToWrite),
                 DMA_DIR_MemoryToPeripheral, DMA_Mode_Normal, DMA_MemoryIncMode_Inc,
                 DMA_Priority_VeryHigh, DMA_MemoryDataSize_Byte);
        
        //! Enable the DMA Channels Interrupts 
        DMA_ITConfig(sEE_I2C_DMA_CHANNEL_TX, DMA_ITx_TC, ENABLE);
        DMA_ITConfig(sEE_I2C_DMA_CHANNEL_RX, DMA_ITx_TC, ENABLE);
     
        //! Enable the DMA Tx Channel 
        DMA_Cmd(sEE_I2C_DMA_CHANNEL_TX, ENABLE);
        
        //! Global DMA Enable 
        DMA_GlobalCmd(ENABLE);
    }
}
//******************************************************************************
//! \brief  Wait for EEPROM Standby state
//******************************************************************************
void sEE_WaitEepromStandbyState( void )
{
    __IO u8 tempreg = 0;
    __IO u32 timeout = 0xFFFF;
    
    do
    {
        //!< Send START condition 
        I2C_GenerateSTART(sEE_I2C, ENABLE);
        
        //! Test on EEPROM_I2C EV5 and clear it 
        while (!I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_SB))  /* EV5 */
        {
        }
        
        //!< Send EEPROM address for write 
        I2C_Send7bitAddress(sEE_I2C, (u8)sEEAddress, I2C_Direction_Transmitter);
        
        //!< Wait for address aknowledgement 
        for (;timeout > 0; timeout--);
        
        //!< Read sEE SR1 register to clear pending flags 
        tempreg = I2C_ReadRegister(sEE_I2C, I2C_Register_SR1);
        
    }
    while (!(tempreg & 0x02));
    
    //!< Clear AF flag 
    I2C_ClearFlag(sEE_I2C, I2C_FLAG_AF);
    
    //!< STOP condition 
    I2C_GenerateSTOP(sEE_I2C, ENABLE);
}
//******************************************************************************
//! \brief  This function handles the DMA Tx Channel interrupt Handler.
//! \note This function should be called in the
//!   DMA1_CHANNEL2_3_IRQHandler in the stm8l15x_it.c file.

//!         INTERRUPT_HANDLER(DMA1_CHANNEL2_3_IRQHandler, 3)
//!         {
//!             sEE_I2C_DMA_TX_IRQHandler();
//!         }
//******************************************************************************
void sEE_I2C_DMA_TX_IRQHandler(void)
{
    //! Check if the DMA transfer is complete 
    if (DMA_GetFlagStatus(sEE_I2C_DMA_FLAG_TX_TC) != RESET)
    {
        //! Disable the DMA Tx Channel and Clear all its Flags 
        DMA_Cmd(sEE_I2C_DMA_CHANNEL_TX, DISABLE);
        DMA_ClearFlag(sEE_I2C_DMA_FLAG_TX_TC);
        
        //!< Wait till all data have been physically transferred on the bus 
        while (!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {}
        
        //!< Send STOP condition 
        I2C_GenerateSTOP(sEE_I2C, ENABLE);
        
        //! Reset the variable holding the number of data to be written 
        *sEEDataWritePointer = 0;
    }
}

//******************************************************************************
//! \brief  This function handles the DMA Rx Channel interrupt Handler.
//! \note This function should be called in the
//!   DMA1_CHANNEL0_1_IRQHandler in the stm8l15x_it.c file just as follow.

//!         INTERRUPT_HANDLER(DMA1_CHANNEL0_1_IRQHandler, 2)
//!         {
//!             sEE_I2C_DMA_RX_IRQHandler();
//!         }
//******************************************************************************
void sEE_I2C_DMA_RX_IRQHandler(void)
{
    //! Check if the DMA transfer is complete 
    if (DMA_GetFlagStatus(sEE_I2C_DMA_FLAG_RX_TC) != RESET)
    {
        //!< Send STOP Condition 
        I2C_GenerateSTOP(sEE_I2C, ENABLE);
        
        //! Disable the DMA Rx Channel and Clear all its Flags 
        DMA_Cmd(sEE_I2C_DMA_CHANNEL_RX, DISABLE);
        DMA_ClearFlag(sEE_I2C_DMA_FLAG_RX_TC);
        
        //! Reset the variable holding the number of data to be read 
        *sEEDataReadPointer = 0;
    }
}
