
#include "position.h"

position::position(){
    positionRobot = {0.0, 0.0, 0.0};
}

void position::loop(){
    odometrieLoop(positionRobot);
    if(needChangePos){
        positionRobot.x = newPosition.x;
        positionRobot.y = newPosition.y;
        positionRobot.teta = newPosition.teta*DEG_TO_RAD;
        needChangePos = false;
        positionChanged = true;
    }
    positionRobot.time = get_uptime_ms();
}

void position::setPosition(position_t inCommingposition){
    newPosition = inCommingposition;
    newPosition.teta = mod_angle(inCommingposition.teta);
    needChangePos = true;
}
void position::setPosition(double x, double y, double teta){
    newPosition.x = x;
    newPosition.y = y;
    newPosition.teta =  mod_angle(teta);
    newPosition.time = get_uptime_ms();
    needChangePos = true;
}

position_t position::getPosition(){
    position_t retPos = positionRobot;
    retPos.teta = positionRobot.teta*RAD_TO_DEG;
    return retPos;
}
double position::getPosition_X(){
    return positionRobot.x;
}
double position::getPosition_Y(){
    return positionRobot.y;
}
double position::getPosition_Teta(){
    return positionRobot.teta*RAD_TO_DEG;
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

