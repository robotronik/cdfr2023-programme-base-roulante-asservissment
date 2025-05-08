#include "types/PID.h"



PID::PID(double _kp, double _ki, double _kd) {
    kp = _kp;
    ki = _ki;
    kd = _kd;
    reset();
}

void PID::setPID(double _kp, double _ki, double _kd) {
    kp = _kp;
    ki = _ki;
    kd = _kd;
    reset();
}

void PID::getPID(double &_kp, double &_ki, double &_kd) {
    _kp = kp;
    _ki = ki;
    _kd = kd;
}

void PID::reset(void){
    lastError = 0.0;
    integral = 0.0;
    oldtime = 0.0;
}

double PID::update(double error, uint32_t time) {
    double deltaTime = time - oldtime;
    if (oldtime==0) {
        deltaTime = 0;
    }
    oldtime = time;

    // Calcul du terme proportionnel
    valP = kp * error;

    // Calcul du terme intégral
    integral += error * deltaTime;
    valI = ki * integral;

    // Calcul du terme dérivé
    valD = 0;
    if (deltaTime>0) {
        valD = kd * ((error - lastError)/deltaTime);
    }

    lastError = error;

    return valP + valI + valD;
}



PID::~PID()
{
}