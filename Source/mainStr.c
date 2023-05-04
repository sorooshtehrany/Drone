#include "..\Include\mainStr.h"

//******************************************************************************
//! Variable -------------------------------------------------------------------
u8 bMainStrUsartDataRx[20];
u8 bMainStrUsartDataRxFlag=0;;
u8 bMainStrUsartDataRxLen=0;;

u8 bMainStrRfDataRx[150];
u8 bMainStrRfDataRxFlag=0;
u8 bMainStrRfDataRxLen=0;
//******************************************************************************
//! \brief  main str init
//******************************************************************************
void MainStrInit( void )
{
    
}
//******************************************************************************
//! \brief  main str main function
//******************************************************************************
void MainStrMain( void )
{
    
}
//******************************************************************************
//! \brief  main str check usart data rx
//******************************************************************************
void MainStrUsartDataCheck( void )
{
    //!   __  ________________  ___________  _________  _________________  _____________________  __
    //!  | *| Destination Add |	Source Add | Data Len |	CMD + Data Array | sum of all data pack | # |
    
    u8  bUsartDataBuffer[20];
    u8  bUsartDataLength=0;
    u8  bDataCheckSum=0;
    
    //! check usart rx data flag
    if( UsartGetRxDataFlag()==1 )
    {
        //! clear usart rx data flag
        UsartSetRxDataFlag(0);
        
        //! set main usart rx data flag
        bMainStrUsartDataRxFlag=1;
        
        //! usart get data
        bUsartDataLength = UsartGetRxData( bUsartDataBuffer );
        
        //! set data length variable
        bMainStrUsartDataRxLen=bUsartDataLength-1;
        
        //! check destination address and cmd for ping main board
        if( bUsartDataBuffer[0]==0 && bUsartDataBuffer[1]==1 )
        {
            bMainStrRfDataRxFlag=1;
            bMainStrRfDataRxLen=1;
            bMainStrRfDataRx[1]=0;
            bMainStrRfDataRx[2]=0;
            bMainStrRfDataRx[3]=1;
            bMainStrRfDataRx[4]=101;
            bMainStrRfDataRx[5]=101;
        }
        else
        {
            //! set start byte
            bMainStrUsartDataRx[0]='*';
            //! set destination address
            bMainStrUsartDataRx[1]=bUsartDataBuffer[0];
            //! set source address
            bMainStrUsartDataRx[2]=0;
            //! set data length
            bMainStrUsartDataRx[3]=bMainStrUsartDataRxLen;
            //! set data pack
            for( u8 i=0 ; i<bMainStrUsartDataRxLen ; i++ )
            {
                bMainStrUsartDataRx[i+4]=bUsartDataBuffer[i+1];
                bDataCheckSum+=bUsartDataBuffer[i+1];
            }
            //! set check sum
            bMainStrUsartDataRx[bMainStrUsartDataRxLen+4]=bDataCheckSum;
            //! set end byte
            bMainStrUsartDataRx[bMainStrUsartDataRxLen+5]='#';
        }
    }
}
//******************************************************************************
//! \brief  main str send data to usart 
//******************************************************************************
void MainStrUsartDataSend( void )
{
    //!   __  ________________  ___________  _________  _________________  _____________________  __
    //!  | *| Destination Add |	Source Add | Data Len |	CMD + Data Array | sum of all data pack | # |
    
    //! check main rf rx data flag
    if( bMainStrRfDataRxFlag==1 )
    {
        //! clear main rf rx data flag
        bMainStrRfDataRxFlag=0;
        
        printf( "\n\r*," );
        
        for( u8 i=1 ; i<=bMainStrRfDataRxLen+4 ; i++ )
            printf( "%d,", bMainStrRfDataRx[i] );
        
        printf( "#\n\r" );
    }
}
//******************************************************************************
//! \brief  main str check rf data rx
//******************************************************************************
void MainStrRfDataCheck( void )
{   
    //!   __  ________________  ___________  _________  _________________  _____________________  __
    //!  | *| Destination Add |	Source Add | Data Len |	CMD + Data Array | sum of all data pack | # |

    u8 bRfDataSum=0;
    
    //! check rf data rx flag
    if( Mrf24j40GetRxFlag()==1 )
    {
        //! clear rf data rx flag
        Mrf24j40SetRxFlag(0);
        //! get rf rx data
        Mrf24j40GetRxData(bMainStrRfDataRx);
        //! read pack data len
        bMainStrRfDataRxLen=bMainStrRfDataRx[3];
    
        //! check start and end byte
        if( bMainStrRfDataRx[0]=='*' && bMainStrRfDataRx[bMainStrRfDataRxLen+5]=='#' )
        {
            //! claculate data pack check sum
            for( u8 i=0 ; i<bMainStrRfDataRxLen ; i++ )
                bRfDataSum+=bMainStrRfDataRx[i+4];
            //! check check sum
            if( bRfDataSum==bMainStrRfDataRx[bMainStrRfDataRxLen+4] )
            {
                //! check Destination 
                if( bMainStrRfDataRx[1]==0 )
                {
                    //! set data rx flag    
                    bMainStrRfDataRxFlag=1;
                }
            }
        }
    }
}
//******************************************************************************
//! \brief  main str send data to rf
//******************************************************************************
void MainStrRfDataSend( void )
{
    //!   __  ________________  ___________  _________  _________________  _____________________  __
    //!  |* | Destination Add |	Source Add | Data Len |	CMD + Data Array | sum of all data pack | # |
    
    //! check main data rx flag
    if( bMainStrUsartDataRxFlag==1 )
    {
        //! clear main rx data flag
        bMainStrUsartDataRxFlag=0;
        
        //! send data with rf module
        Mrf24j40SendData( bMainStrUsartDataRx, bMainStrUsartDataRxLen+6+4 );
    }
}
//******************************************************************************
//! \brief  main str set all system work mode
//! \param  bWorkMode : all system work mode
//******************************************************************************
void MainStrSetWorkMode( u8 bWorkMode )
{
    //!   __  ________________  ___________  _________  _________________  _____________________  __
    //! | *	| Destination Add |	Source Add | Data Len |	CMD + Data Array | sum of all data pack | # |
    
    u8 bDataSend[20];
    
    //! set start byte
    bDataSend[0]='*';
    //! set Destination address
    bDataSend[1]=0xFF;
    //! set source address ( select all device )
    bDataSend[2]=0x00;  
    //! set data length
    bDataSend[3]=1;
    
    //! check work mode for set
    if( bWorkMode==MAINSTR_WORK_MODE_STOP )
        bDataSend[4]=MAINSTR_STOP_GAME_REQ_CMD;
    else if( bWorkMode==MAINSTR_WORK_MODE_START )
        bDataSend[4]=MAINSTR_START_GAME_REQ_CMD;
    else if( bWorkMode==MAINSTR_WORK_MODE_PAUSE )
        bDataSend[4]=MAINSTR_PAUSE_GAME_REQ_CMD;
    
    //! set check sum
    bDataSend[5]=bDataSend[4];
    
    //! set end byte
    bDataSend[6]='#';
    
    //! send data with rf module
    Mrf24j40SendData( bDataSend, 7 );
}