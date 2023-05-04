#include "..\Include\Accelerometer.h"
#include "..\Include\myiic.h"
#include "..\Include\usart.h"
//******************************************************************************
//! Variable -------------------------------------------------------------------
enum MMA8452Q_Scale {SCALE_2G = 2, SCALE_4G = 4, SCALE_8G = 8};                       // Possible full-scale settings
enum MMA8452Q_ODR {ODR_800, ODR_400, ODR_200, ODR_100, ODR_50, ODR_12, ODR_6, ODR_1}; // possible data rates



unsigned char fsr = SCALE_2G;
unsigned char odr = ODR_100;

	
int x, y, z;
float cx, cy, cz;

unsigned char address;
unsigned char scale;

//=========================================================================================================================================
void MMA8452Q(unsigned char add)
{
	address = add; // Store address into private variable
}
//=========================================================================================================================================
// INITIALIZATION
//	This function initializes the MMA8452Q. It sets up the scale (either 2, 4,
//	or 8g), output data rate, portrait/landscape detection and tap detection.
//	It also checks the WHO_AM_I register to make sure we can communicate with
//	the sensor. Returns a 0 if communication failed, 1 if successful.
//=========================================================================================================================================
unsigned char init(/*unsigned char Fsr, unsigned char Odr*/)
{
        //! int pin input
        GPIO_Init( ACCELEREMETER_INT_GPIO, GPIO_Mode_In_FL_IT );
        EXTI_DeInit();
        EXTI_SetPinSensitivity(EXTI_Pin_4,EXTI_Trigger_Rising);
        EXTI_SelectPort(EXTI_Port_B);
        
        
        scale = fsr; // Haul fsr into our class variable, scale
        IIC_Init(); // Initialize I2C
        unsigned char X= readRegister(WHO_AM_I);  // Read WHO_AM_I register
	
	if (X != 0x2A) // WHO_AM_I should always be 0x2A
	{
		return 0;
	}
	
	standby();  // Must be in standby to change registers	
	setScale(scale);  // Set up accelerometer scale
	setODR(odr);  // Set up output data rate
	setupPL();  // Set up portrait/landscape detection
	// Multiply parameter by 0.0625g to calculate threshold.
	setupTap(0x80, 0x80, 0x08); // Disable x, y, set z to 0.5g
	
	active();  // Set to active to start reading
	
	return 1;
}
//=========================================================================================================================================
// READ ACCELERATION DATA
//  This function will read the acceleration values from the MMA8452Q. After
//	reading, it will update two triplets of variables:
//		* int's x, y, and z will store the signed 12-bit values read out
//		  of the acceleromter.
//		* floats cx, cy, and cz will store the calculated acceleration from
//		  those 12-bit values. These variables are in units of g's.
//=========================================================================================================================================
void read(void)
{
	unsigned char rawData[6];  // x/y/z accel register data stored here
        
        // Read the six raw data registers into data array
        
	rawData[0]=readRegister(OUT_X_MSB);  
        rawData[1]=readRegister(OUT_X_MSB+1);  
        rawData[2]=readRegister(OUT_X_MSB+2);  
        rawData[3]=readRegister(OUT_X_MSB+3);  
        rawData[4]=readRegister(OUT_X_MSB+4);  
        rawData[5]=readRegister(OUT_X_MSB+5);  
        
        
	x = ((unsigned int)(rawData[0]<<8 | rawData[1])) >> 4;   x+=2048;
	y = ((unsigned int)(rawData[2]<<8 | rawData[3])) >> 4;   y+=2048; 
	z = ((unsigned int)(rawData[4]<<8 | rawData[5])) >> 4;   z+=2048;  
	cx = (float) x / (float)(1<<11) * (float)(scale);
	cy = (float) y / (float)(1<<11) * (float)(scale);
	cz = (float) z / (float)(1<<11) * (float)(scale);
}
//=========================================================================================================================================
// CHECK IF NEW DATA IS AVAILABLE
//	This function checks the status of the MMA8452Q to see if new data is availble.
//	returns 0 if no new data is present, or a 1 if new data is available.
//=========================================================================================================================================
unsigned char available()
{
        unsigned char X=readRegister((STATUS) & 0x08 >> 3);
        return(X);
}
//=========================================================================================================================================
// SET FULL-SCALE RANGE
//	This function sets the full-scale range of the x, y, and z axis accelerometers.
//	Possible values for the fsr variable are SCALE_2G, SCALE_4G, or SCALE_8G.
//=========================================================================================================================================
void setScale(unsigned char Fsr)
{
	// Must be in standby mode to make changes!!!
	unsigned char cfg=readRegister(XYZ_DATA_CFG);
	cfg &= 0xFC; // Mask out scale bits
	cfg |= (Fsr >> 2);  // Neat trick, see page 22. 00 = 2G, 01 = 4A, 10 = 8G
	writeRegister(XYZ_DATA_CFG, cfg);
}
//=========================================================================================================================================
// SET THE OUTPUT DATA RATE
//	This function sets the output data rate of the MMA8452Q.
//	Possible values for the odr parameter are: ODR_800, ODR_400, ODR_200, 
//	ODR_100, ODR_50, ODR_12, ODR_6, or ODR_1
//=========================================================================================================================================
void setODR(unsigned char Odr)
{
	// Must be in standby mode to make changes!!!
	unsigned char ctrl=readRegister(CTRL_REG1);
	ctrl &= 0xCF; // Mask out data rate bits
	ctrl |= (Odr << 3);
	writeRegister(CTRL_REG1, ctrl);
}
//=========================================================================================================================================
// SET UP TAP DETECTION
//	This function can set up tap detection on the x, y, and/or z axes.
//	The xThs, yThs, and zThs parameters serve two functions:
//		1. Enable tap detection on an axis. If the 7th bit is SET (0x80)
//			tap detection on that axis will be DISABLED.
//		2. Set tap g's threshold. The lower 7 bits will set the tap threshold
//			on that axis.
//=========================================================================================================================================
void setupTap(unsigned char xThs, unsigned char yThs, unsigned char zThs)
{
	// Set up single and double tap - 5 steps:
	// for more info check out this app note:
	// http://cache.freescale.com/files/sensors/doc/app_note/AN4072.pdf
	// Set the threshold - minimum required acceleration to cause a tap.
	unsigned char temp = 0;
	if (!(xThs & 0x80)) // If top bit ISN'T set
	{
		temp |= 0x3; // Enable taps on x
		writeRegister(PULSE_THSX, xThs);  // x thresh
	}
	if (!(yThs & 0x80))
	{
		temp |= 0xC; // Enable taps on y
		writeRegister(PULSE_THSY, yThs);  // y thresh
	}
	if (!(zThs & 0x80))
	{
		temp |= 0x30; // Enable taps on z
		writeRegister(PULSE_THSZ, zThs);  // z thresh
	}
	// Set up single and/or double tap detection on each axis individually.
	writeRegister(PULSE_CFG, temp | 0x40);
	// Set the time limit - the maximum time that a tap can be above the thresh
	writeRegister(PULSE_TMLT, 0x30);  // 30ms time limit at 800Hz odr
	// Set the pulse latency - the minimum required time between pulses
	writeRegister(PULSE_LTCY, 0xA0);  // 200ms (at 800Hz odr) between taps min
	// Set the second pulse window - maximum allowed time between end of
	//	latency and start of second pulse
	writeRegister(PULSE_WIND, 0xFF);  // 5. 318ms (max value) between taps max
}
//=========================================================================================================================================
// READ TAP STATUS
//	This function returns any taps read by the MMA8452Q. If the function 
//	returns no new taps were detected. Otherwise the function will return the
//	lower 7 bits of the PULSE_SRC register.
//=========================================================================================================================================
unsigned char readTap()
{
	unsigned char tapStat=readRegister(PULSE_SRC);
	if (tapStat & 0x80) // Read EA bit to check if a interrupt was generated
	{
		return tapStat & 0x7F;
	}
	else
		return 0;
}
//=========================================================================================================================================
// SET UP PORTRAIT/LANDSCAPE DETECTION
//	This function sets up portrait and landscape detection.
//=========================================================================================================================================
void setupPL()
{
	// Must be in standby mode to make changes!!!
	// For more info check out this app note:
	//	http://cache.freescale.com/files/sensors/doc/app_note/AN4068.pdf
	// 1. Enable P/L
        unsigned char X=readRegister(PL_CFG);
	writeRegister(PL_CFG, X | 0x40); // Set PL_EN (enable)
	// 2. Set the debounce rate
	writeRegister(PL_COUNT, 0x50);  // Debounce counter at 100ms (at 800 hz)
}
//=========================================================================================================================================
// READ PORTRAIT/LANDSCAPE STATUS
//	This function reads the portrait/landscape status register of the MMA8452Q.
//	It will return either PORTRAIT_U, PORTRAIT_D, LANDSCAPE_R, LANDSCAPE_L,
//	or LOCKOUT. LOCKOUT indicates that the sensor is in neither p or ls.
//=========================================================================================================================================
unsigned char readPL()
{
	unsigned char plStat=readRegister(PL_STATUS);
	
	if (plStat & 0x40) // Z-tilt lockout
		return LOCKOUT;
	else // Otherwise return LAPO status
		return (plStat & 0x6) >> 1;
}
//=========================================================================================================================================
// SET STANDBY MODE
//	Sets the MMA8452 to standby mode. It must be in standby to change most register settings
//=========================================================================================================================================
void standby()
{
	unsigned char X=readRegister(CTRL_REG1);
	writeRegister(CTRL_REG1, X & ~(0x01)); //Clear the active bit to go into standby
}
//=========================================================================================================================================
// SET ACTIVE MODE
//	Sets the MMA8452 to active mode. Needs to be in this mode to output data
//=========================================================================================================================================
void active()
{
	unsigned char X =readRegister(CTRL_REG1);
	writeRegister(CTRL_REG1, X | 0x01); //Set the active bit to begin detection
}
/*------------------------------------------------------------------------------
readRegister : Single Byte Read From MMA8452Q
------------------------------------------------------------------------------*/
unsigned char   readRegister          (u8 add )
{               
  u8 fault;
  u8 Read_Data;
  unsigned char data;
 //! generate start condition
 IIC_Start();
 //! generate 7bit slave address + 1bit write/read mode
 IIC_Send_Byte((AccSlaveAddr <<1) | WriteMode);
 //! wait until slave put ack
 fault = IIC_Wait_Ack();
 //! return 0 if not receive ack
 //if(fault==0)   return(0);
 //!delay
 DelayUs(2000);
 //..................
 //! put register address
 IIC_Send_Byte(add);
 //! wait until slave put ack
 fault = IIC_Wait_Ack();  
 //! return 0 if not receive ack
// if(fault==0)   return(0);
 //.............
 //! generate repeted start condition
 IIC_Start();
 //! generate 7bit slave address + 1bit write/read mode
 IIC_Send_Byte((AccSlaveAddr <<1) | ReadMode);
 //! wait until slave put ack
 fault = IIC_Wait_Ack();
 //! return 0 if not receive ack
// if(fault==0)   return(0);
 //............. 
 //! read unsigned char and send nack
 data=IIC_Read_Byte(0);
 //! generate stop condition
 IIC_Stop();
 return(data);        
}
/*------------------------------------------------------------------------------
writeRegister : Single Byte Write to MMA8452Q
------------------------------------------------------------------------------*/
u8            writeRegister         (u8 reg,u8 data)
  {                
      u8 fault;
      //! generate start condition
      IIC_Start();
      //! generate 7bit slave address + 1bit write/read mode
      IIC_Send_Byte((AccSlaveAddr <<1) | WriteMode);
      //! wait until slave put ack
      fault = IIC_Wait_Ack();
      //! return 0 if not receive ack
      if(fault==0)   return(0);
      //.............
      //! put register address
      IIC_Send_Byte(reg);
      //! wait until slave put ack
      fault = IIC_Wait_Ack();  
      //! return 0 if not receive ack
      if(fault==0)   return(0);
      //.............
      //! put Data
      IIC_Send_Byte(data);
      //! wait until slave put ack
      fault = IIC_Wait_Ack();  
      //! return 0 if not receive ack
      if(fault==0)   return(0);
      //! generate stop condition
      IIC_Stop();
      return(1);              
  }
