#include "..\Include\systemTick.h"

//******************************************************************************
//! Variable -------------------------------------------------------------------
u32  iSystemTickMsCounter=0;
u32  iSystemTickSCounter=0;
//******************************************************************************
//! \brief  Configure System Tick by 1khz timer interrupt
//******************************************************************************
void SystemTickInit( void )
{
    //! Enable TIM1 clock 
    CLK_PeripheralClockConfig( CLK_Peripheral_TIM1, ENABLE );
    //! TIM1 Deinit
    TIM1_DeInit();
    //! Time base configuration ( 1Khz )
    TIM1_TimeBaseInit( 15999, TIM1_CounterMode_Up, 1000, 0 );
    //! Clear TIM1 update flag 
    TIM1_ClearFlag(TIM1_FLAG_Update);
    //! Enable update interrupt 
    TIM1_ITConfig(TIM1_IT_Update, ENABLE);
    //! Enable TIM4 
    TIM1_Cmd(ENABLE);
}
//******************************************************************************
//! \brief  timer interrupt Handeler
//******************************************************************************
void SystemTickIrqHandeler( void )
{
    iSystemTickSCounter++;   
}
//******************************************************************************
//! \brief  get system tick Msecond counter value
//! \retval system tick counter value
//******************************************************************************
u32 SystemTickGetMsecond( void )
{
    iSystemTickMsCounter=iSystemTickSCounter*1000+TIM1_GetCounter();
    return iSystemTickMsCounter;
}
//******************************************************************************
//! \brief  get system tick second counter value
//! \retval system tick counter value
//******************************************************************************
u32 SystemTickGetSecond( void )
{
    return iSystemTickSCounter;
}