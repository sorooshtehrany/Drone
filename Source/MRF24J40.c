#include "..\Include\MRF24J40.h"

//******************************************************************************
//! Variable -------------------------------------------------------------------
u8 bMrf24j40DataRxFlag=0;
u8 bMrf24j40DataRxBuffer[150];
u8 bMrf24j40DataRxLen=0;
//******************************************************************************
//! \brief  init micro pio and spi for communicate with mrf24j40
//******************************************************************************
void Mrf24j40PinInit( void )
{
    //! init pio ------------------------------------
    //! reset pin output ( active low )
    GPIO_Init( MRF24J40_RESET_GPIO, GPIO_Mode_Out_PP_High_Fast );  
    //! wake pin output
    GPIO_Init( MRF24J40_WAKE_GPIO, GPIO_Mode_Out_PP_Low_Fast );
    //! sdi pin output
    GPIO_Init( MRF24J40_SDI_GPIO, GPIO_Mode_Out_PP_Low_Fast ); 
    //! int pin input
    GPIO_Init( MRF24J40_INT_GPIO, GPIO_Mode_In_FL_IT );
    //! sck pin output
    GPIO_Init( MRF24J40_SCK_GPIO, GPIO_Mode_Out_PP_Low_Fast );
    //! sdo pin input
    GPIO_Init( MRF24J40_SDO_GPIO, GPIO_Mode_In_FL_No_IT );
    //! cs pin output ( active low )
    GPIO_Init( MRF24J40_CS_GPIO, GPIO_Mode_Out_PP_High_Fast );
    //! --------------------------------------------- 
    //! init external interrupt ---------------------
    EXTI_SetPinSensitivity( EXTI_Pin_4, EXTI_Trigger_Falling );
    //! ---------------------------------------------
    //! init spi ------------------------------------ 
    //! Enable SPI clock 
    CLK_PeripheralClockConfig( CLK_Peripheral_SPI1, ENABLE );
  
    SPI_DeInit( MRF24J40_SPI );
    SPI_Init( MRF24J40_SPI, SPI_FirstBit_MSB, SPI_BaudRatePrescaler_8, SPI_Mode_Master,
              SPI_CPOL_Low, SPI_CPHA_1Edge, SPI_Direction_2Lines_FullDuplex,
              SPI_NSS_Soft, (uint8_t)0x07 );
    
    SPI_Cmd( MRF24J40_SPI, ENABLE );
    //! ---------------------------------------------
}
//******************************************************************************
//! \brief  set mrf24j40 register
//! \param  add : register address
//! \param  value : register value
//******************************************************************************
void Mrf24j40SetRegister( u8 bAdd , u8 bData )
{
    //! CS Pin Set Low ( Active )
    GPIO_ResetBits( MRF24J40_CS_GPIO );
    
    //! send address
    SPI_SendData( MRF24J40_SPI, bAdd );
    
    //! wait to compelet Transmit
    while( !SPI_GetFlagStatus( MRF24J40_SPI, SPI_FLAG_TXE ) );
    
    //! send register value
    SPI_SendData( MRF24J40_SPI, bData );
    
    //! wait to compelet Transmit
    while( !SPI_GetFlagStatus( MRF24J40_SPI, SPI_FLAG_TXE ) );
    
    //! CS Pin Set High ( Deactive )
    GPIO_SetBits( MRF24J40_CS_GPIO );    
}
//******************************************************************************
//! \brief  get mrf24j40 register value
//! \param  add : register address
//! \retval register value
//******************************************************************************
u8 Mrf24j40GetRegister( u8 bAdd )
{
    u8 bRegisterValue=0;
    
    //! CS Pin Set Low ( Active )
    GPIO_ResetBits( MRF24J40_CS_GPIO );
    
    //! send address
    SPI_SendData( MRF24J40_SPI, bAdd );
    
    //! wait to compelet Transmit
    while( !SPI_GetFlagStatus( MRF24J40_SPI, SPI_FLAG_TXE ) );
    
    //! get register value
    SPI_SendData( MRF24J40_SPI, 0x00 );
    bRegisterValue=SPI_ReceiveData( MRF24J40_SPI ); 
    
    //! wait to compelet Transmit
    while( !SPI_GetFlagStatus( MRF24J40_SPI, SPI_FLAG_TXE ) );
    
    //! CS Pin Set High ( Deactive )
    GPIO_SetBits( MRF24J40_CS_GPIO );
    
    return bRegisterValue;
}
//******************************************************************************
//! \brief  set mrf24j40 register long address
//! \param  add : register long address
//! \param  value : register value
//******************************************************************************
void Mrf24j40SetRegisterLong( u16 sAdd , u8 bData )
{
    //! CS Pin Set Low ( Active )
    GPIO_ResetBits( MRF24J40_CS_GPIO );
    
    //! send address
    SPI_SendData( MRF24J40_SPI, ((sAdd >> 3) & 0x7f)|0x80 );
    
    //! wait to compelet Transmit
    while( !SPI_GetFlagStatus( MRF24J40_SPI, SPI_FLAG_TXE ) );
   
    //! send address
    SPI_SendData( MRF24J40_SPI, ((sAdd << 5) & 0xe0)|0x10 ); 
    
    //! wait to compelet Transmit
    while( !SPI_GetFlagStatus( MRF24J40_SPI, SPI_FLAG_TXE ) );
    
    //! send register value
    SPI_SendData( MRF24J40_SPI, bData );
    
    //! wait to compelet Transmit
    while( !SPI_GetFlagStatus( MRF24J40_SPI, SPI_FLAG_TXE ) );
     
    //! CS Pin Set High ( Deactive )
    GPIO_SetBits( MRF24J40_CS_GPIO );
}
//******************************************************************************
//! \brief  get mrf24j40 register value
//! \param  add : register address
//! \retval register value
//******************************************************************************
u8 Mrf24j40GetRegisterLong( u16 sAdd )
{
    u8 bRegisterValue=0;
    
    //! CS Pin Set Low ( Active )
    GPIO_ResetBits( MRF24J40_CS_GPIO );
    
    //! send address
    SPI_SendData( MRF24J40_SPI, ((sAdd >> 3) & 0x7f)|0x80 );
    
    //! wait to compelet Transmit
    while( !SPI_GetFlagStatus( MRF24J40_SPI, SPI_FLAG_TXE ) );
    
    //! send address
    SPI_SendData( MRF24J40_SPI, ((sAdd << 5) & 0xe0) ); 
    
    //! wait to compelet Transmit
    while( !SPI_GetFlagStatus( MRF24J40_SPI, SPI_FLAG_TXE ) );
    
    //! get register value
    SPI_SendData( MRF24J40_SPI, 0x00 );
    bRegisterValue=SPI_ReceiveData( MRF24J40_SPI );
    
    //! wait to compelet Transmit
    while( !SPI_GetFlagStatus( MRF24J40_SPI, SPI_FLAG_TXE ) );
    
    //! CS Pin Set High ( Deactive )
    GPIO_SetBits( MRF24J40_CS_GPIO ); 
    
    return bRegisterValue;
}
//******************************************************************************
//! \brief  init mrf24j40 
//******************************************************************************
void Mrf24j40ChipInit( void )
{
    //u8 check=0;
    
    //! 1
    Mrf24j40SetRegister( WRITE_SOFTRST, 0x07 );     //! Power Management Reset, Baseband Reset, MAC Reset  
    //! 2
    Mrf24j40SetRegister( WRITE_PACON2, 0x98 );      //! FIFO Enable, Transmitter on time before beginning of packet:0x6 (6 * 16us = 96us).
    //! 3
    Mrf24j40SetRegister( WRITE_TXSTBL, 0x95 );      //! VCO Stabilization Period:0x9, Minimum Short Interframe Spacing:0x5
    //! 4
    Mrf24j40SetRegisterLong( RFCON0, 0x03 );        //! Channel 11 (2405 MHz):0x0, RF Optimize Control:0x3
    //! 5
    Mrf24j40SetRegisterLong( RFCON1, 0x01 );        //! VCO Optimize Control:0x1
    //! 6
    Mrf24j40SetRegisterLong( RFCON2, 0x80 );        //! PLL Enable
    //! 7
    Mrf24j40SetRegisterLong( RFCON6, 0x90 );        //! TX Filter Control, 20 MHz Clock Recovery Control:Less than 1 ms 
    //! 8
    Mrf24j40SetRegisterLong( RFCON7, 0x80 );        //! Sleep Clock Selection:100 kHz internal oscillator
    //! 9
    Mrf24j40SetRegisterLong( RFCON8, 0x10 );        //! VCO Control
    //! 10
    Mrf24j40SetRegisterLong(SLPCON1, 0x21);         //! CLKOUT Pin Disable:0x2, Sleep Clock Divisor:0x1
    //! 11
    Mrf24j40SetRegister( WRITE_BBREG2, 0x80 );      //1 Clear Channel Assessment (CCA) Mode:CCA Mode 1, 
    //! 12
    Mrf24j40SetRegister( WRITE_CCAEDTH, 0x60 );     //! Clear Channel Assessment (CCA) Energy Detection (ED) Mode:0x60 (approximately -69 dBm).
    //! 13
    Mrf24j40SetRegister( WRITE_BBREG6, 0x40 );      //! Calculate RSSI for each received packet. The RSSI value is stored in RXFIFO
    //! 14
    Mrf24j40SetRegister( WRITE_INTCON, 0xF7 );      //! Enables the RX FIFO reception interrupt, Disable the TX Normal FIFO transmission interrupt
    //! 15
    //set channel
    //! 16
    Mrf24j40SetRegisterLong( RFCON3, 0x00 );        //! Large Scale Control for TX Power:0 dB, Small Scale Control for TX Power:0 dB          
    //! 17
    Mrf24j40SetRegister( WRITE_RFCTL, 0x04 );       //! RF state machine in Reset
    //! 18
    Mrf24j40SetRegister( WRITE_RFCTL, 0x00 );       //! Normal operation of RF state machine
    //! 19

    Mrf24j40SetRegister( WRITE_RXFLUSH, 0x01 );     //! Reset Receive FIFO Address Pointer bit    
    
    Mrf24j40SetRegister( WRITE_RXMCR, 0x01 );     //! Receive all packet types with good CRC
    //mrf24j40SetRegister( WRITE_RXMCR, 0x03 );       //! Receive all packet types with good CRC, Accept all packets including those with CRC error
    
    Mrf24j40SetRegister( WRITE_RFCTL, 0x04 );       //! RF state machine in Reset
    Mrf24j40SetRegister( WRITE_RFCTL, 0x00 );       //! Normal operation of RF state machine     
    
}
//******************************************************************************
//! \brief  mrf24j40 send data
//! \param  data : data array for send
//! \param  dataLen : data len 
//******************************************************************************
void Mrf24j40SendData( u8* bDataArray, u8  bDataLen )
{   
    u8 bDataCounter=0; 
    u8 bTemp=0;
    u8 bDataLenBuffer=bDataLen+1;
    
    if( bDataLenBuffer<20 )
        bDataLenBuffer=20;
    
    Mrf24j40SetRegisterLong( 0, 0 );                //! HEADER_LENGHT
    Mrf24j40SetRegisterLong( 1, bDataLenBuffer );    //! HEADER_LENGHT + DATA_LENGHT
    Mrf24j40SetRegisterLong( 2, 0 );                //! FRAME CONTROL
    Mrf24j40SetRegisterLong( 3, 0 );                //! FRAME CONTROL
    Mrf24j40SetRegisterLong( 4, 0 );
    Mrf24j40SetRegisterLong( 5, 0 );
    
    for( bDataCounter=0 ; bDataCounter<bDataLen ; bDataCounter++ )
        Mrf24j40SetRegisterLong( bDataCounter+6, bDataArray[bDataCounter] );
        
    //! set_not_ACK
    bTemp = Mrf24j40GetRegister( READ_TXNCON );
    bTemp = bTemp & (!0x04);                 //0x04 mask for set not ACK
    Mrf24j40SetRegister( WRITE_TXNCON, bTemp ); 
    
    //! set_not_encrypt
    bTemp = Mrf24j40GetRegister( READ_TXNCON );
    bTemp = bTemp & (!0x02);                // mask for set not encrypt
    Mrf24j40SetRegister( WRITE_TXNCON, bTemp );   
    
    //! start_transmit
    bTemp = Mrf24j40GetRegister( READ_TXNCON );
    bTemp = bTemp | 0x01;                // mask for start transmit
    Mrf24j40SetRegister( WRITE_TXNCON, bTemp );   
    
    Mrf24j40GetRegister( READ_INTSTAT );
}
//******************************************************************************
//! \brief  mrf24j40 send data
//! \param  data : data array for send
//! \param  dataLen : data len 
//! \retval data valid
//******************************************************************************
void Mrf24j40ReadData( u8* bDataArray, u8 *bDataLen )
{   
    u8 bTemp=0;  
    u8 bDataCounter=0;
    
    bTemp = Mrf24j40GetRegister( READ_BBREG1 );      // disable receiving packets off air.
    bTemp = bTemp | 0x04;                             // mask for disable receiving packets 
    Mrf24j40SetRegister( WRITE_BBREG1, bTemp );  
         
    Mrf24j40GetRegisterLong( 0x300 );
    (*bDataLen)=Mrf24j40GetRegisterLong( 0x300+1 );
    Mrf24j40GetRegisterLong( 0x300+5 );
    
    for( bDataCounter=0 ; bDataCounter<(*bDataLen) ; bDataCounter++ )
        bDataArray[bDataCounter]=Mrf24j40GetRegisterLong( 0x300+bDataCounter+6 );
    
    bTemp = Mrf24j40GetRegister( READ_BBREG1 );      // enable receiving packets off air.
    bTemp = bTemp & (!0x04);                          // mask for enable receiving
    Mrf24j40SetRegister( WRITE_BBREG1, bTemp );
    
    Mrf24j40GetRegister( READ_INTSTAT );
}
//******************************************************************************
//! \brief  mrf24j40 get rx data flag
//! \retval rx flag
//******************************************************************************
u8 Mrf24j40GetRxFlag( void )
{
    return bMrf24j40DataRxFlag;
}
//******************************************************************************
//! \brief  mrf24j40 set rx data flag
//! \param  bFlag : mrf24j40 rx data flag
//******************************************************************************
void Mrf24j40SetRxFlag( u8 bFlag )
{
    bMrf24j40DataRxFlag=bFlag;
}
//******************************************************************************
//! \brief  mrf24j40 get rx data
//******************************************************************************
u8 Mrf24j40GetRxData( u8* bDataArray )
{
    //! return data array
    for( u8 i=0 ; i<bMrf24j40DataRxLen ; i++ )
        bDataArray[i]=bMrf24j40DataRxBuffer[i];
    
    //! return data len
    return bMrf24j40DataRxLen;
}
//******************************************************************************
//! \brief  mrf24j40 interrupt handeler
//******************************************************************************
void Mrf24j40IrqHandeler( void )
{
    Mrf24j40ReadData( bMrf24j40DataRxBuffer, &bMrf24j40DataRxLen );
    bMrf24j40DataRxFlag=1;
}
//******************************************************************************
//! \brief  mrf24j40 tx power set
//! \param  power : tx power value ( min tx power = 0 , max tx power = 31 )
//******************************************************************************
void Mrf24j40SetTxPower( u8 bPower )
{
    u8 bTxPower=bPower;
    
    if( bTxPower>31 )
        bTxPower=31;
    
    bTxPower=31-bTxPower;
    
    bTxPower<<=3;
    
    Mrf24j40SetRegisterLong( RFCON3, bTxPower );       
}
//******************************************************************************
//! \brief  mrf24j40 rx power set
//! \param  power : rx power value ( min rx power = 0 , max rx power = 255 )
//******************************************************************************
void Mrf24j40SetRxPower( u8 bPower )
{
    u8 bRxPower=bPower;
    
    bRxPower=255-bRxPower;
    
    Mrf24j40SetRegisterLong( RSSI, bRxPower );    
}
//******************************************************************************
//! \brief  mrf24j40 channel set
//! \param  channel : channel
//******************************************************************************
void Mrf24j40SetChannel( u8 bChannel )
{
    Mrf24j40SetRegisterLong( RFCON0, (bChannel|0x03) );      //! RF set channel
    Mrf24j40SetRegister( WRITE_RFCTL, 0x04 );               //! RF state machine in Reset
    Mrf24j40SetRegister( WRITE_RFCTL, 0x00 );               //! Normal operation of RF state machine 
}