//=============================================================================================================================================
// The loop function will simply check for new data from the
//  accelerometer and print it out if it's available.
//=============================================================================================================================================
void loop(void)
{
  // Use the accel.available() function to wait for new data
  //  from the accelerometer.
  if (available())
  {
    // First, use read() to read the new variables:
    read();    
    // read() will update two sets of variables. 
    // * int's x, y, and z will store the signed 12-bit values 
    //   read out of the accelerometer.
    // * floats cx, cy, and cz will store the calculated 
    //   acceleration from those 12-bit values. These variables 
    //   are in units of g's.
    // Check the two function declarations below for an example
    // of how to use these variables.
    printCalculatedAccels();
    //printAccels(); // Uncomment to print digital readings
    
    // The library also supports the portrait/landscape detection
    //  of the MMA8452Q. Check out this function declaration for
    //  an example of how to use that.
    printOrientation();
    

    printAccels();
  }
}
//=========================================================================================================================================
// This function demonstrates how to use the readPL()
// function, which reads the portrait/landscape status of the
// sensor.
//=========================================================================================================================================
void printOrientation(void)
{
// accel.readPL() will return a  containing information
// about the orientation of the sensor. It will be either
// PORTRAIT_U, PORTRAIT_D, LANDSCAPE_R, LANDSCAPE_L, or
// LOCKOUT.
char pl = readPL();
switch (pl)
 {
  case PORTRAIT_U:
    PrintString("Portrait Up",12);
    PrintEnter() ;       
     break;
  case PORTRAIT_D:
    PrintString("Portrait Down",14);
    PrintEnter()  ;  
    break;
  case LANDSCAPE_R:
    PrintString("Landscape Right",15);
    PrintEnter();    
    break;
  case LANDSCAPE_L:
    PrintString("Landscape Left",14);
    PrintEnter();
    break;
  case LOCKOUT:
    PrintString("Flat",4);
    PrintEnter();
    break;
  }  
}
//=========================================================================================================================================
// The function demonstrates how to use the x, y and
//  z variables.
// Before using these variables you must call the read()
//  function!
//=========================================================================================================================================
void printAccels(void)
{
  PrintString("x=",2);PrintNum(x);
  PrintEnter();
  PrintString("y=",2);PrintNum(y);
  PrintEnter();
  PrintString("z=",2);PrintNum(z);
  PrintEnter();  
}
//=========================================================================================================================================
// This function demonstrates how to use the accel.cx, accel.cy,
//  and accel.cz variables.
// Before using these variables you must call the accel.read()
//  function!
//=========================================================================================================================================
void printCalculatedAccels(void)
{ 
  u32 X,Y,Z;
  X=cx*1000;
  Y=cy*1000;
  Z=cz*1000;
  
  
  PrintString("cx=",3);PrintNum(X);
  PrintEnter();
  PrintString("cy=",3);PrintNum(Y);
  PrintEnter();
  PrintString("cz=",3);PrintNum(Z);
  PrintEnter();
}
//=========================================================================================================================================
void FF_MTInit(void)
{
unsigned char CTRL_REG1_Data;
//X or Y > 3g using MFF Function 4g, 100 Hz ODR, Normal Mode
//Step 1: Put the device into Standby Mode: Register 0x2A CTRL_REG1
standby();
//Step 2: Set Configuration Register for Motion Detection by setting the “OR” condition OAE = 1, enabling
//X, Y, and the latch
writeRegister(0x15, 0xD8);
//Step 3: Threshold Setting Value for the Motion detection of > 3g
//Note: The step count is 0.063g/ count
//• 3g/0.063g = 47.6; //Round up to 48
writeRegister(0x17, 0x30);
//Step 4: Set the debounce counter to eliminate false readings for 100 Hz sample rate with a requirement
//of 100 ms timer.
//Note: 100 ms/10 ms (steps) = 10 counts
writeRegister(0x18, 0x0A);
//Step 5: Enable Motion/Freefall Interrupt Function in the System (CTRL_REG4)
writeRegister(0x2D, 0x04);
//Step 6: Route the Motion/Freefall Interrupt Function to INT1 hardware pin (CTRL_REG5)
writeRegister(0x2E, 0x04);
//Step 7: Put the device in Active Mode
active();  
}
//======================================================================================================================================  
//Step 8: Write Interrupt Service Routine Reading the System Interrupt Status and the Motion/Freefall
//Status
//======================================================================================================================================  
void            FF_MTIrqHandeler      (void)
{
 u8 Int_SourceSystem;  
 u8 IntSourceMFF=5;

//Determine source of interrupt by reading the system interrupt
Int_SourceSystem=readRegister(0x0C);
//Set up Case statement here to service all of the possible interrupts
if ((Int_SourceSystem & 0x20)==0x20)
   {
    //Perform an Action since Motion Flag has been set
    //Read the Motion/Freefall Function to clear the interrupt
    Transition_Detect();
    //Can parse out data to perform a specific action based on the
    //axes that made the condition true and read the direction of the
    //motion event 
    PrintNum(IntSourceMFF); 
   }
    
}
//======================================================================================================================================
void FF_MTInit2(void)
{
 u8  CTRL_REG1_Data;
//X AND Y AND Z < 0.2g using MFF Function, 50 Hz ODR
//Step 1: Put the device in Standby Mode: Register 0x2A CTRL_REG1
writeRegister(0x2A, 0x20); //Set the device in 50 Hz ODR, Standby
//Step 2: Configuration Register set for Freefall Detection enabling “AND” condition, OAE = 0, Enabling X,
//Y, Z and the Latch
writeRegister(0x15, 0xB8);
//Step 3: Threshold Setting Value for the resulting acceleration < 0.2g
//Note: The step count is 0.063g/count
//• 0.2g/0.063g = 3.17 counts //Round to 3 counts
writeRegister(0x17, 0x03);
//Step 4: Set the debounce counter to eliminate false positive readings for 50Hz sample rate with a
//requirement of 120 ms timer, assuming Normal Mode.
//Note: 120 ms/20 ms (steps) = 6 counts
writeRegister(0x18, 0x06);
//Step 5: Enable Motion/Freefall Interrupt Function in the System (CTRL_REG4)
writeRegister(0x2D, 0x04);
//Step 6: Route the Motion/Freefall Interrupt Function to INT2 hardware pin (CTRL_REG5)
writeRegister(0x2E, 0x00);
//Step 7: Put the device in Active Mode, 50 Hz
CTRL_REG1_Data=readRegister(0x2A);
CTRL_REG1_Data |= 0x01;
writeRegister(0x2A,CTRL_REG1_Data);
}


