#pragma once

#ifdef SIMULATION
    #include <hardware_interface.h>
#else
    #include <libopencm3/cm3/nvic.h>
    #include <libopencm3/stm32/exti.h>
#endif

#include <math.h>

#include "config.h"
#include "clock.h"
#include "uart.h"
#include "robot.h"

#define _BUFFERSIZE 512
#define PI 3.14159265359

// Number of step for each resolution : 360 step
#define NUMBERSTEPBYROTATION 360

// theorical distance between the 2 wheel : 298.765mm
#define DISTANCEWHEEL 300 

// theorical diametre of the wheel : 39mm
#define DIAMETER_WHEEL_R 41.12                // +1 roue = +25 distance 
#define DIAMETER_WHEEL_L 41.410 // + = y -     // +1 roue = +25 distance

#define STEP_ANGLE_R ((DIAMETER_WHEEL_R*180)/(NUMBERSTEPBYROTATION*DISTANCEWHEEL))
#define STEP_ANGLE_L ((DIAMETER_WHEEL_L*180)/(NUMBERSTEPBYROTATION*DISTANCEWHEEL)) 
#define STEP_FWD_R ((DIAMETER_WHEEL_R*PI)/(NUMBERSTEPBYROTATION*2))
#define STEP_FWD_L ((DIAMETER_WHEEL_L*PI)/(NUMBERSTEPBYROTATION*2))

typedef enum{fordwardL=0,backwardL,fordwardR,backwardR}odometrieTrigger_t;

void odometrieSetup(void);
void printBuffer(void);
void printPosition(void);
void odometrieLoop(position_t position, position_t* updatedPos);
position_t odometrieGetPosition(void);
position_u odometrieGetPositionInt(void);
void odometrieSetPosition(position_u positionUnion);
