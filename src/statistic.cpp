#include "statistic.h"
#include "clock.h"
#include "math.h"

Statistic::Statistic(/* args */):lowPassFilter(0.1){

}

void Statistic::reset(){
    lowPassFilter.reset();
    integral = 0;
    derivateIntegral = 0;
    quadraticIntegral = 0;
    startTime = 0;
    enableReset = true;
    minError = 0;
    maxError = 0;
}

void Statistic::update(double error, int32_t time){
    double filteredValue = lowPassFilter.update(error, time);
    double derivate;
    int32_t deltaTime;

    if(error < minError){
        minError = error;
    }

    if(error > maxError){
        maxError = error;
    }

    if(enableReset){
        enableReset = false;
        startTime = time;
    }
    else{
        deltaTime = (time - lastTime);
        derivate = (filteredValue - prevfilteredValue); // to get real derivate : (filteredValue - prevfilteredValue)/deltaTime
        derivateIntegral += abs(derivate); // to get real derivate integral : derivate * deltaTime
        integral += abs(error * deltaTime);
        quadraticIntegral += error * deltaTime * error * deltaTime;
    }
    prevfilteredValue = filteredValue;
    lastTime = time;
}

double Statistic::getMinError(){
    return minError;
}

double Statistic::getMaxError(){
    return maxError;
}

double Statistic::getIntegral(){
    if(!enableReset)
        return integral/(lastTime - startTime);
    else
        return 0;
}

double Statistic::getQuadraticIntegral(){
    if(!enableReset)
        return quadraticIntegral/(lastTime - startTime);
    else
        return 0;
}

double Statistic::getDerivateIntegral(){
    if(!enableReset)
        return derivateIntegral/(lastTime - startTime);
    else
        return 0;
}

int32_t Statistic::getTime(){
    return lastTime - startTime;
}


Statistic::~Statistic()
{
}
