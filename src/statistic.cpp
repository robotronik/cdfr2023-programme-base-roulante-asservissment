#include "statistic.h"


statistic::statistic(/* args */):lowPassFilter(0.1){

}

void statistic::reset(){
    lowPassFilter.reset();
    integral = 0;
    derivateIntegral = 0;
    quadraticIntegral = 0;
    startTime = 0;
    enableReset = true;
    minError = 0;
    maxError = 0;
}

void statistic::update(double error, int32_t time){
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
        derivateIntegral += derivate; // to get real derivate integral : derivate * deltaTime
        integral += abs(error * deltaTime);
        quadraticIntegral += error * deltaTime * error * deltaTime;
    }
    prevfilteredValue = filteredValue;
    lastTime = time;
}

double statistic::getMinError(){
    return minError;
}

double statistic::getMaxError(){
    return maxError;
}

double statistic::getIntegral(){
    return integral;
}

double statistic::getQuadraticIntegral(){
    return quadraticIntegral;
}

double statistic::getDerivateIntegral(){
    return derivateIntegral;
}


statistic::~statistic()
{
}
