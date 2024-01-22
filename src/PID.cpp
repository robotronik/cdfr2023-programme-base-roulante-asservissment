#include "PID.h"



PID::PID(double _kp, double _ki, double _kd) {
    kp = _kp;
    ki = _ki;
    kd = _kd;
    lastError = 0.0;
    integral = 0.0;
}


double PID::update(double error, uint32_t time) {

    double deltaTime = time - oldtime;
    oldtime = time;

    // Calcul du terme proportionnel
    double proportional = kp * error;

    // Calcul du terme intégral
    integral += error * deltaTime;
    double integralTerm = ki * integral;

    // Calcul du terme dérivé
    double derivative = kd * ((error - lastError)/deltaTime);

    // Mise à jour de la dernière erreur
    lastError = error;

    // Calcul de la sortie du contrôleur PID
    double output = proportional + integralTerm + derivative;

    return output;
}



PID::~PID()
{
}