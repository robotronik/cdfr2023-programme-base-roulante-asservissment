#pragma once

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <math.h>

#include "config.h"
#include "uart.h"

#define _BUFFERSIZE 512

// theorical distance between the 2 wheel : 298.765mm
// theorical diametre of the wheel : 39mm
// Number of step for each resolution : 360 step

// STEPANGLE = (RayonRoueCodeuse * 360) / (NbDePasRoueCodeuse * LargeurEntraxeDes2RoueCodeuses * 2)
// STEPAVANCE = (RayonRoueCodeuse * 360)/(NbDePasRoueCodeuse*2)

const double NUMBERSTEPBYROTATION = 360;
const double DISTANCEWHEEL = 300;
const double DIAMETERWHEELD = 41.3;
const double DIAMETERWHEELG = 41.2;

const double STEPANGLED  = ((DIAMETERWHEELD*180)/(NUMBERSTEPBYROTATION*DISTANCEWHEEL));
const double STEPANGLEG  = ((DIAMETERWHEELG*180)/(NUMBERSTEPBYROTATION*DISTANCEWHEEL));
const double STEPAVANCED = ((DIAMETERWHEELD*M_PI)/(NUMBERSTEPBYROTATION*2));
const double STEPAVANCEG = ((DIAMETERWHEELG*M_PI)/(NUMBERSTEPBYROTATION*2));

const double DEG2RAD = M_PI/180.0;
const double RAD2DEG = 180.0/M_PI;

enum class odometrieTrigger : char {fordwardL,backwardL,fordwardR,backwardR};

void odometrieSetup(void);
void printBuffer(void);
void printPosition(void);
void odometrieLoop(void);
position_t odometrieGetPosition(void);
position_u odometrieGetPositionInt(void);
void odometrieSetPosition(position_u positionUnion);
