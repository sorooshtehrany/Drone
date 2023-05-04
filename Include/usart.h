#ifndef USART_H
#define USART_H

//******************************************************************************
//! Library --------------------------------------------------------------------
#include "..\STM8L Library\Include\stm8l15x.h"
//******************************************************************************
//! Define ---------------------------------------------------------------------       
#define USART_RX_GPIO        GPIOC,GPIO_Pin_2    
#define USART_TX_GPIO        GPIOC,GPIO_Pin_3
//******************************************************************************
//! Variable -------------------------------------------------------------------

//******************************************************************************
//! Function -------------------------------------------------------------------
void UsartInit              ( void );
void UsartSendData          ( u8 bData );
void UsartRxIrqHandeler     ( void );
u8   UsartGetRxDataFlag     ( void );
void UsartSetRxDataFlag     ( u8 bRxFlag );
u8   UsartGetRxData         ( u8* bDataArray );
//-----------------
void PrintNum( u16 Num);
void PrintEnter(void);
void PrintLine(void);
void PrintString(u8* Str,u8 len);




//******************************************************************************

#endif // USART_H