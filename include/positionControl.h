#pragma once

#include <math.h>
#include <stdint.h>

#ifndef SIMULATION_POSITION_CONTROL
    #include "clock.h"
    #include "uart.h"
#else
    void SET_TIME_(uint32_t time);
    extern uint32_t TIME_;
#endif
#include "protectedMaxValue.h"

class positionControl{
private:
    /* data */
    double position;
    double consigne;
    double vitesse;

    double t_accel;
    double t_cruise;
    double t_decel;

    double d_accel;
    double d_cruise;
    double d_decel;

    uint32_t startTimeMs;
    double startPosition;
    double startSpeed;

    double maxSpeedOut = 0;

    bool stopStatus;

public:
    protectedMaxValue<double> vitesseMaxAv;
    protectedMaxValue<double> accelerationMaxAv;
    protectedMaxValue<double> decelerationMaxAv;
    protectedMaxValue<double> vitesseMaxAr;
    protectedMaxValue<double> accelerationMaxAr;
    protectedMaxValue<double> decelerationMaxAr;

public:
    positionControl(double positionDepart  = 0.0);
    ~positionControl();

    void reset(double initialValue);
    void stop(void);
    void setPosition(double initialValue);
    void setConsigne(double setConsigne);
    void setMaxSpeedOut(double max);

    double getPostion();
    bool getMove();
    int getBrakingDistance();
    bool getStatus(void);

private:
    void updateSpeed();
    void updatePosition();
    double getMaxSpeedOut();
    void computeStroke();
    void computeDT(double distance,double vitesseMax, double acceleration, double deceleration);
};
