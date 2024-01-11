
#include "robot.h"
robot::robot(){
    position = {0.0, 0.0, 0.0};
    oldPosition = {0.0, 0.0, 0.0};
}



void robot::updatePostion(position_t inCommingposition){
    oldPosition = position;
    position = inCommingposition;
    position.teta = mod_angle(position.teta);
}
void robot::updatePostion(double x, double y, double teta){
    oldPosition = position;
    position.x = x;
    position.y = y;
    position.teta = mod_angle(teta);
}

void robot::setPostion(position_t inCommingposition){
    position = inCommingposition;
    position.teta = mod_angle(position.teta);
    oldPosition = position;
}
void robot::setPostion(double x, double y, double teta){
    position.x = x;
    position.y = y;
    position.teta =  mod_angle(teta);
    oldPosition = position;
}

position_t robot::getPosition(){
    return position;
}
double robot::getPosition_X(){
    return position.x;
}
double robot::getPosition_Y(){
    return position.y;
}
double robot::getPosition_Teta(){
    return position.teta;
}






robot::~robot()
{
}

