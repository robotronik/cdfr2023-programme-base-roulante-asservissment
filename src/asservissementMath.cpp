#include "asservissementMath.h"




double calculAngle(double x, double y, position_t actualPostion){
    double dx = x - actualPostion.x;
    double dy = y - actualPostion.y;
    double angleTournerPoint = -atan2(dy,dx)*RAD_TO_DEG;
    return angleTournerPoint;
}



double mod_angle(double a){
	a = fmod(a,360);
	if(a>180){
		a -=360;
	}
	else if(a<-180){
		a +=360;
	}
	return a;
}