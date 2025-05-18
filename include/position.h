#pragma once
#include "config.h"
#include "clock.h"
#include "AsservissementMath.h"
#include "odometrie.h"

class position
{
private:
    position_t positionRobot = {0.0, 0.0, 0.0};
public:
    position(/* args */);

    void loop(void);

    void setPosition(position_t inCommingposition);
    void setPosition(double x, double y, double teta);


    position_t getPosition();
    double getPosition_X();
    double getPosition_Y();
    double getPosition_Teta();
    double getPosition_Time();
    bool getPositionChanged();


    ~position();
};






