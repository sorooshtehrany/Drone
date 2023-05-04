// In The Name Of God

//! Library --------------------------------------------------------------------
#include "STM8L Library\Include\stm8l15x.h"
#include "STM8L Library\Include\stm8l15x_conf.h"
// -----------------------------------------------------------------------------
#include "Include\usart.h"
#include "Include\systemTick.h"
#include "Include\time.h"
#include "Include\MRF24J40.h"
#include "Include\delay.h"
#include "Include\key.h"
#include "Include\led.h"
#include "Include\mainStr.h"
#include "Include\AT24C04.h"
#include "Include\MyI2C.h"
#include "Include\myiic.h"
#include "Include\Accelerometer.h"
//******************************************************************************
//! Define ---------------------------------------------------------------------

//******************************************************************************
//! Variable -------------------------------------------------------------------
u8 led=3;
//******************************************************************************
//! Function -------------------------------------------------------------------
void MainClkInit    ( void );
void delay          ( void );
//******************************************************************************
void main( void )
{
  u8 Flag;
  u8 DD;
    //! check read out protection register
    /*
    if( OPT->ROP != 0x00 )
    {
        //! flash data unlock
        FLASH_Unlock( FLASH_MemType_Data );  
        
        //! active read out protection
        FLASH_ProgramOptionByte( 0x4800, 0x00 ); 
        
        //! flash data lock
        FLASH_Lock( FLASH_MemType_Data );
    }
    */

    //! init main clock
    MainClkInit();    
 
    //! system tick init
    SystemTickInit();

    //! usart init
    UsartInit();
    
    //! accelerometer init
      Flag=init();
//    MMA8452Reset(); // Start by resetting sensor device to default settings
//    initMMA8452();  // init the accelerometer if communication is OK
     Transition_WithIntrrupt();    
//    MMA8452Offsets(); // User defined compensation for x/y/z acceleration errors
//      Auto_WakeSleep();
     //! Enable general interrupts       
//      AccInit();
    enableInterrupts();
//     Orientation_Detect();
    Transition_Detect();
    standby();
    while(1)
    {   
     // loop();
//      for(u32 a=0;a<100000;a++);
//      Flag=readRegister(0X16,&DD);
//     portraitLandscapeHandler();
//      motionDetect();      
     //tapHandler();
     // Transition_Detect();
    }
}
//******************************************************************************
//! \brief  Configure System Clock 
//! \param  None
//! \retval None
//******************************************************************************
void MainClkInit( void )
{
  //! Select HSE as system clock source 
  CLK_SYSCLKSourceSwitchCmd(ENABLE);
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
  //! system clock prescaler: 1
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
  while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSI);
}
//******************************************************************************
void delay          ( void )
{
  for(u32 i=0;i<100000;i++);
}
//******************************************************************************
//! \brief  Reports the name of the source file and the source line number
//!   where the assert_param error has occurred.
//! \param  file: pointer to the source file name
//! \param  line: assert_param error line source number
//******************************************************************************
#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
    
  }
}
#endif