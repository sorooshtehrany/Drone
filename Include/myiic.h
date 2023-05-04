#ifndef __MYIIC_H
#define __MYIIC_H

#include "..\STM8L Library\Include\stm8l15x.h"

#define SCL_PIN                  GPIO_Pin_1                  /* PC.01 */
#define SCL_GPIO_PORT            GPIOC                       /* GPIOC */
#define SDA_PIN                  GPIO_Pin_0                  /* PC.00 */
#define SDA_GPIO_PORT            GPIOC                       /* GPIOC */

#define WriteMode                0
#define ReadMode                 1   
#define AccSlaveAddr             0X1C



#define SDA_IN()                 GPIO_Init( SDA_GPIO_PORT,SDA_PIN, GPIO_Mode_In_FL_No_IT );
#define SDA_OUT()                GPIO_Init( SDA_GPIO_PORT,SDA_PIN, GPIO_Mode_Out_PP_Low_Fast );

#define IIC_SCL_1                GPIO_SetBits(SCL_GPIO_PORT,SCL_PIN) 
#define IIC_SCL_0                GPIO_ResetBits(SCL_GPIO_PORT,SCL_PIN) 
  
#define IIC_SDA_1                GPIO_SetBits(SDA_GPIO_PORT,SDA_PIN) 
#define IIC_SDA_0                GPIO_ResetBits(SDA_GPIO_PORT,SDA_PIN) 

#define READ_SDA                 GPIO_ReadInputDataBit(SDA_GPIO_PORT,SDA_PIN)




void IIC_Init(void);                
void IIC_Start(void);				
void IIC_Stop(void);	  			
void IIC_Send_Byte(u8 txd);			
u8   IIC_Read_Byte(unsigned char ack);
u8   IIC_Wait_Ack(void); 				
void IIC_Ack(void);					
void IIC_NAck(void);				

//void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
//u8   IIC_Read_One_Byte(u8 daddr,u8 addr);	  
#endif

