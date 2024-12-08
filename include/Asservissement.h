#pragma once

#include "config.h"
#include "PID.h"
#include "uart.h"
#include "positionControl.h"
#include "position.h"
#include "motor.h"



class Asservissement
{
private:
    /* data */
    PID pidLineaire;
    PID pidAngulaire;
    PID pidLineaireBlock;
    PID pidAngulaireBlock;
    Rotation currentState;
    position_t consigne;

    double getAngularErrorReel(void);
    double getLinearErrorReel(void);

    uint32_t nextTime;
    int loopPeriod = 50;

    position* posRobot;

public :
    positionControl positionControlLineaire;
    positionControl positionControlAngulaire;

public:
    Asservissement(position* pos);

    void loop();
    void asservissementLoop();
    void setAsservissementLoopPeriod(int period);

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