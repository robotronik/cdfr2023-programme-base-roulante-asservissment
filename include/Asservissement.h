#pragma once

#include "config.h"
#include "types/PID.h"
#include "uart.h"
#include "positionControl.h"
#include "position.h"


class Asservissement
{
private:
    /* data */
    Rotation currentState;
    position_t target;

    double getAngularErrorReel(void);
    double getLinearErrorReel(void);

    uint32_t nextTime;
    int loopPeriod = 50;

public :
    positionControl positionControlLineaire;
    positionControl positionControlAngulaire;
    PID pidLineaire;
    PID pidAngulaire;
    PID pidLineaireBlock;
    PID pidAngulaireBlock;

public:
    Asservissement();

    void loop();
    void asservissementLoop();
    void setAsservissementLoopPeriod(int period);

    void reset(void);

    void setTargetStop(void);
    void setTarget(position_t position);

    double getAngularError(void);
    double getLinearError(void);

    bool robotMoving(void);
    bool robotTurning(void);
    bool robotRunning(void);
    int getBrakingDistance(void);
    Rotation getRotationSide(void);
    Direction getDirectionSide(void);
    position_t getCurrentTarget(void);

private:
    void setTargetAngulaire(double angle,Rotation rotation);
    void setTargetLineaire(double x, double y);

protected:
    void setProtectedTargetLineaire(double x, double y);
    void setTargetLookAt(double x, double y, Rotation rotation);
    void setTargetLookAtForward(double x, double y, Rotation rotation);
    void setTargetLookAtBackward(double x, double y, Rotation rotation);
    void setProtectedTargetAngulaire(double angle, Rotation rotation);
};