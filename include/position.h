#pragma once
#include "structs.h"

class position
{
public:
    position();

    void loop(void);

    void setPosition(position_t inCommingposition);
    void setPosition(double x, double y, double a);


    position_t getPosition();
    double getPosition_X();
    double getPosition_Y();
    double getPosition_Teta();
    double getPosition_Time();
    bool getPositionChanged();
private:
    position_t positionRobot = {0.0, 0.0, 0.0};
    position_t newPosition = {0.0, 0.0, 0.0};
    bool needChangePos = false;
    bool positionChanged = false;
};