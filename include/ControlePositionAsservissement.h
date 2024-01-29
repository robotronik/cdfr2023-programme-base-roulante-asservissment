#pragma once

#include "robot.h"
#include "config.h"
#include "PID.h"
#include "uart.h"
#include "positionControl.h"
#include "Asservissement.h"


class ControlePositionAsservissement
{
private:
    /* data */
    positionControl positionControlLineaire;
    positionControl positionControlAngulaire;
    position_t currentConsigne;
    position_t finalConsigne;
    robot* robotAsservi;
    Asservissement systemAsservisement;
public:
    ControlePositionAsservissement(robot* bot);
    motorSpeed_t ControlePositionAsservissementLoop();
    void setConsigneAngulaire(double angle);
    void setConsigneLineaire(double x, double y);
    ~ControlePositionAsservissement();
};