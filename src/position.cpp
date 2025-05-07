#include "position.h"
#include "config.h"
#include "Odometry.h"
#include "clock.h"

position::position(){
    positionRobot = {0.0, 0.0, 0.0};
}

void position::loop(){
    odometryLoop(positionRobot);
    if(needChangePos){
        positionRobot.x = newPosition.x;
        positionRobot.y = newPosition.y;
        positionRobot.a = newPosition.a*DEG_TO_RAD;
        needChangePos = false;
        positionChanged = true;
    }
    positionRobot.time = get_uptime_ms();
}

void position::setPosition(position_t inCommingposition){
    newPosition = inCommingposition;
    newPosition.a = mod_angle(inCommingposition.a);
    needChangePos = true;
}
void position::setPosition(double x, double y, double a){
    newPosition.x = x;
    newPosition.y = y;
    newPosition.a =  mod_angle(a);
    newPosition.time = get_uptime_ms();
    needChangePos = true;
}

position_t position::getPosition(){
    position_t retPos = positionRobot;
    retPos.a = positionRobot.a*RAD_TO_DEG;
    return retPos;
}
double position::getPosition_X(){
    return positionRobot.x;
}
double position::getPosition_Y(){
    return positionRobot.y;
}
double position::getPosition_Teta(){
    return positionRobot.a*RAD_TO_DEG;
}
double position::getPosition_Time(){
    return positionRobot.time;
}

bool position::getPositionChanged(){
    bool bret = positionChanged;
    positionChanged = false;
    return bret;
}