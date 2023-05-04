#ifndef AT24C00_H
#define AT24C00_H

#include "..\STM8L Library\Include\stm8l15x.h"
//******************************************************************************
//! Define ---------------------------------------------------------------------
#define sEE_I2C                          I2C1
#define sEE_I2C_CLK                      CLK_Peripheral_I2C1
//!Pin config--------------------------------------------------------------------
#define sEE_I2C_SCL_PIN                  GPIO_Pin_1                  /* PC.01 */
#define sEE_I2C_SCL_GPIO_PORT            GPIOC                       /* GPIOC */
#define sEE_I2C_SDA_PIN                  GPIO_Pin_0                  /* PC.00 */
#define sEE_I2C_SDA_GPIO_PORT            GPIOC                       /* GPIOC */

#define sEE_M24C64_32

#define sEE_I2C_DMA                      DMA1
#define sEE_I2C_DMA_CHANNEL_TX           DMA1_Channel3
#define sEE_I2C_DMA_CHANNEL_RX           DMA1_Channel0
#define sEE_I2C_DMA_FLAG_TX_TC           DMA1_FLAG_TC3
#define sEE_I2C_DMA_FLAG_RX_TC           DMA1_FLAG_TC0
#define sEE_I2C_DR_Address               ((u16)0x005216)
#define sEE_USE_DMA

#define sEE_DIRECTION_TX                 0
#define sEE_DIRECTION_RX                 1


//! For M24C32 and M24C64 devices, E0,E1 and E2 pins are all used for device 
//!  address selection (ne need for additional address lines). According to the 
//!   Harware connection on the board (on STM810C-EVAL board E0 = E1 = E2 = 0) 
#define sEE_HW_ADDRESS      0xA0   /* E0 = E1 = E2 = 0 */ 


#define I2C_SPEED           200000
             
#define I2C_SLAVE_ADDRESS7  0xA0



#define sEE_PAGESIZE        32

//! Defintions for the state of the DMA transfer   
#define sEE_STATE_READY     0
#define sEE_STATE_BUSY      1
   
//! Maximum timeout value for counting before exiting waiting loop on DMA 
//!  Trasnfer Complete. This value depends directly on the maximum page size and
//!   the sytem clock frequency. 
#define sEE_TIMEOUT_MAX     0x10000
//******************************************************************************
//! Variable -------------------------------------------------------------------

//******************************************************************************
//! Function -------------------------------------------------------------------
void sEE_DeInit                 ( void );
void sEE_Init                   ( void );
void sEE_WriteByte              ( u8* pBuffer, u16 WriteAddr );
void sEE_WritePage              ( u8* pBuffer, u16 WriteAddr, u8* NumByteToWrite );
void sEE_WriteBuffer            ( u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite );
void sEE_ReadBuffer             ( u8* pBuffer, u16 ReadAddr, u16* NumByteToRead );
void sEE_WaitEepromStandbyState ( void );
void sEE_I2C_DMA_RX_IRQHandler  ( void );
void sEE_I2C_DMA_TX_IRQHandler  ( void );
//******************************************************************************

#endif // AT24C00_H