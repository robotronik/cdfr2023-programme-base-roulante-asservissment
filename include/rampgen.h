
#include <cmath>
#include "asservissementMath.h"

template<class T>
class positionasserv
{
public:
	T AsservStop;
	T minspeed, maxspeed;
	T maxacc, maxdec;

	positionasserv(T inasservstop, T inminspeed, T inmaxspeed, T inmaxacc, T inmaxdec)
	:AsservStop(inasservstop), minspeed(inminspeed), maxspeed(inmaxspeed), maxacc(inmaxacc), maxdec(inmaxdec)
	{}

	static double SpeedDeltaDistance(double v0, double v1, double acc)
	{
		double deltaspeed = v1-v0;
		return deltaspeed/acc*(deltaspeed*0.5 + v0);
	};
	
	T GetBrakingDistance(T v0) const
	{
		if (abs(v0) < minspeed)
		{
			return 0;
		}
		
		T v1 = copysign(minspeed, v0);
		T dec = -copysign(maxdec, v0);
		return SpeedDeltaDistance(v0, v1, dec);
	}

	//returns target speed
	T Tick(T deltaTime, T error, T speed)
	{
		T StopDist = GetBrakingDistance(speed);
		T ns = speed;
		if (abs(error) < AsservStop && abs(speed) < minspeed)
		{
			return 0;
		}
		bool GoingTheRightWay = error*speed >= -__DBL_EPSILON__;
		bool accelerate = false;
		if (abs(error) < abs(StopDist) && GoingTheRightWay)
		{
			accelerate = false;
		}
		else if (!GoingTheRightWay)
		{
			accelerate = false;
		}
		else
		{
			accelerate = true;
		}

		if (accelerate)
		{
			if (abs(speed) < minspeed)
			{
				speed = copysign(minspeed, error);
			}
			ns = speed + deltaTime*copysign(maxacc, speed);
			ns = clamp(ns, -maxspeed, maxspeed);
		}
		else
		{
			ns = speed - deltaTime*copysign(maxdec, speed);
			if (abs(ns) < minspeed)
			{
				ns = 0;
			}
			
		}
		return ns;
	}
};
