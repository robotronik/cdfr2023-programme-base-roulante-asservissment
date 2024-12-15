#pragma once

#include <math.h>
#include <stdint.h>

#include "uart.h"
#include "clock.h"
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

    uint32_t PreviousTime;

    bool stopStatus;

public:
    positionControl(double positionDepart  = 0.0);
    void reset(double initialValue);
    void stop(void);
    void setPosition(double initialValue);
    void setConsigne(double setConsigne);
    double getPostion();
    void setConsigne(double setConsigne, int timems);
    double getPostion(int timems);
    bool getMove();
    int getBrakingDistance();
    bool getStatus(void);
    ~positionControl();
private:
    void calculVitesse();
};
