#include "PID.h"



PID::PID(double _kp, double _ki, double _kd) {
    kp = _kp;
    ki = _ki;
    kd = _kd;
    reset();
}

void PID::reset(void){
    lastError = 0.0;
    integral = 0.0;
    oldtime = 0.0;
}

double PID::update(double error, uint32_t time) {
    double proportional = 0;
    double integralTerm = 0;
    double derivative = 0;
    double deltaTime = time - oldtime;
    oldtime = time;

    // Calcul du terme proportionnel
    proportional = kp * error;

    // Calcul du terme intégral
    integral += error * deltaTime;
    integralTerm = ki * integral;

    // Calcul du terme dérivé
    if(deltaTime>0){
        derivative = kd * ((error - lastError)/deltaTime);
    }

    lastError = error;

    return proportional + integralTerm + derivative;
}



PID::~PID()
{
}