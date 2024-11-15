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
    sensRotation_t currentState;
    position_t consigne;
    position_t currentPostion;

    double getAngularErrorReel(void);
    double getLinearErrorReel(void);

public :
    positionControl positionControlLineaire;
    positionControl positionControlAngulaire;

public:
    

    Asservissement();
    bool asservissementLoop(int *pL_speed, int *pR_speed);
    void reset(void);

    void setConsigneAngulaire(double angle,sensRotation_t rotation);
    void setConsigneLineaire(double x, double y);
    void setConsigneLookAtForward(double x, double y, sensRotation_t rotation);
    void setConsigneLookAtBackward(double x, double y, sensRotation_t rotation);
    void setProtectedConsigneAngulaire(double angle, sensRotation_t rotation);
    void setProtectedConsigneLineaire(double x, double y);
    void setConsigneStop(void);
    void setConsigne(position_t position);
    void setCurrentPos(position_t position);

    double getAngularError(void);
    double getLinearError(void);

    bool robotMovingIsDone(void);
    bool robotTurningIsDone(void);
    bool robotRunningIsDone(void);
    int getBrakingDistance(void);
    ~Asservissement();
};