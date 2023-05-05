#include "..\Include\led.h"

//*******************************************************************************
//! Variable -------------------------------------------------------------------
u8  bLedStatus=0;
u32 iLedTickBuffer=0;
u8  bLedMode=0;
//******************************************************************************
//! \brief  init led gpio
//******************************************************************************
void LedInit( void )
{
    //! led gpio init
    GPIO_Init( LED_1_GPIO, LED_GPIO_MODE );
    GPIO_Init( LED_2_GPIO, LED_GPIO_MODE );
    GPIO_Init( LED_3_GPIO, LED_GPIO_MODE );
}
//******************************************************************************
//! \brief  led main
//******************************************************************************
void LedMain( void )
{
    if( bLedMode==0 )
    {
        if( SystemTickGetMsecond()-iLedTickBuffer>LED_BLINK_OFF_TIME )
        {
            iLedTickBuffer=SystemTickGetMsecond();
            
            bLedMode=1;
            
            if( bLedStatus & 0x01 )
                GPIO_SetBits( LED_1_GPIO );
            if( bLedStatus & 0x02 )
                GPIO_SetBits( LED_2_GPIO );
            if( bLedStatus & 0x04 )
                GPIO_SetBits( LED_3_GPIO );
        }
    }
    else
    {
        if( SystemTickGetMsecond()-iLedTickBuffer>LED_BLINK_ON_TIME )
        {
            iLedTickBuffer=SystemTickGetMsecond();
            
            bLedMode=0;
            
            if( bLedStatus & 0x01 )
                GPIO_ResetBits( LED_1_GPIO );
            if( bLedStatus & 0x02 )
                GPIO_ResetBits( LED_2_GPIO );
            if( bLedStatus & 0x04 )
                GPIO_ResetBits( LED_3_GPIO );
        }
    }
}
//******************************************************************************
//! \brief  set led status
//! \param  bLedNumber : number off led
//! \param  bLedStatus : led staus for set
//******************************************************************************
void LedSet( u8 bNumber, u8 bStatus )
{
    if( bStatus==0 )
        bLedStatus &= ~(1<<bNumber-1);
    else 
        bLedStatus |=  (1<<bNumber-1);
}