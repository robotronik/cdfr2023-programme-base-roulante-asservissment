#include "AsservissementMath.h"
#include "uart.h"




double calculAngle(double x, double y, position_t actualPostion){
    return mod_angle(atan2(y - actualPostion.y, x - actualPostion.x)*RAD_TO_DEG);
}



double mod_angle(double a){
	a = fmod(a,360);
	if (a>180) 
        a -=360;	
	else if (a<-180) 
        a +=360;	
	return a;
}
