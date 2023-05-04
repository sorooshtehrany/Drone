#ifndef KEY_H
#define KEY_H

//******************************************************************************
//! Define ---------------------------------------------------------------------
#include "..\STM8L Library\Include\stm8l15x.h"

//******************************************************************************
//! Define ---------------------------------------------------------------------         
#define KEY_1_GPIO          GPIOD,GPIO_Pin_4 
#define KEY_2_GPIO          GPIOD,GPIO_Pin_5
#define KEY_3_GPIO          GPIOD,GPIO_Pin_6

#define KEY_GPIO_MODE       GPIO_Mode_In_FL_No_IT
//******************************************************************************
//! Variable -------------------------------------------------------------------

//******************************************************************************
//! Function -------------------------------------------------------------------
void KeyInit    ( void );
u8   KeyStatus  ( void );
//******************************************************************************

#endif // KEY_H;