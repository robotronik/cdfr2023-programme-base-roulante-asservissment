#pragma once

#include <math.h>
#include <stdint.h>

#include "uart.h"
#include "clock.h"

class positionControl{
public:
    /* data */
    double position;
    double consigne;
    double vitesse;

    double vitesseMaxAv = 45;
    double accelerationMaxAv = 25;
    double decelerationMaxAv = 25;
    double vitesseMaxAr = 45;
    double accelerationMaxAr = 25;
    double decelerationMaxAr = 25;
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
