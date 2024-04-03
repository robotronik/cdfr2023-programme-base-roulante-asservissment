#pragma once 

#include <math.h>
#include <stdint.h>

#ifdef STM32F4

#include "uart.h"
#include "clock.h"

#else 

#include <time.h>
inline unsigned long get_uptime_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts); // Utilisation de CLOCK_MONOTONIC pour éviter les changements d'horloge
    return (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000); // Conversion en millisecondes
}

#endif

class positionControl{
public:
    /* data */
    double position;
    double consigne;
    double vitesse;

    double vitesseMaxAv = 45;
    double accelerationMaxAv = 25; 
    double decelerationMaxAv = 25;
    double vitesseMaxAr = 45;
    double accelerationMaxAr = 25; 
    double decelerationMaxAr = 25;
    double deltaTemps = 0;
    bool decelationLineair = true;

    uint32_t PreviousTime;

    bool stopStatus;

public:
    positionControl(double positionDepart  = 0.0);
    void reset(double initialValue);
    void stop(void);
    void setPosition(double initialValue);
    void setConsigne(double setConsigne);
    double getPostion();
    void setConsigne(double setConsigne, int timems);
    double getPostion(int timems);
    bool getMove();
    int getBrakingDistance();
    ~positionControl();
private:
    void calculVitesse();
};
