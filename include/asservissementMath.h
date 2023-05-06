#pragma once

#include <math.h>
#include "config.h"

const double RAD_TO_DEG = 180.0/M_PI;
const double DEG_TO_RAD = M_PI/180.0;


double mod_angle(double a);
double calculAngle(double x, double y, position_t actualPostion);
template<class T> 
T clamp(T x, T lower, T upper)
{
	if (x<lower)
	{
		return lower;
	}
	if (x>upper)
	{
		return upper;
	}
	return x;
}