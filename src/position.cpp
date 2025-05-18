
#include "position.h"

position::position(){
    positionRobot = {0.0, 0.0, 0.0};
}

void position::loop(){
    odometrieLoop(positionRobot);
    positionRobot.time = get_uptime_ms();
}

void position::setPosition(position_t inCommingposition){
    positionRobot = inCommingposition;
    positionRobot.teta = mod_angle(inCommingposition.teta * DEG_TO_RAD);
}
void position::setPosition(double x, double y, double teta){
    positionRobot.x = x;
    positionRobot.y = y;
    positionRobot.teta =  mod_angle(teta * DEG_TO_RAD);
    positionRobot.time = get_uptime_ms();
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


position::~position(){
}

