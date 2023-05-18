#pragma once

#include "asservissementMath.h"

template<class T, int DBufSize>
class pid
{
public:
	T target=0, position=0;
	T kP=1, kI=0, kD=0;

	T integral=0;
	T lastError=0;
	T derivateBuff[DBufSize] = {0};
	int erroridx = 0;
	T maxWindup=1;

	bool angular=false;

	pid(T inkP=1, T inkI=0, T inkD=0, T inMaxWindup=1, bool InAngular = false)
		:kP(inkP), kI(inkI), kD(inkD), maxWindup(inMaxWindup), angular(InAngular)
	{}

	T ComputeDerivativeBuffered()
	{
		T sum = 0;
		for (int i = 0; i < DBufSize; i++)
		{
			sum += derivateBuff[i];
		}
		return sum / DBufSize;
	}

	T Tick(T deltaTime, T newPosition)
	{
		position = newPosition;
		T error = target - position;
		if (angular)
		{
			error = mod_angle(error);
		}
		
		T derror = (error-lastError)/deltaTime;
		derivateBuff[erroridx] = derror;
		erroridx = (erroridx+1)%DBufSize;
		T derivatefiltered = ComputeDerivativeBuffered();

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
		T loopOutput = kP*error+kD*derivatefiltered+kI*integral;		
		return loopOutput;
	}

	void Reset()
	{
		lastError = 0;
		integral = 0;
	}
};