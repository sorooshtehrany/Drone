#ifndef MAIN_STR_H
#define MAIN_STR_H

//******************************************************************************
//! Library --------------------------------------------------------------------
#include <stdio.h>
#include "..\STM8L Library\Include\stm8l15x.h"
#include "MRF24J40.h"
#include "usart.h"
//******************************************************************************
//! Define ---------------------------------------------------------------------
#define MAINSTR_WORK_MODE_STOP      0
#define MAINSTR_WORK_MODE_START     1
#define MAINSTR_WORK_MODE_PAUSE     2

//! network protocol -----------------------------------------------------------
//! ping
#define MAINSTR_PING_REQ_CMD                    1
#define MAINSTR_PING_RES_CMD                    101
//! get setting
#define MAINSTR_GET_SETTING_REQ_CMD             2       
#define MAINSTR_GET_SETTING_RES_CMD             102
//! set ID
#define MAINSTR_SET_ID_REQ_CMD                  10
#define MAINSTR_SET_ID_RES_CMD                  110
//! set led normal mode
#define MAINSTR_SET_LED_NORMAL_REQ_CMD          11
#define MAINSTR_SET_LED_NORMAL_RES_CMD          111
//! set led danger mode
#define MAINSTR_SET_LED_DANGER_REQ_CMD          12
#define MAINSTR_SET_LED_DANGER_RES_CMD          112
//! set game time lengh
#define MAINSTR_SET_GAME_TIME_REQ_CMD           13
#define MAINSTR_SET_GAME_TIME_RES_CMD           113
//! set fire limit
#define MAINSTR_SET_FIRE_LIMIT_REQ_CMD          14
#define MAINSTR_SET_FIRE_LIMIT_RES_CMD          114
//! reset vest all setting
#define MAINSTR_RESET_ALL_SETTING_REQ_CMD       15
#define MAINSTR_RESET_ALL_SETTING_RES_CMD       115
//! start game
#define MAINSTR_START_GAME_REQ_CMD              20
#define MAINSTR_START_GAME_RES_CMD              120
//! pause game
#define MAINSTR_PAUSE_GAME_REQ_CMD              21
#define MAINSTR_PAUSE_GAME_RES_CMD              121
//! stop game
#define MAINSTR_STOP_GAME_REQ_CMD               22
#define MAINSTR_STOP_GAME_RES_CMD               122
//! get fire data
#define MAINSTR_GET_FIRE_DATA_REQ_CMD           30
#define MAINSTR_GET_FIRE_DATA_RES_CMD           130
//! get fire counter
#define MAINSTR_GET_FIRE_COUNTER_REQ_CMD        31
#define MAINSTR_GET_FIRE_COUNTER_RES_CMD        131
//! clear data
#define MAINSTR_CLEAR_DATA_REQ_CMD              32
#define MAINSTR_CLEAR_DATA_RES_CMD              132
//******************************************************************************
//! Variable -------------------------------------------------------------------

//******************************************************************************
//! Function -------------------------------------------------------------------
void MainStrInit            ( void );
void MainStrMain            ( void );
void MainStrUsartDataCheck  ( void );
void MainStrUsartDataSend   ( void );
void MainStrRfDataCheck     ( void );
void MainStrRfDataSend      ( void );
void MainStrSetWorkMode     ( u8 bWorkMode );
//******************************************************************************

#endif // MAIN_STR_H