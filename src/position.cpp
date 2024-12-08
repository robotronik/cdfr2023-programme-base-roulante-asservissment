
#include "position.h"

position::position(){
    positionRobot = {0.0, 0.0, 0.0};
}

void position::loop(){
    odometrieLoop(positionRobot);
    if(needChangePos){
        positionRobot = newPosition;
        needChangePos = false;
        positionChanged = true;
    }
    positionRobot.time = get_uptime_us();
}

void position::setPosition(position_t inCommingposition){
    newPosition = inCommingposition;
    newPosition.teta = mod_angle(positionRobot.teta);
    needChangePos = true;
}
void position::setPosition(double x, double y, double teta){
    positionRobot.x = x;
    positionRobot.y = y;
    positionRobot.teta =  mod_angle(teta);
    positionRobot.time = get_uptime_us();
}

position_t position::getPosition(){
    return positionRobot;
}
double position::getPosition_X(){
    return positionRobot.x;
}
double position::getPosition_Y(){
    return positionRobot.y;
}
double position::getPosition_Teta(){
    return positionRobot.teta;
}
double position::getPosition_Time(){
    return positionRobot.time;
}

bool position::getPositionChanged(){
    bool bret = positionChanged;
    positionChanged = false;
    return bret;
}


position::~position(){
}

