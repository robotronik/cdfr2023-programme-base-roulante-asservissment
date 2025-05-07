#include "AsservissementMath.h"
#include "uart.h"

#include <math.h>

double calculAngle(double x, double y, position_t actualPostion) {
    double angleTournerPoint;
    if ((x - actualPostion.x) == 0) {
        if ((y - actualPostion.y)<0) {
                angleTournerPoint = 90;
            }
            else {
                angleTournerPoint = -90;
            }
    }
    else {
        angleTournerPoint = (atan((y - actualPostion.y)/(x - actualPostion.x))*RAD_TO_DEG);
        if ((x - actualPostion.x)<0) {
            angleTournerPoint -= 180;
        }
    }
    angleTournerPoint = mod_angle(angleTournerPoint);
    return angleTournerPoint;
}

double mod_angle(double a) {
	a = fmod(a,360);
	if (a > 180)
		a -= 360;
	else if (a < -180)
		a += 360;
	return a;
}