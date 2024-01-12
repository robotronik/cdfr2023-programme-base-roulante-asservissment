
#include "robot.h"
robot::robot(){
    position = {0.0, 0.0, 0.0};
    oldPosition = {0.0, 0.0, 0.0};
}



void robot::updatePostion(position_t inCommingposition){
    captureTimeoldPosition = captureTimepostion;
    captureTimepostion = get_uptime_us();
    oldPosition = position;
    position = inCommingposition;
    position.teta = mod_angle(position.teta);
}
void robot::updatePostion(double x, double y, double teta){
    captureTimeoldPosition = captureTimepostion;
    captureTimepostion = get_uptime_us();
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


double robot::getAngularSpeed(void){
    return (mod_angle(position.teta - oldPosition.teta))/(captureTimepostion - captureTimeoldPosition);
}

double robot::getLinearSpeed(void){
    double anglevitesse;
    double longueurDeplacement = sqrt(pow(position.y - oldPosition.y,2)+pow(position.x - oldPosition.x,2))/(captureTimepostion - captureTimeoldPosition);
    if((position.x - oldPosition.x) == 0){
        if((position.y - oldPosition.y)<0){
                anglevitesse = 90;
            }
            else{
                anglevitesse = -90;
            }
    }
    else{
        anglevitesse = (atan((position.y - oldPosition.y)/(position.x - oldPosition.x))*-RAD_TO_DEG);
        if((position.x - oldPosition.x)<0){
                anglevitesse = mod_angle(anglevitesse+180);
        } 
    }
    if(anglevitesse-position.teta<-90 || anglevitesse-position.teta>90){
        return -longueurDeplacement;
    }
    return longueurDeplacement;
}






robot::~robot()
{
}

