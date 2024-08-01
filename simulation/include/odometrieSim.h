#pragma once

#include "simulation.h"
#include "motor.h"

#define MINSPEED    500000
#define MAXSPEED    10
#define SEUIL       5*COEFMULT

class odometrieSim
{
private:
    GThread* threadOdoLeft = NULL;
    GThread* threadOdoRight = NULL;
    bool stop_thread = false;
    bool moveForwardRight = true;
    bool moveForwardLeft = true;
    int valueRight = 0;
    int valueLeft = 0;
public:
    odometrieSim(/* args */);
    void setLeftSpeed(int speed);
    void setRightSpeed(int speed);
    void setLeftDirection(bool directionForward);
    void setRightDirection(bool directionForward);
    void stopThread(void);
    static double map(double x, double in_min, double in_max, double out_min, double out_max);
    ~odometrieSim();
    static gpointer threadFuncOdometrieLeft(gpointer data);
    static gpointer threadFuncOdometrieRight(gpointer data);
};


