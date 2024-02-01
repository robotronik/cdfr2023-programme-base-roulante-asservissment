#pragma once

#include "robot.h"
#include "config.h"
#include "PID.h"
#include "uart.h"
#include "positionControl.h"


typedef enum {
    ROTATION_TRIGO,
    ROTATION_HORRAIRE,
    ROTATION_DIRECT
} sensRotation_t;

class Asservissement
{
private:
    /* data */
    PID pidLineaire;
    PID pidAngulaire;
    positionControl positionControlLineaire;
    positionControl positionControlAngulaire;
    robot* robotAsservi;
    sensRotation_t currentState;

public:
    position_t consigne;

    Asservissement(robot* bot);
    motorSpeed_t asservissementLoop(void);

    void setConsigneAngulaire(double angle,sensRotation_t rotation);
    void setConsigneLineaire(double x, double y);
    void setConsigneLookAt(double x, double y, sensRotation_t rotation);
    void setConsigneStop(void);
    void setConsigne(position_t position);
   
    double getAngularError(void);
    double getLinearError(void);

    bool robotIsMoving(void);
    bool robotIsTurning(void);
    bool robotIsRunning(void);
    ~Asservissement();
};