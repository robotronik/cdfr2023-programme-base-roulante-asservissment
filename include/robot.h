#pragma once
#include "config.h"
#include "clock.h"
#include "AsservissementMath.h"

class robot
{
private:
    /* data */
    position_t position = {0.0, 0.0, 0.0};
    position_t oldPosition = {0.0, 0.0, 0.0};
public:
    robot();

    void updatePostion(position_t inCommingposition);
    void updatePostion(double x, double y, double theta, uint64_t time);

    void setPosition(position_t inCommingposition);
    void setPosition(double x, double y, double theta, uint64_t time);

    position_t getPosition();

    double getAngularSpeed(void);
    double getLinearSpeed(void);

    ~robot();
};


