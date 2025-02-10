#pragma once

#ifdef SIMULATION
    #include <hardware_interface.h>
#else
    #include <libopencm3/cm3/nvic.h>
    #include <libopencm3/stm32/exti.h>
#endif

#include <math.h>

#include "circularBufferOdo.h"
#include "config.h"
#include "clock.h"
#include "uart.h"
#include "AsservissementMath.h"

#define _BUFFERSIZE 50000

// theorical distance between the 2 wheel : 298.765mm
// theorical diametre of the wheel : 39mm
// Number of step for each resolution : 360 step

// STEPANGLE = (RayonRoueCodeuse * 360) / (NbDePasRoueCodeuse * LargeurEntraxeDes2RoueCodeuses * 2)
// STEPAVANCE = (RayonRoueCodeuse * 360)/(NbDePasRoueCodeuse*2)

#define STEPANGLED (((DIAMETERWHEELD)*180)/((NUMBERSTEPBYROTATION)*(DISTANCEWHEEL)))
#define STEPANGLEG (((DIAMETERWHEELG)*180)/((NUMBERSTEPBYROTATION)*(DISTANCEWHEEL)))
#define STEPAVANCED (((DIAMETERWHEELD)*(PI))/((NUMBERSTEPBYROTATION)*2))
#define STEPAVANCEG (((DIAMETERWHEELG)*(PI))/((NUMBERSTEPBYROTATION)*2))

#define COMPUTE_STEPANGLE(DIAMETER,DISTANCE)    (((DIAMETER)*180)/((NUMBERSTEPBYROTATION)*(DISTANCE)))
#define COMPUTE_STEPAVANCE(DIAMETER)            (((DIAMETER)*PI)/((NUMBERSTEPBYROTATION)*2))


#define COEFCONVDEGRETORADIAN (PI/180)

typedef enum{fordwardL=0,backwardL,fordwardR,backwardR}odometrieTrigger_t;

typedef struct
{
    double stepAngleD;
    double stepAngleG;
    double stepForrwardG;
    double stepForrwardD;
}odometrieParam_t;

void odometrieSetup(void);
void odometrieLoop(position_t &position);

void startCalibration(void);
void sectionCalibration(void);
void stopCalibration(void);
void computeCalibration(void);
