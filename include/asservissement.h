#pragma once

#include <math.h>

#include "clock.h"
#include "uart.h"
#include "odometrie.h"
#include "motor.h"
#include "asservissementMath.h"


#define KP_POSITIONANGULAIRE 20.0
#define KD_POSITIONANGULAIRE -0.8
#define KP_VITESSEANGULAIRE 0.10
#define KI_VITESSEANGULAIRE 0.5
#define KD_VITESSEANGULAIRE 0.0
#define VITESSEANGULAIREMAX 80.0 //deg/s 
#define ACCELERATIONANGULAIREMAX 320.0 //deg/s^2


#define KP_POSITIONLINEAIRE 4.0
#define KD_POSITIONLINEAIRE 0.0
#define KP_VITESSELINEAIRE 0.02
#define KI_VITESSELINEAIRE 0.375
#define KD_VITESSELINEAIRE 0.0
#define VITESSELINEAIREMAXAVANT 800 //mm/s
#define ACCELERATIONLINEAIREMAXAVANT 600 //mm/s^2
#define VITESSELINEAIREMAXARRIERE 600 //mm/s
#define ACCELERATIONLINEAIREMAXARRIERE 800 //mm/s^2


#define DISTANCEMINFINASSERVANGULAIRE 50 //mm

//Vitesse maximal
//Au dela de cette vitesse, l'asservisssment se bloque
#define VITESSEANGULAIREMAXSECU  (VITESSEANGULAIREMAX*1.5)
#define VITESSELINEAIREMAXSECU  (VITESSELINEAIREMAXAVANT*1.5)

enum class asservissementType {NONE,ANGULAIRE,LINEAIREAVANT,LINEAIREARRIERE};

void SetPIDValues(int index, double kP, double kI, double kD);

void asservissementSetup(void);
void asservissementLoop(void);

void asservissementStop(void);
void asservissementBrake(void);
void printAllInformation(void);

void setAngularAsservissement(double angle);
void setLinearAsservissement(double x,double y,bool arriere);
double getLinearError(void);
double getAngularError(void);




//Valeur avant coupe régional le 31/03/2022
// #define KP_POSITIONANGULAIRE 4.0
// #define KD_POSITIONANGULAIRE 0
// #define KP_VITESSEANGULAIRE 0.1
// #define KI_VITESSEANGULAIRE 1
// #define KD_VITESSEANGULAIRE 0
// #define VITESSEANGULAIREMAX 500.0 //mm/s ou pas
// #define ACCELERATIONANGULAIREMAX 1500.0 //mm/s^2 ou pas


// #define KP_POSITIONLINEAIRE 2
// #define KP_VITESSELINEAIRE 0
// #define KI_VITESSELINEAIRE 0.3
// #define KD_VITESSELINEAIRE 0
// #define VITESSELINEAIREMAX 2000 //mm/s ou pas
// #define ACCELERATIONLINEAIREMAX 4000 //mm/s^2 ou pas