//==============================================================================================================================================
//! Transiton with itterrupt
//==============================================================================================================================================
void Transition_WithIntrrupt(void)
{
u8 CTRL_REG1_Data;
//Change in X or Y > 0.5g for 50 ms at 100 Hz ODR, Normal mode
//Step 1: Put the device in Standby Mode: Register 0x2A CTRL_REG1
writeRegister(0x2A, 0x18); //Set device in 100 Hz ODR, Standby
//Step 2: Enable X and Y Axes and enable the latch: Register 0x1D Configuration Register
writeRegister (0x1D, 0x1E);// Enable Latch, Enable X and Enable Y
//Step 3: Set the Threshold: Register 0x1F
//Note: Step count is 0.063g per count
//0.5g / 0.063g = 7.93. Therefore set the threshold to 8 counts
writeRegister(0x1F, 0x04);
//Step 4: Set the Debounce Counter for 50 ms: Register 0x20
//Note: 100 Hz ODR, therefore 10 ms step sizes
writeRegister(0x20, 0x05);
//Step 5: Enable Transient Detection Interrupt in the System (CTRL_REG4)
writeRegister(0x2D, 0x20);
//Step 6: Route the Transient Interrupt to INT 1 hardware pin (CTRL_REG5)
writeRegister(0x2E, 0x20);
//Step 7: Put the device in Active Mode: Register 0x2A CTRL_REG1
CTRL_REG1_Data = readRegister(0x2A); //Read out the contents of the register
CTRL_REG1_Data |= 0x01; //Change the value in the register to Active Mode.
writeRegister(0x2A, CTRL_REG1_Data); //Write in the updated value to put the device in
//Active Mode
}
//==============================================================================================================================================
// This function will read the motion detection source register and
// print motion direction
//==============================================================================================================================================
void motionDetect(void)
{
 u8 source =readRegister(FF_MT_SRC);
  if((source >> 7) == 1) 
    {  // If Event Active flag set in the FF_MT_SRC register
     //  int deltat = millis() - count;
     if ((source & 0x02)==0x02)  // If XHE bit is set, x-motion detected
        {
         if ((source & 0x01)==0x01)  
            { // If XHP is 1, x event was negative g
             UsartSendData('-');
             UsartSendData('X'); 
            }             
         else 
            {
             UsartSendData('+');
             UsartSendData('X'); 
            }
        }
     if ((source & 0x08)==0x08)  // If XHE bit is set, x-motion detected
        {
         if ((source & 0x04)==0x04)  
            { // If YHP is 1, y event was negative g
             UsartSendData('-');
             UsartSendData('Y'); 
            }          
         else 
            {
             UsartSendData('+');
             UsartSendData('Y'); 
            }
        }
     if ((source & 0x20)==0x20)  // If XHE bit is set, x-motion detected
        {
         if ((source & 0x10)==0x10)  
            { // If XHP is 1, x event was negative g
             UsartSendData('-');
             UsartSendData('Z'); 
            }
         else 
            {
             UsartSendData('+');
             UsartSendData('Z'); 
            }
        }
  }
}



