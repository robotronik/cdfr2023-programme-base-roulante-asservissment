#pragma once

#include "stdint.h"
#include "clock.h"
#include "AsservissementMath.h"

class statistic
{
private:
    LowPassFilter lowPassFilter;

    bool enableReset;
    double startTime;
    double lastTime;
    double prevfilteredValue;

    double minError;
    double maxError;
    double integral;
    double quadraticIntegral;
    double derivateIntegral;
public:
    statistic(/* args */);
    void reset();
    void update(double error, int32_t time);

    double getMinError();
    double getMaxError();
    double getIntegral();
    double getQuadraticIntegral();
    double getDerivateIntegral();

    ~statistic();
};
