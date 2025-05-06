#pragma once
#include "config.h"
#include "clock.h"
#include "AsservissementMath.h"
#include "Odometry.h"

class position
{
private:
    position_t positionRobot = {0.0, 0.0, 0.0};
    position_t newPosition = {0.0, 0.0, 0.0};
    bool needChangePos = false;
    bool positionChanged = false;
public:
    position();

    void loop(void);

    void setPosition(position_t inCommingposition);
    void setPosition(double x, double y, double teta);


    position_t getPosition();
    double getPosition_X();
    double getPosition_Y();
    double getPosition_Teta();
    double getPosition_Time();
    bool getPositionChanged();
};






