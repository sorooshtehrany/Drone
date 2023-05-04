#ifndef MYIIC_H
#define MYIIC_H

#include "..\STM8L Library\Include\stm8l15x.h"
//******************************************************************************
//! Define ---------------------------------------------------------------------
//!Pin config--------------------------------------------------------------------
#define SCL_PIN                  GPIO_Pin_1                  /* PC.01 */
#define SCL_GPIO_PORT            GPIOC                       /* GPIOC */
#define SDA_PIN                  GPIO_Pin_0                  /* PC.00 */
#define SDA_GPIO_PORT            GPIOC                       /* GPIOC */

#define Write_Mode               0
#define Read_Mode                1  

//******************************************************************************
//! Variable -------------------------------------------------------------------

//******************************************************************************
//! Function -------------------------------------------------------------------
void          MyIIC_Init           (void);
void          MyIIC_GenerateStart  (void);
unsigned char MyIIC_Read           (unsigned char Data);
void          MyIIC_Write          (unsigned char Data);
void          MyIIC_SlaveAddress   (unsigned char Address,unsigned char Mode);  
void          Delay                (void);





//******************************************************************************

#endif // MYIIC_H