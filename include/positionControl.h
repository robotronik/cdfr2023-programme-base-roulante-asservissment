#pragma once 

#include <math.h>
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

    uint32_t PreviousTime;

public:
    positionControl(double positionDepart  = 0.0);
    void setPostion(double setConsigne);
    double getPostion();
    bool servoMove();
    ~positionControl();
private:
    void calculVitesse();
};
