#pragma once

#include <math.h>
#include <stdint.h>

#ifndef SIMULATION_POSITION_CONTROL
    #include "clock.h"
#endif
#include "protectedMaxValue.h"

class positionControl{
public:
    /* data */
    double position;
    double consigne;
    double vitesse;

    protectedMaxValue<double> vitesseMaxAv = 45;
    protectedMaxValue<double> accelerationMaxAv = 25;
    protectedMaxValue<double> decelerationMaxAv = 25;
    protectedMaxValue<double> vitesseMaxAr = 45;
    protectedMaxValue<double> accelerationMaxAr = 25;
    protectedMaxValue<double> decelerationMaxAr = 25;
    double deltaTemps = 0;
    bool decelationLineair = true;
    double decelerationStop = 0;
    double maxSpeedOut = 0;

    uint32_t PreviousTime;

    bool stopStatus;

public:
    positionControl(double positionDepart  = 0.0);
    void reset(double initialValue);
    void stop(void);
    void setPosition(double initialValue);
    void setConsigne(double setConsigne);
    void setMaxSpeedOut(double max);
#ifndef SIMULATION_POSICONT
    double getPostion();
#endif
    double getPostion(uint32_t timems);
    bool getMove();
    int getBrakingDistance();
    bool getStatus(void);
    ~positionControl();
private:
    void calculVitesse();
};
