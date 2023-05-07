#pragma once

#include "asservissementMath.h"

template<class T, bool angular>
class pid
{
public:
	T target=0, position=0;
	T kP=1, kI=0, kD=0;

	T integral=0;
	T lastError=0;
	T maxWindup=1;

	pid(T inkP=1, T inkI=0, T inkD=0, T inMaxWindup=1)
		:kP(inkP), kI(inkI), kD(inkD), maxWindup(inMaxWindup)
	{}

	T Tick(T deltaTime, T newPosition)
	{
		position = newPosition;
		T error = target - position;
		if (angular)
		{
			error = mod_angle(error);
		}
		
		T derror = (error-lastError)/deltaTime;
		integral = integral + error*deltaTime;
		if (integral>maxWindup)
		{
			integral = maxWindup;
		}
		else if (integral<-maxWindup)
		{
			integral = -maxWindup;
		}
		lastError = error;
		T loopOutput = kP*error+kD*derror+kI*integral;		
		return loopOutput;
	}

	void Reset()
	{
		lastError = 0;
		integral = 0;
	}
};