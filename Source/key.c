#include "..\Include\key.h"
//******************************************************************************
//! Variable -------------------------------------------------------------------

//******************************************************************************
//! \brief  configure keyboard gpio
//******************************************************************************
void KeyInit( void )
{
    //! init key gpio
    GPIO_Init( KEY_1_GPIO, KEY_GPIO_MODE );
    GPIO_Init( KEY_2_GPIO, KEY_GPIO_MODE );
    GPIO_Init( KEY_3_GPIO, KEY_GPIO_MODE );    
}
//******************************************************************************
//! \brief  check key status
//! \retval key status
//******************************************************************************
u8   KeyStatus  ( void )
{
    if( GPIO_ReadInputDataBit(KEY_1_GPIO)==0 )
    {
        return 1;
    }
    else if( GPIO_ReadInputDataBit(KEY_2_GPIO)==0 )
    {
        return 2;
    }
    else if( GPIO_ReadInputDataBit(KEY_3_GPIO)==0 )
    {
        return 3;
    }
    
    return 0;
}