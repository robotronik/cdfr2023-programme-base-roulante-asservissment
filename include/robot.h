#pragma once
#include "config.h"
#include "clock.h"
#include "AsservissementMath.h"

class robot
{
private:
    /* data */
    position_t position = {0.0, 0.0, 0.0};
    uint32_t captureTimepostion;
    position_t oldPosition = {0.0, 0.0, 0.0};
    uint32_t captureTimeoldPosition;
public:
    robot();

    void updatePostion(position_t inCommingposition);
    void updatePostion(double x, double y, double teta);

    void setPostion(position_t inCommingposition);
    void setPostion(double x, double y, double teta);

    position_t getPosition();
    double getPosition_X();
    double getPosition_Y();
    double getPosition_Teta();

    double getAngularSpeed(void);
    double getLinearSpeed(void);

    ~robot();
};


