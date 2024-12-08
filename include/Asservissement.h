#pragma once

#include "config.h"
#include "PID.h"
#include "uart.h"
#include "positionControl.h"
#include "position.h"



class Asservissement
{
private:
    /* data */
    PID pidLineaire;
    PID pidAngulaire;
    PID pidLineaireBlock;
    PID pidAngulaireBlock;
    position posRobot;
    Rotation currentState;
    position_t consigne;

    double getAngularErrorReel(void);
    double getLinearErrorReel(void);

public :
    positionControl positionControlLineaire;
    positionControl positionControlAngulaire;

public:
    Asservissement();
    motorSpeed_t asservissementLoop(position positionRobot);
    void reset(void);

    void setConsigneStop(void);
    void setConsigne(position_t position);

    double getAngularError(void);
    double getLinearError(void);

    bool robotMovingIsFinish(void);
    bool robotTurningIsFinish(void);
    bool robotRunningIsFinish(void);
    int getBrakingDistance(void);
    Rotation getRotationSide(void);
    Direction getDirectionSide(void);
    position_t getCurrentConsigne(void);

    ~Asservissement();

private:
    void setConsigneAngulaire(double angle,Rotation rotation);
    void setConsigneLineaire(double x, double y);

protected:
    void setProtectedConsigneLineaire(double x, double y);
    void setConsigneLookAtForward(double x, double y, Rotation rotation);
    void setConsigneLookAtBackward(double x, double y, Rotation rotation);
    void setProtectedConsigneAngulaire(double angle, Rotation rotation);
};