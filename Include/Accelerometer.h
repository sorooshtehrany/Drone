#ifndef __Accelerometer_H
#define __Accelerometer_H

#include "..\STM8L Library\Include\stm8l15x.h"



#define ACCELEREMETER_INT_GPIO       GPIOB,GPIO_Pin_4

/*Register Addresses & Description*/
///////////////////////////////////
// MMA8452Q Register Definitions //
///////////////////////////////////

#define	STATUS            0x00
#define	OUT_X_MSB         0x01
#define	OUT_X_LSB         0x02
#define	OUT_Y_MSB         0x03
#define	OUT_Y_LSB         0x04
#define	OUT_Z_MSB         0x05
#define	OUT_Z_LSB         0x06
#define	SYSMOD            0x0B
#define	INT_SOURCE        0x0C
#define	WHO_AM_I          0x0D
#define	XYZ_DATA_CFG      0x0E
#define	HP_FILTER_CUTOFF  0x0F
#define	PL_STATUS         0x10
#define	PL_CFG            0x11
#define	PL_COUNT          0x12
#define	PL_BF_ZCOMP       0x13
#define	P_L_THS_REG       0x14
#define	FF_MT_CFG         0x15
#define	FF_MT_SRC         0x16
#define	FF_MT_THS         0x17
#define	FF_MT_COUNT       0x18
#define	TRANSIENT_CFG     0x1D
#define	TRANSIENT_SRC     0x1E
#define	TRANSIENT_THS     0x1F
#define	TRANSIENT_COUNT   0x20
#define	PULSE_CFG         0x21
#define	PULSE_SRC         0x22
#define	PULSE_THSX        0x23
#define	PULSE_THSY        0x24
#define	PULSE_THSZ        0x25
#define	PULSE_TMLT        0x26
#define	PULSE_LTCY        0x27
#define	PULSE_WIND        0x28
#define	ASLP_COUNT        0x29
#define	CTRL_REG1         0x2A
#define	CTRL_REG2         0x2B
#define	CTRL_REG3         0x2C
#define	CTRL_REG4         0x2D
#define	CTRL_REG5         0x2E
#define	OFF_X             0x2F
#define	OFF_Y             0x30
#define	OFF_Z             0x31
////////////////////////////////
// MMA8452Q Misc Declarations //
////////////////////////////////
// Possible portrait/landscape settings
#define PORTRAIT_U    0
#define PORTRAIT_D    1
#define LANDSCAPE_R   2
#define LANDSCAPE_L   3
#define LOCKOUT       0x40
#define addr          0X1C
////////////////////////////////
// MMA8452Q Class Declaration //
////////////////////////////////
//MMA8452Q(unsigned char addr = 0x1C); // Constructor
/*
struct Accel_Init {
                    unsigned char StaticMotionEnabled;
                    unsigned char StaticMotionDebunce;
                    unsigned char StaticMotionThreshold;
                    unsigned char StaticMotion_OnXDirection;     //0 1 255
                    unsigned char StaticMotion_OnYDirection;     // 0  1  255
                    unsigned char StaticMotion_OnZDirection;     //0  1  255
                    //..........................................................    
                    unsigned char DynamicMotionEnabled;
                    unsigned char DynamicMotion_Debunce;
                    unsigned char DynamicMotion_Threshold;
                    unsigned char DynamicMotion_OnXDirection;     //0 1 255
                    unsigned char DynamicMotion_OnYDirection;     // 0  1  255
                    unsigned char DynamicMotion_OnZDirection;     //0  1  255
                    //..........................................................    
                    unsigned char PulseDetetcEnabled;
                    unsigned char PulseDetect_OnXEnabled;
                    unsigned char PulseDetect_OnYEnabled;
                    unsigned char PulseDetect_OnZEnabled;
                    unsigned char PulseDetect_OnXThreshold;
                    unsigned char PulseDetect_OnYThreshold;
                    unsigned char PulseDetect_OnZThreshold;
                    unsigned char PulseDetect_TimeLimit;
                    unsigned char PulseDetect_TimeLattency;
                    unsigned char PulseDetect_WindiwTimeFor2d;
                    unsigned char PulseDetect_OnXHappen;
                    unsigned char PulseDetect_OnYHappen;
                    unsigned char PulseDetect_OnZHappen;
                    //..........................................................    
                    unsigned char LandscapePortrate_Enabled;
                    unsigned char 
                    //................................
                    unsigned char PowerMode;
                  }

struct Accel_Init Accel;
                    
void Accel_FirstInitValue(void);
void Accel_Init(void);
void Accel_FactoryDefaulte(void);

                    
                    
                    
void Accel_FirstInitValue(void)
{
Accel.StaticMotionEnabled=1;
                    unsigned char StaticMotionDebunce;
                    unsigned char StaticMotionThreshold;
                    unsigned char StaticMotion_OnXDirection;     //0 1 255
                    unsigned char StaticMotion_OnYDirection;     // 0  1  255
                    unsigned char StaticMotion_OnZDirection;     //0  1  255
                    //..........................................................    
                    unsigned char DynamicMotionEnabled;
                    unsigned char DynamicMotion_Debunce;
                    unsigned char DynamicMotion_Threshold;
                    unsigned char DynamicMotion_OnXDirection;     //0 1 255
                    unsigned char DynamicMotion_OnYDirection;     // 0  1  255
                    unsigned char DynamicMotion_OnZDirection;     //0  1  255
                    //..........................................................    
                    unsigned char PulseDetetcEnabled;
                    unsigned char PulseDetect_OnXEnabled;
                    unsigned char PulseDetect_OnYEnabled;
                    unsigned char PulseDetect_OnZEnabled;
                    unsigned char PulseDetect_OnXThreshold;
                    unsigned char PulseDetect_OnYThreshold;
                    unsigned char PulseDetect_OnZThreshold;
                    unsigned char PulseDetect_TimeLimit;
                    unsigned char PulseDetect_TimeLattency;
                    unsigned char PulseDetect_WindiwTimeFor2d;
                    unsigned char PulseDetect_OnXHappen;
                    unsigned char PulseDetect_OnYHappen;
                    unsigned char PulseDetect_OnZHappen;
                    //..........................................................    
                    unsigned char LandscapePortrate_Enabled;
                    unsigned char 
                    //................................
                    unsigned char PowerMode;

}

void Accel_Init(void)
{




}
                    

                      
unsigned char AccelRead_StaticMotion(void)
{
}
                      
                    


*/









  

                    
                    
                    
void            AccInit               (void);
unsigned char   init                  (/*unsigned char Fsr, unsigned char Odr*/);
void            MMA8452Q              (unsigned char add);
void            read                  (void);       
unsigned char   available             (void);
unsigned char   readTap               (void);
unsigned char   readPL                (void);	
void            standby               (void);
void            active                (void);
void            setupPL               (void);
void            setupTap              (unsigned char xThs, unsigned char yThs, unsigned char zThs);
void            setScale              (unsigned char fsr);
void            setODR                (unsigned char odr);
u8   writeRegister         (u8 reg,u8 data);
//void          writeRegisters        (MMA8452Q_Register reg, unsigned char *buffer, unsigned char len);
unsigned char   readRegister          (u8 add );
//void          readRegisters         (MMA8452Q_Register reg, unsigned char *buffer, unsigned char len);
//---------------------
void            loop                  (void);
void            printOrientation      (void);
void            printAccels           (void);
void            printCalculatedAccels (void);
//---------------------high level accel
void            FF_MTInit             (void);
void            FF_MTIrqHandeler      (void);
void            tapHandler            (void);
void            FF_MTInit2            (void);
void            motionDetect          (void);
void            MMA8452Reset          (void);
void            MMA8452Offsets        (void);
void            portraitLandscapeHandler(void);
void            Transition_WithIntrrupt(void);
void            Transition_Detect      (void);
void            Orientation_Detect    (void);
void            Auto_WakeSleep        (void);
#endif