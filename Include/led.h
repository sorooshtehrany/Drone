#ifndef LED_H
#define LED_H

//******************************************************************************
//! Library --------------------------------------------------------------------
#include "..\STM8L Library\Include\stm8l15x.h"
#include "systemTick.h"

//******************************************************************************
//! Define ---------------------------------------------------------------------
#define LED_BLINK_ON_TIME       100     //! on time unit mS
#define LED_BLINK_OFF_TIME      1000    //! off time unit mS


#define LED_PORT            GPIOD
        
#define LED_1_GPIO          GPIOD,GPIO_Pin_1 
#define LED_2_GPIO          GPIOD,GPIO_Pin_2
#define LED_3_GPIO          GPIOD,GPIO_Pin_3

#define LED_GPIO_MODE       GPIO_Mode_Out_PP_Low_Fast
//******************************************************************************
//! Variable -------------------------------------------------------------------

//******************************************************************************
//! Function -------------------------------------------------------------------
void LedInit    ( void );
void LedMain    ( void );
void LedSet     ( u8 bNumber, u8 bStatus );
//******************************************************************************

#endif // LED_H;