//=====================================================================================================================================================
void Transition_Detect(void)
{
 u8 source =readRegister(TRANSIENT_SRC);
  if((source >> 6) == 1) 
    {  // If Event Active flag set in the FF_MT_SRC register
     //  int deltat = millis() - count;
     if ((source & 0x02)==0x02)  // If XHE bit is set, x-motion detected
        {
         if ((source & 0x01)==0x01)  
            { // If XHP is 1, x event was negative g
             UsartSendData('-');
             UsartSendData('X'); 
            }             
         else 
            {
             UsartSendData('+');
             UsartSendData('X'); 
            }
        }
     if ((source & 0x08)==0x08)  // If XHE bit is set, x-motion detected
        {
         if ((source & 0x04)==0x04)  
            { // If YHP is 1, y event was negative g
             UsartSendData('-');
             UsartSendData('Y'); 
            }          
         else 
            {
             UsartSendData('+');
             UsartSendData('Y'); 
            }
        }
     if ((source & 0x20)==0x20)  // If XHE bit is set, x-motion detected
        {
         if ((source & 0x10)==0x10)  
            { // If XHP is 1, x event was negative g
             UsartSendData('-');
             UsartSendData('Z'); 
            }
         else 
            {
             UsartSendData('+');
             UsartSendData('Z'); 
            }
        }
  }
  
}
//=====================================================================================================================================================
void initMMA8452(void)
{
  standby();  // Must be in standby to change registers
  // Set up the full scale range to 2, 4, or 8g.
  if ((fsr==2)||(fsr==4)||(fsr==8))
    writeRegister(XYZ_DATA_CFG, fsr >> 2);  
  else
    writeRegister(XYZ_DATA_CFG, 0);

  // Setup the 3 data rate bits, from 0 to 7
  writeRegister(CTRL_REG1, readRegister(CTRL_REG1) & ~(0x38));
  if (odr <= 7)
    writeRegister(CTRL_REG1, readRegister(CTRL_REG1) | (odr << 3));  
    
// These settings have to do with setting up the sleep mode and should probably be broken up into a separate function
// set Auto-WAKE sample frequency when the device is in sleep mode

     writeRegister(0x29, 0x40 ); // sleep after ~36 seconds of inactivity at 6.25 Hz ODR

     writeRegister(CTRL_REG1, readRegister(CTRL_REG1) & ~(0xC0)); // clear bits 7 and 8
     writeRegister(CTRL_REG1, readRegister(CTRL_REG1) |  (0xC0)); // select 1.56 Hz sleep mode sample frequency for low power

  // set sleep power mode scheme
     writeRegister(CTRL_REG2, readRegister(CTRL_REG2) & ~(0x18)); // clear bits 3 and 4
     writeRegister(CTRL_REG2, readRegister(CTRL_REG2) |  (0x18)); // select low power mode
     
  // Enable auto SLEEP
     writeRegister(CTRL_REG2, readRegister(CTRL_REG2) & ~(0x04)); // clear bit 2
     writeRegister(CTRL_REG2, readRegister(CTRL_REG2) |  (0x04)); // enable auto sleep mode

  // set sleep mode interrupt scheme
     writeRegister(CTRL_REG3, readRegister(CTRL_REG3) & ~(0x3C)); // clear bits 3, 4, 5, and 6
     writeRegister(CTRL_REG3, readRegister(CTRL_REG3) |  (0x3C)); // select wake on transient, orientation change, pulse, or freefall/motion detect
     
   // Enable Auto-SLEEP/WAKE interrupt
     writeRegister(CTRL_REG4, readRegister(CTRL_REG4) & ~(0x80)); // clear bit 7
     writeRegister(CTRL_REG4, readRegister(CTRL_REG4) |  (0x80)); // select  Auto-SLEEP/WAKE interrupt enable
   
  // Set up portrait/landscape registers - 4 steps:
  // 1. Enable P/L
  // 2. Set the back/front angle trigger points (z-lock)
  // 3. Set the threshold/hysteresis angle
  // 4. Set the debouce rate
  // For more info check out this app note: http://cache.freescale.com/files/sensors/doc/app_note/AN4068.pdf
  writeRegister(PL_CFG, 0x40);        // 1. Enable P/L
 // writeRegister(PL_BF_ZCOMP, 0x44); // 2. 29deg z-lock (don't think this register is actually writable)
 // writeRegister(P_L_THS_REG, 0x84); // 3. 45deg thresh, 14deg hyst (don't think this register is writable either)
  writeRegister(PL_COUNT, 0x50);      // 4. debounce counter at 100ms (at 800 hz)

  /* Set up single and double tap - 5 steps:
   1. Set up single and/or double tap detection on each axis individually.
   2. Set the threshold - minimum required acceleration to cause a tap.
   3. Set the time limit - the maximum time that a tap can be above the threshold
   4. Set the pulse latency - the minimum required time between one pulse and the next
   5. Set the second pulse window - maximum allowed time between end of latency and start of second pulse
   for more info check out this app note: http://cache.freescale.com/files/sensors/doc/app_note/AN4072.pdf */
  writeRegister(PULSE_CFG, 0x7F);  // 1. enable single/double taps on all axes
  // writeRegister(PULSE_CFS, 0x55);  // 1. single taps only on all axes
  // writeRegister(PULSE_CFS, 0x6A);  // 1. double taps only on all axes
  writeRegister(PULSE_THSX, 0x04);  // 2. x thresh at 0.25g, multiply the value by 0.0625g/LSB to get the threshold
  writeRegister(PULSE_THSY, 0x04);  // 2. y thresh at 0.25g, multiply the value by 0.0625g/LSB to get the threshold
  writeRegister(PULSE_THSZ, 0x04);  // 2. z thresh at 0.25g, multiply the value by 0.0625g/LSB to get the threshold
  writeRegister(PULSE_TMLT, 0x30);  // 3. 2.55s time limit at 100Hz odr, this is very dependent on data rate, see the app note
  writeRegister(PULSE_LTCY, 0xA0);  // 4. 5.1s 100Hz odr between taps min, this also depends on the data rate
  writeRegister(PULSE_WIND, 0xFF);  // 5. 10.2s (max value)  at 100 Hz between taps max

  // Set up motion detection
//  writeRegister(FF_MT_CFG, 0x58); // Set motion flag on x and y axes
  writeRegister(FF_MT_CFG, 0xF8);
  writeRegister(FF_MT_THS, 0x84); // Clear debounce counter when condition no longer obtains, set threshold to 0.25 g
  writeRegister(FF_MT_COUNT, 0x8); // Set debounce to 0.08 s at 100 Hz

  // Set up interrupt 1 and 2
  writeRegister(CTRL_REG3, readRegister(CTRL_REG3) & ~(0x02)); // clear bits 0, 1 
  writeRegister(CTRL_REG3, readRegister(CTRL_REG3) |  (0x02)); // select ACTIVE HIGH, push-pull interrupts
     
 // writeRegister(0x2C, 0x02);  // Active high, push-pull interrupts

  writeRegister(CTRL_REG4, readRegister(CTRL_REG4) & ~(0x1D)); // clear bits 0, 3, and 4
  writeRegister(CTRL_REG4, readRegister(CTRL_REG4) |  (0x1D)); // DRDY, Freefall/Motion, P/L and tap ints enabled
   
  writeRegister(CTRL_REG5, 0x01);  // DRDY on INT1, P/L and taps on INT2

  active();  // Set to active to start reading
}
//==============================================================================================================================================
// This function will read the status of the tap source register.
// Print if there's been a single or double tap, and on what axis.
//==============================================================================================================================================
void tapHandler(void)
{
  u8 source = readRegister(PULSE_SRC);  // Reads the PULSE_SRC register

  if ((source & 0x10)==0x10)  // If AxX bit is set
  {
    if ((source & 0x08)==0x08)  // If DPE (double puls) bit is set
      {
       PrintString("Double Tap (2) on X",19);  // tabbing here for visibility
       PrintEnter();
      }
    else
      {
       PrintString("Single (1) tap on X",19);
       PrintEnter();
      }
    if ((source & 0x01)==0x01)  
      { // If PoIX is set
       PrintString("-X",2);
       PrintEnter();
      }
    else 
      {
       PrintString("+X",2);
       PrintEnter();
      }
  }
  if ((source & 0x20)==0x20)  // If AxY bit is set
  {
    if ((source & 0x08)==0x08)  // If DPE (double pulse) bit is set
      {
        PrintString("Double Tap (2) on Y",19);  // tabbing here for visibility
        PrintEnter();
      }
    else
      {
       PrintString("Single (1) tap on Y",19);
       PrintEnter();
      }
    if ((source & 0x02)==0x02) 
      { // If PoIY is set
       PrintString("-Y",2);
       PrintEnter();
      }
    else 
      {
       PrintString("+Y",2);
       PrintEnter();
      }
  }
  if ((source & 0x40)==0x40)  // If AxX bit is set
  {
    if ((source & 0x08)==0x08)  // If DPE (double puls) bit is set
      {
       PrintString("Double Tap (2) on Z",19);  // tabbing here for visibility
       PrintEnter();
      }
    else
      {
       PrintString("Single (1) tap on Z",19);
       PrintEnter();
      }
    if ((source & 0x40)==0x40)  
      { // If PoIX is set
       PrintString("-Z",2);
       PrintEnter();
      }
    else 
      {
       PrintString("+Z",2);
       PrintEnter();
      }
  }
}
//========================================================================================================================================================
// Set up sensor software reset
//========================================================================================================================================================
void MMA8452Reset() 
{
 writeRegister(CTRL_REG2, 0x40); // set reset bit to 1 to assert software reset to zero at end of boot process
}
//========================================================================================================================================================
// Allow user compensation of acceleration errors
//========================================================================================================================================================
void MMA8452Offsets()
{
   standby();  // Must be in standby to change registers   
   // Factory settings are pretty good; the settings below produce 1 mg error or less at 2 g full scale! For the device at rest on my table 
   // these values partially compensate for the slope of the table and the slope of the sensor in my breadboard. It is a pretty stable setup!
   // For negative values use 2's complement, i.e., -2 mg = 0xFF, etc.
   writeRegister(OFF_X, 0xF9); // X-axis compensation; this is -14 mg
   writeRegister(OFF_Y, 0x01); // Y-axis compensation; this is +2 mg
   writeRegister(OFF_Z, 0x00); // z-axis compensation; this is  0 mg adjustment   
   active();  // Set to active to start reading
}
//========================================================================================================================================================
// This function will read the p/l source register and
// print what direction the sensor is now facing
//========================================================================================================================================================
void portraitLandscapeHandler(void)
{
  u8 pl = readRegister(0x10);  // Reads the PL_STATUS register
  switch((pl&0x06)>>1)  // Check on the LAPO[1:0] bits
  {
  case 0:
    PrintString("Portrait up, ",14);
    break;
  case 1:
    PrintString("Portrait Down, ",15);
    break;
  case 2:
    PrintString("Landscape Right, ",17);
    break;
  case 3:
    PrintString("Landscape Left, ",16);
    break;
  }
  if (pl&0x01)  // Check the BAFRO bit
    PrintString("Back",4);
  else
    PrintString("Front",5);
  if (pl&0x40)  // Check the LO bit
    PrintString(", Z-tilt!",9);
  
}
//=================================================================================================================================================
void Orientation_Detect(void)
{
  u8 CTRL_REG1_Data;
  u8 PLCFG_Data;
  u8 PL_BF_ZCOMP_Data;
  u8 P_L_THS_Data;
  u8 CTRL_REG4_Data;
  u8 CTRL_REG5_Data;
//Step 1: Put the part into Standby Mode
CTRL_REG1_Data = readRegister(0x2A); //read contents of register
CTRL_REG1_Data &= 0xFE; //Set last bit to 0.
writeRegister(0x2A, CTRL_REG1_Data); //write the updated value in CTRL_REG1
//Step 2: Set the data rate to 50 Hz (for example, but can choose any sample rate).
CTRL_REG1_Data = readRegister(0x2A); //Note: Can combine this step with above
CTRL_REG1_Data &= 0xC7; //Clear the sample rate bits
CTRL_REG1_Data |= 0x20; //Set the sample rate bits to 50 Hz
writeRegister(0x2A, CTRL_REG1_Data); //Write updated value into the register.
//Step 3: Set the PL_EN bit in Register 0x11 PL_CFG. This will enable the orientation detection.
PLCFG_Data = readRegister (0x11);
PLCFG_Data |= 0x40;
writeRegister(0x11, PLCFG_Data);
//Step 4: Set the Back/Front Angle trip points in register 0x13 following the table in the data sheet.
//NOTE: This register is readable in all versions of MMA845xQ but it is only modifiable in the
//MMA8451Q.
PL_BF_ZCOMP_Data = readRegister(0x13);
PL_BF_ZCOMP_Data &= 0x3F; //Clear bit 7 and 6
//Select one of the following to set the B/F angle value:
PL_BF_ZCOMP_Data |= 0x00; //This does nothing additional and keeps bits [7:6] = 00
//PL_BF_ZCOMP_Data | = 0x40; //Sets bits[7:6] = 01
//PL_BF_ZCOMP_Data | = 0x80; //Sets bits[7:6] = 02
//PL_BF_ZCOMP_Data | = 0xC0; //Sets bits[7:6] = 03
writeRegister(0x13, PL_BF_ZCOMP_Data); //Write in the updated Back/Front Angle
//Step 5: Set the Z-Lockout angle trip point in register 0x13 following the table in the data sheet.
//NOTE: This register is readable in all versions of MMA845xQ but it is only modifiable in the
//MMA8451Q.
//PL_BF_ZCOMP_Data = readRegister(0x1C); //Read out contents of the register (can be read by all
//versions of MMA845xQ)
//The remaining parts of this step only apply to MMA8451Q
PL_BF_ZCOMP_Data &= 0xF8; //Clear the last three bits of the register
//Table 1. Registers of Importance for Configuring the Orientation Detection in the in the MMA8451, 2, 3Q
//R# Name Definition Bit 7 Bit 6 Bit 5 Bit 4 Bit 3 Bit 2 Bit 1 Bit 0
//0C INT_SOURCE Interrupt Status R SRC_ASLP SRC_FIFO SRC_TRANS SRC_LNDPRT SRC_PULSE SRC_FF_MT — SRC_DRDY
//10 PL_STATUS PL Status R NEWLP LO — — — LAPO[1] LAPO[0] BAFRO
//11 PL_CFG PL Configuration R/W DBCNTM PL_EN — — — — — —
//12 PL_COUNT PL Debounce R/W DBNCE[7] DBNCE[6] DBNCE[5] DBNCE[4] DDBNCE[3] DBNCE[2] DBNCE[1] DBNCE[0]
//13 PL_BF_ZCOMP PL Back/Front and
//Z Compensation R BKFR[1] BKFR[0] — — — ZLOCK[2] ZLOCK[1] ZLOCK[0]
//14 P_L_THS_REG Landscape to Portrait
//Threshold Setting R P_L_THS[4] P_L_THS[3] P_L_THS[2] P_L_THS[1] P_L_THS[0] HYS[2] HYS[1] HYS[0]
//2D CTRL_REG4 Control Reg4 R/W
//(Interrupt Enable Map) INT_EN_ASLP INT_EN_FIFO INT_EN_TRANS INT_EN_LNDPRT INT_EN_PULSE INT_EN_FF_MT — INT_EN_DRDY
//2E CTRL_REG5 Control Reg5 R/W
//(Interrupt Configuration) INT_CFG_ASLP INT_CFG_FIFO INT_CFG_TRANS INT_CFG_LNDPRT INT_CFG_PULSE INT_CFG_FF_MT — INT_CFG_DRDY
//Select one of the following to set the Z-lockout value
//PL_BF_ZCOMP_Data | = 0x00; //     14°  This does nothing additional but the Z-lockout selection will remain at

PL_BF_ZCOMP_Data |= 0x01; //Set the Z-lockout angle to 18°
//PL_BF_ZCOMP_Data | = 0x02; //Set the Z-lockout angle to 21°
//PL_BF_ZCOMP_Data | = 0x03; //Set the Z-lockout angle to 25°
//PL_BF_ZCOMP_Data | = 0x04; //Set the Z-lockout angle to 29°
//PL_BF_ZCOMP_Data | = 0x05; //Set the Z-lockout angle to 33°
//PL_BF_ZCOMP_Data | = 0x06; //Set the Z-lockout angle to 37°
//PL_BF_ZCOMP_Data | = 0x07; //Set the Z-lockout angle to 42°
writeRegister(0x13, PL_BF_ZCOMP_Data); //Write in the updated Z-lockout angle
//Step 6: Set the Trip Threshold Angle
//NOTE: This register is readable in all versions of MMA845xQ but it is only modifiable in the
//MMA8451Q.
//Select the angle desired in the table, and,
//Enter in the values given in the table for the corresponding angle.
//Refer to Figure 7 for the reference frame of the trip angles.
P_L_THS_Data = readRegister(0x14); //(can be read by all versions of MMA845xQ)
//The remaining parts of this step only apply to MMA8451Q
P_L_THS_Data &= 0x07; //Clear the Threshold values
//Choose one of the following options
P_L_THS_Data  |= (0x07)<<3; //Set Threshold to 15°
//P_L_THS_Data | = (0x09)<<3; //Set Threshold to 20°
//P_L_THS_Data | = (0x0C)<<3; //Set Threshold to 30°
//P_L_THS_Data | = (0x0D)<<3; //Set Threshold to 35°
//P_L_THS_Data | = (0x0F)<<3; //Set Threshold to 40°
//P_L_THS_Data | = (0x10)<<3; //Set Threshold to 45°
//P_L_THS_Data | = (0x13)<<3; //Set Threshold to 55°
//P_L_THS_Data | = (0x14)<<3; //Set Threshold to 60°
//P_L_THS_Data | = (0x17)<<3; //Set Threshold to 70°
//P_L_THS_Data | = (0x19)<<3; //Set Threshold to 75°
writeRegister(0x14, P_L_THS_Data);
//Step 7: Set the Hysteresis Angle
//NOTE: This register is readable in all versions of MMA845xQ but it is only modifiable in the
//MMA8451Q.
//Select the hysteresis value based on the desired final trip points (threshold + hysteresis)
//Enter in the values given in the table for that corresponding angle.
//Note: Care must be taken. Review the final resulting angles. Make sure there isn’t a resulting trip value
//greater than 90 or less than 0.
//The following are the options for setting the hysteresis.
P_L_THS_Data = readRegister(0x14);
//NOTE: The remaining parts of this step only apply to the MMA8451Q.
P_L_THS_Data &= 0xF8; //Clear the Hysteresis values
P_L_THS_Data |= 0x01; //Set Hysteresis to ±4°
//P_L_THS_Data | = 0x02; //Set Threshold to ±7°
//P_L_THS_Data | = 0x03; //Set Threshold to ±11°
//P_L_THS_Data | = 0x04; //Set Threshold to ±14°
//P_L_THS_Data | = 0x05; //Set Threshold to ±17°
//P_L_THS_Data | = 0x06; //Set Threshold to ±21°
//P_L_THS_Data | = 0x07; //Set Threshold to ±24°
writeRegister(0x14, P_L_THS_Data);
//Step 8: Register 0x2D, Control Register 4 configures all embedded features for interrupt
//detection.
//To set this device up to run an interrupt service routine:
//Program the Orientation Detection bit in Control Register 4.
//Set bit 4 to enable the orientation detection “INT_EN_LNDPRT”.
CTRL_REG4_Data = readRegister(0x2D); //Read out the contents of the register
CTRL_REG4_Data |= 0x10; //Set bit 4
writeRegister(0x2D, CTRL_REG4_Data); //Set the bit and write into CTRL_REG4
//Step 9: Register 0x2E is Control Register 5 which gives the option of routing the interrupt to
//either INT1 or INT2
//Depending on which interrupt pin is enabled and configured to the processor:
//Set bit 4 “INT_CFG_LNDPRT” to configure INT1, or,
//Leave the bit clear to configure INT2.
CTRL_REG5_Data = readRegister(0x2E);
//In the next two lines choose to clear bit 4 to route to INT2 or set bit 4 to route to INT1
CTRL_REG5_Data &= 0xEF; //Clear bit 4 to choose the interrupt to route to INT2
CTRL_REG5_Data |= 0x10; //Set bit 4 to choose the interrupt to route to INT1
writeRegister(0x2E, CTRL_REG5_Data); //Write in the interrupt routing selection
//Step 10: Set the debounce counter in register 0x12
//This value will scale depending on the application-specific required ODR.
//If the device is set to go to sleep, reset the debounce counter before the device goes to sleep. This setting
//helps avoid long delays since the debounce will always scale with the current sample rate. The debounce
//can be set between 50 ms - 100 ms to avoid long delays.
writeRegister(0x12, 0x05); //This sets the debounce counter to 100 ms at 50 Hz
//Step 11: Put the device in Active Mode
CTRL_REG1_Data = readRegister(0x2A); //Read out the contents of the register
CTRL_REG1_Data |= 0x01; //Change the value in the register to Active Mode.
writeRegister(0x2A, CTRL_REG1_Data); //Write in the updated value to put the device in Active Mode
//Step 12: Write a Service Routine to Service the Interrupt
}
//=================================================================================================================================================
void Auto_WakeSleep(void)
{
  u8 CTRL_REG1_Data;
  u8 CTRL_REG2_Data;
  u8 XYZ_CFG_Data;
//Step 1: Put the device in Standby Mode
//Register 0x2A CTRL_REG1
CTRL_REG1_Data = readRegister(0x2A);
CTRL_REG1_Data &= 0xFE; //Clear Active Bit
writeRegister(0x2A,CTRL_REG1_Data);
//Step 2: To enable the Auto-Wake/Sleep set bit 2 in Register 0x2B, the SLPE bit.
//Register 0x2B CTRL_REG2
CTRL_REG2_Data = readRegister(0x2B); //Store value in the Register
CTRL_REG2_Data |= 0x04; //Set the Sleep Enable bit
writeRegister(0x2B, CTRL_REG2_Data); //Write the updated value into CTRL_REG2.
//Step 3: The sleep sample rate must be chosen by writing in the corresponding sample rate value to bits
//6 and 7 ASLP_RATE0 and ASLP_RATE1 (01) and the Wake Sample rate bits 5, 4 and 3 to DR
//(011) in Register 0x2A.
//Register 0x2A CTRL_REG1 ASLP_RATE = 01 (6.25 Hz), DR = 011(100 Hz)
CTRL_REG1_Data = readRegister(0x2A);
CTRL_REG1_Data &= 0x5E; //clear the bits that should be cleared for the sample rates
CTRL_REG1_Data |= 0x58; //Set ASLP = 6.25 Hz, DR = 100 Hz
writeRegister(0x2A,CTRL_REG1_Data);
//Step 4: Set the Wake Oversampling Mode to High Resolution (10) and the Sleep Oversampling Mode to
//Low Power (11)
CTRL_REG2_Data = readRegister(0x2B);
CTRL_REG2_Data &= 0xE4; //puts both Oversampling modes in Normal Mode
CTRL_REG2_Data |= 0x1A; //Wake High Res, Sleep Low Power
writeRegister(0x2B,CTRL_REG2_Data);
//Step 5: The Interrupt for the event to trigger the device to wake up must be enabled by writing to
//Register 0x2D, CTRL_Reg4. Bits 2 through 7 will affect the Auto-Wake/sleep. The data ready
//interrupt doesn’t trigger the Auto-Wake/Sleep mechanism.
//Example: Set Pulse and Orientation and Motion 1 and Auto-Wake/Sleep Interrupts
//Enabled in the System
writeRegister(0x2D, 0x9C);
//Step 6: Route the interrupt chosen and enabled to either INT1 or INT2 in Register 0x2E CTRL_REG5.
//Example: Route Pulse, Motion1 and Orientation to INT2 and Auto-Sleep to INT1.
writeRegister(0x2E,0x80);
//Step 7: Enable the interrupts that will wake the device from sleep. There can be more interrupts enabled
//in Step 4 than in Step 6. Only interrupts that are Enabled in Step 4 and that have the “Wake-from
//-Sleep” bit set in Register 0x2C will actually wake the device.
//Example: Choose Pulse and Motion to wake the device from sleep
writeRegister(0x2C,0x18);
//Step 8: Set the Dynamic Range to 2g
//Register 0x0E XYZ_DATA_CFG
XYZ_CFG_Data = readRegister(0x0E);
XYZ_CFG_Data &= 0xFC; //Clear the FS bits to 00 2g
writeRegister(0x0E, XYZ_CFG_Data);
//Step 9: Write an Interrupt Service routine to monitor the Auto-Sleep Interrupt
}
//******************************************************************************


