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
    double target;
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

    double decelerationAv;
    double decelerationAr;
    double maxSpeedOut = 0;

public:
    protectedMaxValue<double> vitesseMaxAv;
    protectedMaxValue<double> accelerationMaxAv;
    protectedMaxValue<double> decelerationMaxAv;
    protectedMaxValue<double> vitesseMaxAr;
    protectedMaxValue<double> accelerationMaxAr;
    protectedMaxValue<double> decelerationMaxAr;

    protectedMaxValue<double> decelerationStopAv;
    protectedMaxValue<double> decelerationStopAr;

public:
    positionControl(double positionDepart  = 0.0);
    ~positionControl();

    void reset(double initialValue);
    void stop(void);
    void setPosition(double initialValue);
    void setTarget(double setTarget);
    void setMaxSpeedOut(double max);

    double getPostion();
    bool getMove();
    int getBrakingDistance();

private:
    void updateSpeed();
    void updatePosition();
    double getMaxSpeedOut();
    void computeStroke(bool forceStop = false);
    void computeDT(double distance,double vitesseMax, double acceleration, double deceleration);
};
