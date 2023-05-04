#include "../Include/MyI2C.h"

//******************************************************************************
//! Variable -------------------------------------------------------------------
void Delay(void)
{
  for(u8 i=0;i<5;i++);
}
//=======================================================================================================
void          MyIIC_Init           (void)
{
  GPIO_Init( SCL_GPIO_PORT,SCL_PIN, GPIO_Mode_Out_PP_Low_Fast );
  GPIO_Init( SDA_GPIO_PORT,SDA_PIN, GPIO_Mode_Out_PP_Low_Fast );
  GPIO_SetBits(SDA_GPIO_PORT,SDA_PIN);
  GPIO_SetBits(SCL_GPIO_PORT,SDA_PIN);
}
//=======================================================================================================
void          MyIIC_GenerateStart  (void)
{
  GPIO_ResetBits(SDA_GPIO_PORT,SDA_PIN);
  Delay();
  GPIO_ResetBits(SCL_GPIO_PORT,SDA_PIN);
  Delay();
  GPIO_SetBits(SDA_GPIO_PORT,SDA_PIN);
  GPIO_SetBits(SDA_GPIO_PORT,SCL_PIN);
}
//=======================================================================================================
unsigned char MyIIC_Read           (unsigned char Data)
{
}
//=======================================================================================================
void          MyIIC_Write          (unsigned char Data)
{
  for(u16 x=0;x<8;x++)
     {
       if(Data &  (1<<(7-x))!=0)    GPIO_SetBits(SDA_GPIO_PORT,SDA_PIN);
       else                         GPIO_ResetBits(SDA_GPIO_PORT,SDA_PIN); 
       Delay();
       GPIO_ResetBits(SCL_GPIO_PORT,SCL_PIN); 
       Delay();           
       GPIO_SetBits(SCL_GPIO_PORT,SCL_PIN);          
     }  
}
//=======================================================================================================
void          MyIIC_SlaveAddress   (unsigned char Address,unsigned char Mode)
{
  GPIO_SetBits(SCL_GPIO_PORT,SCL_PIN);   
  for(u16 x=0;x<7;x++)
     {
       if(Address & (1<<(6-x))!=0)    GPIO_SetBits(SDA_GPIO_PORT,SDA_PIN);
       else                         GPIO_ResetBits(SDA_GPIO_PORT,SDA_PIN); 
       Delay();
       GPIO_ResetBits(SCL_GPIO_PORT,SCL_PIN); 
       Delay();           
       GPIO_SetBits(SCL_GPIO_PORT,SCL_PIN);          
     }
  if(Mode == Write_Mode)            GPIO_ResetBits(SDA_GPIO_PORT,SDA_PIN);
  if(Mode == Read_Mode)             GPIO_SetBits(SDA_GPIO_PORT,SDA_PIN);
  Delay();
  GPIO_ResetBits(SCL_GPIO_PORT,SCL_PIN); 
  Delay();           

  GPIO_Init( SDA_GPIO_PORT,SDA_PIN, GPIO_Mode_In_FL_No_IT );
  while(GPIO_ReadInputDataBit(SDA_GPIO_PORT,SDA_PIN)!=0);
}