void AccInit (void)
  {
    u8 data,result;
    
    u16 AccTimeout = 5;
    
    // Check if Accelerometer is installed correctly and ready for configuration
    data =  readRegister (WHO_AM_I);
    //-------------------------------------------    
    // Set the device in Standby Mode
    writeRegister (CTRL_REG1,0x00);
    //-------------------------------------------
    // Portrait/Landscape Detection is Enabled
    data=readRegister (PL_CFG);
    data |= 0x40;
    writeRegister (PL_CFG,data);
    //-------------------------------------------
    // Portrait/Landscape Debounce Count Value  
    writeRegister (PL_COUNT,0x05);    
    
    //-------------------------------------------  
    // Motion Flag Enabled - Event flag enabled on Y event
    writeRegister (FF_MT_CFG,0xD0);
    
    // Motion Threshold Value
    writeRegister (FF_MT_THS,0x11);
    
    // Motion Debounce Register
    writeRegister (FF_MT_COUNT,0x07);
    //-------------------------------------------
    //Set the device in 1.56Hz ODR, Standby Mode
    data=readRegister (CTRL_REG1);
    data &= 0xC6;
    data |= 0x38;      // ODR = 1.56 Hz
    data |= 0xC0;       //ASLPR = 1.56
    writeRegister (CTRL_REG1,data);
    //-------------------------------------------
    // Auto sleep enabled
    //data = readRegister (CTRL_REG2,&data);
    data = 0x00;
    data |= 0x04;  // Enable Auto sleep
    data |= 0x18;  // Low power
    writeRegister (CTRL_REG2,data);
    //-------------------------------------------    
    // Orientation - Motion function can wake up  system
    data = 0x28;
    writeRegister (CTRL_REG3,data); 
    //-------------------------------------------
    // Orientation Detection interrupt enable
    // Motion Detection interrupt enable      
    data = 0x14;
    writeRegister (CTRL_REG4,data);
    //-------------------------------------------
    // Orientation Interrupts routed to INT1
    // Motion Interrupts routed to INT1
    data = 0x14;
    writeRegister (CTRL_REG5,data);
    // 2g
    data = readRegister (XYZ_DATA_CFG);
    data &= 0xFC;
    writeRegister (XYZ_DATA_CFG,data);
    //-------------------------------------------
    //Set the device in 12.5Hz ODR, Active
    data = readRegister (CTRL_REG1);
    // data |= 0xC0; // Sleep Rate = 1.56 Hz
    // data |= 0x38; // Output Data Rate = 12.5 Hz
    data |= 0x01; // Active Mode Enable
    writeRegister (CTRL_REG1,data);
    
    
    // Set the device in Standby Mode
    // writeRegister (CTRL_REG1,0x00);
    
  }