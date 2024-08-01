#pragma once

#include <gtk/gtk.h>

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


