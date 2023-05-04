#include "..\Include\usart.h"

//******************************************************************************
//! Variable -------------------------------------------------------------------
u8 bUsartRxData[50];
u8 bUsartRxDataFlag=0;
u8 bUsartCommaCounter=0;
u8 bUsartDataRxCounter=0;
u8 bUsartDataRxLen=0;
//******************************************************************************
//! \brief  putchar for sync usart with printf
//! \param  c: data for send with usart
//! \retval c: return data send
//******************************************************************************
int putchar( int c )
{
    //! Write a character to the USART 
    USART_SendData8( USART1, c );
  
    //! Loop until the end of transmission 
    while ( USART_GetFlagStatus( USART1, USART_FLAG_TC ) == RESET );

    return (c);
}
//******************************************************************************
//! \brief  Configure usart
//******************************************************************************
void UsartInit( void )
{
    //SYSCFG_REMAPPinConfig( REMAP_Pin_USART1TxRxPortC, ENABLE );
    
    //! Enable USART clock 
    CLK_PeripheralClockConfig( CLK_Peripheral_USART1, ENABLE );
    
    //! Configure USART Tx as alternate function push-pull  (software pull up)
    GPIO_ExternalPullUpConfig( USART_TX_GPIO, ENABLE );
    //GPIO_Init( USART_TX_PORT, USART_TX_PIN, GPIO_Mode_Out_OD_HiZ_Fast );
    
    //! Configure USART Rx as alternate function push-pull  (software pull up)*/
    GPIO_ExternalPullUpConfig( USART_RX_GPIO, ENABLE );
    //GPIO_Init( DIAG_RX_PORT, DIAG_RX_PIN, GPIO_Mode_In_PU_No_IT );
    
    //! USART configuration 
    USART_DeInit( USART1 );
    USART_Init( USART1, (u32) 9600, USART_WordLength_8b, USART_StopBits_1,
                USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx) );

    //! Enable the USART Receive interrupt
    USART_ITConfig( USART1, USART_IT_RXNE, ENABLE );
	
    //! Enable the USART Transmit interrupt
    //USART_ITConfig( USART1, USART_IT_TXE, DISABLE );
    
    USART_Cmd( USART1, ENABLE ); 
}
//******************************************************************************
//! \brief  usart send data
//! \param  data : usart data send
//******************************************************************************
void UsartSendData( u8 bData )
{
    //! Write a data to the USART 
    USART_SendData8( USART1, bData );
  
    //! Loop until the end of transmission 
    while ( USART_GetFlagStatus( USART1, USART_FLAG_TC ) == RESET );
}
//******************************************************************************
//! \brief  usart rx interrupt handeler
//******************************************************************************
void UsartRxIrqHandeler( void )
{
    u8 bStatus=0;
    u8 bDataRx=0;
    
    bStatus=USART1->SR;
    bDataRx=USART_ReceiveData8( USART1 );
       
    if( (bStatus & (USART_FLAG_OR | USART_FLAG_NF | USART_FLAG_FE | USART_FLAG_PE)) == 0x00 ) 
    {
        switch( bDataRx )
        {
            case '*':
                for( u8 i=0 ; i<20 ; i++ )
                    bUsartRxData[i]=0;
                bUsartCommaCounter = 0;
                bUsartDataRxCounter=0;
                break;
            case ',':
                bUsartCommaCounter++;
                break;
            case '#':
                bUsartRxDataFlag=1;
                break;
            default:
              /*
                if( bUsartCommaCounter==1 )
                    bUsartRxData[bUsartDataRxCounter++]=bDataRx;           
                else if( bUsartCommaCounter<=20 && bUsartCommaCounter >1 )
                    bUsartRxData[bUsartCommaCounter+bUsartDataRxCounter-2]=
                        10*bUsartRxData[bUsartCommaCounter+bUsartDataRxCounter-2] + 
                            bDataRx - '0';
              */
                if( bUsartCommaCounter<=20 )
                    bUsartRxData[bUsartCommaCounter-1]=
                        10*bUsartRxData[bUsartCommaCounter-1] + 
                            bDataRx - '0';
                break;
        } 
       
    }
    
    USART_ClearFlag( USART1, USART_FLAG_RXNE );   
}
//******************************************************************************
//! \brief  get usart rx data flag
//! \retval rx data flag
//******************************************************************************
u8 UsartGetRxDataFlag( void )
{
    return bUsartRxDataFlag;
}
//******************************************************************************
//! \brief  set usart rx data flag
//! \param  bRxFlag : rx data flag value
//******************************************************************************
void UsartSetRxDataFlag( u8 bRxFlag )
{
    bUsartRxDataFlag=bRxFlag;
}
//******************************************************************************
//! \brief  get usart rx data
//! \param  bDataArray : rx data array
//! \retval usart rx data length
//******************************************************************************
u8 UsartGetRxData( u8* bDataArray )
{
    for( u8 i=0 ; i<bUsartCommaCounter-1 ; i++ )
        bDataArray[i]=bUsartRxData[i];
        
    return bUsartCommaCounter-1;
}
//==============================================================================
void PrintNum( u16 Num)
{
  u8 D[10];
  u8 a=0;
  while(Num!=0)
  {
    D[a]=(Num%10)+48;
    Num/=10;
    a++;
  }
  for(s8 b=a-1;b>=0;b--)   
  {
    UsartSendData(D[b]);
  }
}
//==============================================================================
void PrintEnter(void)
{
  UsartSendData(13);
  UsartSendData(10);
}
//==============================================================================
void PrintLine(void)
{
  PrintEnter();
  for(u8 a=0;a<50;a++)
    UsartSendData('_');
  PrintEnter();
}
//==============================================================================
void PrintString(u8* Str,u8 len)
{
  for(u8 a=0;a<len;a++)
    UsartSendData(Str[a]);
}
