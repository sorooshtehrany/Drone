#include "../Include/myiic.h"
//////////////////////////////////////////////////////////////////////////////////	 
//////////////////////////////////////////////////////////////////////////////////
void IIC_Init(void)
{					     
  GPIO_Init( SCL_GPIO_PORT,SCL_PIN, GPIO_Mode_Out_PP_Low_Fast );
  GPIO_Init( SDA_GPIO_PORT,SDA_PIN, GPIO_Mode_Out_PP_Low_Fast );
  GPIO_SetBits(SDA_GPIO_PORT,SDA_PIN);
  GPIO_SetBits(SCL_GPIO_PORT,SDA_PIN);
}
//////////////////////////////////////////////////////////////////////////////////
void IIC_Start(void)
{
 SDA_OUT();     
 IIC_SDA_1;	  	  
 IIC_SCL_1;
 DelayUs(4);
 IIC_SDA_0;                //START:when CLK is high,DATA change form high to low 
 DelayUs(4);
 IIC_SCL_0;
}	  
//////////////////////////////////////////////////////////////////////////////////
void IIC_Stop(void)
{
 SDA_OUT();
 IIC_SCL_0;
 IIC_SDA_0;//STOP:when CLK is high DATA change form low to high
 DelayUs(4);
 IIC_SCL_1; 
 IIC_SDA_1;
 DelayUs(4);							   	
}
//////////////////////////////////////////////////////////////////////////////////
u8 IIC_Wait_Ack(void)
{
 u8 ucErrTime=0;
 SDA_IN();      
 IIC_SDA_1;DelayUs(1);	   
 IIC_SCL_1;DelayUs(1);	 
 while(READ_SDA)
      {
	ucErrTime++;
	if(ucErrTime>250)
	  {
	   IIC_Stop();
	   return 0;
	  }
      }
 IIC_SCL_0;
 return 1;  
} 
//////////////////////////////////////////////////////////////////////////////////
void IIC_Ack(void)
{
 IIC_SCL_0;
 SDA_OUT();
 IIC_SDA_0;
 DelayUs(2);
 IIC_SCL_1;
 DelayUs(2);
 IIC_SCL_0;
}

void IIC_NAck(void)
{
 IIC_SCL_0;
 SDA_OUT();
 IIC_SDA_1;
 DelayUs(2);
 IIC_SCL_1;
 DelayUs(2);
 IIC_SCL_0;
}		
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void IIC_Send_Byte(u8 txd)
{                        
 u8 t;   
 SDA_OUT(); 	    
 IIC_SCL_0;
 for(t=0;t<8;t++)
    {              
     if((txd&0x80)>>7)
			IIC_SDA_1;
     else
			IIC_SDA_0;
     txd<<=1; 	  
     DelayUs(2);   
     IIC_SCL_1;
     DelayUs(2); 
     IIC_SCL_0;	
     DelayUs(2);
    }	 
} 	    
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
u8 IIC_Read_Byte(unsigned char ack)
{
 unsigned char i,receive=0;
 SDA_IN();
 for(i=0;i<8;i++ )
    {
     IIC_SCL_0; 
     DelayUs(2);
     IIC_SCL_1;
     receive<<=1;
     if(READ_SDA)receive++;   
     DelayUs(1); 
    }					 
 if (!ack)
        IIC_NAck();
 else
        IIC_Ack(); 
 return receive;
}


