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
    robot* robotAsservi;
public:
    position_t consigne;

    Asservissement(robot* bot);
    motorSpeed_t asservissementLoop(double AttenuationErrorLineaire, double AttenuationErrorAngulaire);
    void setConsigne(position_t positionEntrante);
   
    double getAngularError(void);
    double getLinearError(void);
    ~Asservissement();
};