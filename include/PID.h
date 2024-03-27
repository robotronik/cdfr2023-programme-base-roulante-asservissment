#pragma once

#include "robot.h"


class PID
{
private:
    double kp;       // Gain proportionnel
    double ki;       // Gain intégral
    double kd;       // Gain dérivé

    double lastError; // Dernière erreur
    double integral;  // Terme intégral
    double oldtime; //dernier appel a la fonction update
public:
    PID(double _kp, double _ki, double _kd);
    double update(double currentValue, uint32_t time);
    void reset(void);

    // Fonction pour définir une nouvelle consigne
    void setSetpoint(double newSetpoint);
    ~PID();
};


