#pragma once

#include "robot.h"
#include "config.h"
#include "PID.h"
#include "uart.h"
#include "positionControl.h"


class Asservissement
{
private:
    /* data */
    PID pidLineaire;
    PID pidAngulaire;
    positionControl positionControlLineaire;
    positionControl positionControlAngulaire;
    position_t consigne;
    robot* robotAsservi;
public:
    Asservissement(robot* bot);
    motorSpeed_t asservissementLoop(robot* robot);
    void test();
    double getAngularError(void);
    double getLinearError(void);
    ~Asservissement();
};