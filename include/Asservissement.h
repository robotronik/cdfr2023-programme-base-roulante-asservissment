#pragma once

#include "config.h"
#include "PID.h"
#include "uart.h"
#include "positionControl.h"
#include "position.h"
#include "motor.h"
#include "statistic.h"



class Asservissement
{
private:
    /* data */
    Rotation currentState;
    position_t consigne;

    double getAngularErrorReel(void);
    double getLinearErrorReel(void);

    uint32_t nextTime;
    int loopPeriod = 20;

    bool print = false;

protected:
    position* posRobot;

public :
    positionControl positionControlLineaire;
    positionControl positionControlAngulaire;
    PID pidLineaire;
    PID pidAngulaire;
    PID pidLineaireBlock;
    PID pidAngulaireBlock;
    Statistic statisticLinear;
    Statistic statisticAngular;

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

    bool robotMoving(void);
    bool robotTurning(void);
    bool robotRunning(void);
    int getBrakingDistance(void);
    Rotation getRotationSide(void);
    Direction getDirectionSide(void);
    position_t getCurrentConsigne(void);

    ~Asservissement();

protected:
    void setConsigneLineaire(double x, double y, double maxSpeedOut = 0);
    void setConsigneAngulaire(double angle,Rotation rotation);
    void setConsigneAngulaire(double x, double y, Direction direction, Rotation rotation);
    static double getLookAtAngle(position_t pos, double x, double y, Direction direction, Rotation rotation);
};