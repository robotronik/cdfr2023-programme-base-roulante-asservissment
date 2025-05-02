#pragma once

#include "stdint.h"
#include "AsservissementMath.h"

class Statistic
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
    Statistic();
    void reset();
    void update(double error, int32_t time);

    double getMinError();
    double getMaxError();
    double getIntegral();
    double getQuadraticIntegral();
    double getDerivateIntegral();
    int32_t getTime();

    ~Statistic();
};
