#pragma once

#include <math.h>

#include "uart.h"
#include "odometrie.h"
#include "motor.h"

#define FREQUENCE 50.0
#define PERIODE (1/(double)FREQUENCE)

#define RAD_TO_DEG 57.29577951
#define DEG_TO_RAD 0.01745329252

#define ALPHA 0.001f // coefficient de lissage

#define KP_POSITIONANGULAIRE 8.0
#define KP_VITESSEANGULAIRE 0.1
#define KI_VITESSEANGULAIRE 2
#define VITESSEANGULAIREMAX 500.0 //mm/s
#define ACCELERATIONANGULAIREMAX 1500.0 //mm/s^2


#define KP_POSITIONLINEAIR 0
#define KP_VITESSELINEAIR 0
#define KI_VITESSELINEAIR 0
#define VITESSELINEAIRMAX 300 //mm/s
#define ACCELERATIONLINEAIRMAX 100 //mm/s^2

void asservissementSetup(void);

void asservissementLoop(void);

void setAngularAsservissement(double angle);
void setLinearAsservissement(double x,double y);
