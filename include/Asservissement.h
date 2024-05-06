#pragma once

#include "robot.h"
#include "config.h"
#include "PID.h"
#include "uart.h"
#include "positionControl.h"


typedef enum {
    ROTATION_DIRECT = 0,
    ROTATION_TRIGO = 1,
    ROTATION_HORRAIRE = 2
} sensRotation_t;

class Asservissement
{
private:
    /* data */
    PID pidLineaire;
    PID pidAngulaire;
    PID pidLineaireBlock;
    PID pidAngulaireBlock;
    robot* robotAsservi;
    sensRotation_t currentState;
    position_t consigne;

    double getAngularErrorReel(void);
    double getLinearErrorReel(void);

public :
    positionControl positionControlLineaire;
    positionControl positionControlAngulaire;

public:
    

    Asservissement(robot* bot);
    motorSpeed_t asservissementLoop(void);
    void reset(void);

    void setConsigneAngulaire(double angle,sensRotation_t rotation);
    void setConsigneLineaire(double x, double y);
    void setConsigneLookAtForward(double x, double y, sensRotation_t rotation);
    void setConsigneLookAtBackward(double x, double y, sensRotation_t rotation);
    void setProtectedConsigneAngulaire(double angle, sensRotation_t rotation);
    void setProtectedConsigneLineaire(double x, double y);
    void setConsigneStop(void);
    void setConsigne(position_t position);

    double getAngularError(void);
    double getLinearError(void);

    bool robotMovingIsFinish(void);
    bool robotTurningIsFinish(void);
    bool robotRunningIsFinish(void);
    int getBrakingDistance(void);
    ~Asservissement();
};