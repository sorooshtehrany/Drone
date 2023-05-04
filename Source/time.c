#include "..\Include\time.h"
//******************************************************************************
//! Variable -------------------------------------------------------------------

//******************************************************************************
//! \brief  init rtc and set date and time
//! \param  initTime : time and date set
//******************************************************************************
void timeInit( sTime initTime )
{   
    RTC_InitTypeDef   RTC_InitStr;
    RTC_TimeTypeDef   RTC_TimeStr;
    RTC_DateTypeDef   RTC_DateStr;
    
    //! Select LSE (32.768 KHz) as RTC clock source 
    CLK_RTCClockConfig(CLK_RTCCLKSource_LSE, CLK_RTCCLKDiv_1);

    CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);
    
    RTC_InitStr.RTC_HourFormat = RTC_HourFormat_24;
    RTC_InitStr.RTC_AsynchPrediv = 0x7F;
    RTC_InitStr.RTC_SynchPrediv = 0x00FF;
    RTC_Init(&RTC_InitStr);

    RTC_DateStructInit(&RTC_DateStr);
    RTC_DateStr.RTC_WeekDay = RTC_Weekday_Monday;
    RTC_DateStr.RTC_Date = initTime.day;
    RTC_DateStr.RTC_Month = (RTC_Month_TypeDef)(initTime.month);
    RTC_DateStr.RTC_Year = initTime.year;
    RTC_SetDate(RTC_Format_BIN, &RTC_DateStr);

    RTC_TimeStructInit(&RTC_TimeStr);
    RTC_TimeStr.RTC_Hours   = initTime.hour;
    RTC_TimeStr.RTC_Minutes = initTime.minute;
    RTC_TimeStr.RTC_Seconds = initTime.second;
    RTC_SetTime(RTC_Format_BIN, &RTC_TimeStr);
}
//******************************************************************************
//! \brief  set date and time
//! \param  initTime : time and date set
//******************************************************************************
void timeSet( sTime time )
{
    RTC_TimeTypeDef   RTC_TimeStr;
    RTC_DateTypeDef   RTC_DateStr;
    
    RTC_DateStructInit(&RTC_DateStr);
    RTC_DateStr.RTC_WeekDay = RTC_Weekday_Monday;
    RTC_DateStr.RTC_Date = time.day;
    RTC_DateStr.RTC_Month = (RTC_Month_TypeDef)(time.month);
    RTC_DateStr.RTC_Year = time.year;
    RTC_SetDate(RTC_Format_BIN, &RTC_DateStr);

    RTC_TimeStructInit(&RTC_TimeStr);
    RTC_TimeStr.RTC_Hours   = time.hour;
    RTC_TimeStr.RTC_Minutes = time.minute;
    RTC_TimeStr.RTC_Seconds = time.second;
    RTC_SetTime(RTC_Format_BIN, &RTC_TimeStr);
}
//******************************************************************************
//! \brief  get date and time
//! \param  initTime : time and date get
//******************************************************************************
void timeGet( sTime *time )
{
    RTC_TimeTypeDef   RTC_TimeStr;
    RTC_DateTypeDef   RTC_DateStr;
    
    RTC_DateStructInit(&RTC_DateStr);
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStr);
    //RTC_DateStr.RTC_WeekDay = RTC_Weekday_Friday;
    time->day = RTC_DateStr.RTC_Date;
    time->month = RTC_DateStr.RTC_Month;
    time->year = RTC_DateStr.RTC_Year;
    
    RTC_TimeStructInit(&RTC_TimeStr);
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStr); 
    time->hour = RTC_TimeStr.RTC_Hours;
    time->minute = RTC_TimeStr.RTC_Minutes;
    time->second = RTC_TimeStr.RTC_Seconds;
     
}