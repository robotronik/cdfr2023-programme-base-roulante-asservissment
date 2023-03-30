#pragma once

#include <math.h>

#include "clock.h"
#include "uart.h"
#include "odometrie.h"
#include "motor.h"
#include "AsservissementMath.h"

#define FREQUENCE 50.0
#define PERIODE (1/(double)FREQUENCE)

#define RAD_TO_DEG 57.29577951
#define DEG_TO_RAD 0.01745329252


#define KP_POSITIONANGULAIRE 4.0
#define KP_VITESSEANGULAIRE 0.1
#define KI_VITESSEANGULAIRE 1
#define VITESSEANGULAIREMAX 500.0 //mm/s ou pas
#define ACCELERATIONANGULAIREMAX 1500.0 //mm/s^2 ou pas


#define KP_POSITIONLINEAIRE 2
#define KP_VITESSELINEAIRE 0
#define KI_VITESSELINEAIRE 0.3
#define VITESSELINEAIREMAX 2000 //mm/s ou pas
#define ACCELERATIONLINEAIREMAX 4000 //mm/s^2 ou pas

#define DISTANCEMINFINASSERVANGULAIRE 200 //mm

//Vitesse maximal
//Au dela de cette vitesse, l'asservisssment se bloque
#define VITESSEANGULAIREMAXSECU  (ACCELERATIONANGULAIREMAX*2.5)
#define VITESSELINEAIREMAXSECU  (ACCELERATIONLINEAIREMAX*2.5)

typedef enum{NONE,ANGULAIRE,LINEAIREAVANT,LINEAIREARRIERE}asservissementType_t;

void asservissementSetup(void);
void asservissementLoop(void);

void asservissmentStop(void);
void printAllInformation(void);

void setAngularAsservissement(double angle);
void setLinearAsservissement(double x,double y,bool arriere);
double getLinearError(void);
double getAngularError(void);


