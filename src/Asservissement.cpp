#include "Asservissement.h"

position_t actualPostion;
position_t previousPosition;
double consigneAngle;
bool erreur = true;

double filteredValue = 0.0f;

double consignevittesseAngulairePrecedante;
double intergralAngulaire;
double erreurvittesseAngulairePrecedante;

double lowPassFilter(double input);
double mod_angle(double a);
double calculAngle(double x, double y);

void asservissementSetup(void){
    previousPosition = odometrieGetPosition();
    actualPostion = odometrieGetPosition();
    consignevittesseAngulairePrecedante = 0;
    erreur = false;
}


static double getAngularSpeed(void){
    //*********************
    //CALCUL VITESSEANGULAIRE en mm seconde
    //*********************
    return (actualPostion.teta - previousPosition.teta) * FREQUENCE;
}

static double getLinearSpeed(void){
    //*********************
    //CALCUL VITESSELINEAIRE en mm seconde
    //*********************
    double anglevitesse;
    double longueurDeplacement = sqrt(pow(actualPostion.y - previousPosition.y,2)+pow(actualPostion.x - previousPosition.x,2))*FREQUENCE;
    if((actualPostion.y - previousPosition.y) == 0){
        if((actualPostion.x - previousPosition.x)<0){
                anglevitesse = -90;
            }
            else{
                anglevitesse = 90;
            }
    }
    else{
        anglevitesse = (atan((actualPostion.x - previousPosition.x)/(actualPostion.y - previousPosition.y))*RAD_TO_DEG);
        if((actualPostion.y - previousPosition.y)<0){
            if((actualPostion.x - previousPosition.x)<0){
                anglevitesse -= 180;
            }
            else{
                anglevitesse += 180;
            }
        }
        //erreurPositionAngulairePoint = mod_angle(angleAsserv-actualPostion.teta);  
    }
    //Gestion de la distance
    if(anglevitesse-actualPostion.teta>0){
        return longueurDeplacement;
    }
    return -longueurDeplacement;
}


void asservissementLoop(void){
   actualPostion = odometrieGetPosition();
   double linearSpeed = getLinearSpeed();
   double angularSpeed = getAngularSpeed();
   if(angularSpeed>VITESSEANGULAIREMAX*2.5){
    erreur = true;
   }
   else if(angularSpeed<-VITESSEANGULAIREMAX*2.5){
    erreur = true;
   }
//    if(linearSpeed>VITESSELINEAIRMAX*2.5){
//     erreur = true;
//    }
//    else if(linearSpeed<-VITESSELINEAIRMAX*2.5){
//     erreur = true;
//    }
   double angleErreur = mod_angle(consigneAngle-actualPostion.teta);
//    double ErreurLineaire = consigneLineaire-actualPostion
   previousPosition = actualPostion;



    //CALCUL VITESSE ANGULAIRE

    double consignevittesseAngulaire = angleErreur * KP_POSITIONANGULAIRE;
    if (consignevittesseAngulaire>consignevittesseAngulairePrecedante+ACCELERATIONANGULAIREMAX/FREQUENCE){
        consignevittesseAngulaire = consignevittesseAngulairePrecedante+ACCELERATIONANGULAIREMAX/FREQUENCE;
         usartprintf("1");
    }
    else if (consignevittesseAngulaire<consignevittesseAngulairePrecedante-ACCELERATIONANGULAIREMAX/FREQUENCE){
        consignevittesseAngulaire = consignevittesseAngulairePrecedante-ACCELERATIONANGULAIREMAX/FREQUENCE;
        usartprintf("2");
    }
    if(consignevittesseAngulaire>VITESSEANGULAIREMAX){
        consignevittesseAngulaire = VITESSEANGULAIREMAX;
        usartprintf("3");
    }
    else if (consignevittesseAngulaire<-VITESSEANGULAIREMAX){
        consignevittesseAngulaire = -VITESSEANGULAIREMAX;
        usartprintf("4");
    }
    consignevittesseAngulairePrecedante = consignevittesseAngulaire;
    
    
    //intergralAngulaire
    double erreurvittesseAngulaire = consignevittesseAngulaire-angularSpeed;
    intergralAngulaire += erreurvittesseAngulaire * (PERIODE);

    double motorcontrolAngle = erreurvittesseAngulaire * KP_VITESSEANGULAIRE + intergralAngulaire * KI_VITESSEANGULAIRE;

    double motorcontrolAngleFiltered = motorcontrolAngle;

    if(motorcontrolAngleFiltered>100){
        motorcontrolAngleFiltered = 100;
    }
    else if (motorcontrolAngleFiltered<-100){
        motorcontrolAngleFiltered = -100;
    }
    usartprintf("\n>vitesseAngulaire:%lf\n",angularSpeed);
    usartprintf("\n>intergralAngulaire:%lf\n",intergralAngulaire);
    usartprintf("\n>erreurvittesseAngulaire:%lf\n",erreurvittesseAngulaire);
    usartprintf("\n>motorcontrolAngle:%lf\n",motorcontrolAngle);
    usartprintf("\n>motorcontrolAngleFiltered:%lf\n",motorcontrolAngleFiltered);
    usartprintf("\n>consignevittesseAngulaire:%lf\n",consignevittesseAngulaire);
    usartprintf("\n>erreurPosition:%lf\n",angleErreur);
    


    // //CALCUL VITESSE ANGULAIRE

    // double consignevittesseLineaire = li * KP_POSITIONANGULAIRE;
    // if (consignevittesseAngulaire>consignevittesseAngulairePrecedante+ACCELERATIONANGULAIREMAX/FREQUENCE){
    //     // consignevittesseAngulaire = consignevittesseAngulairePrecedante+ACCELERATIONANGULAIREMAX/FREQUENCE;
    //      usartprintf("1");
    // }
    // else if (consignevittesseAngulaire<consignevittesseAngulairePrecedante-ACCELERATIONANGULAIREMAX/FREQUENCE){
    //     consignevittesseAngulaire = consignevittesseAngulairePrecedante-ACCELERATIONANGULAIREMAX/FREQUENCE;
    //     usartprintf("2");
    // }
    // if(consignevittesseAngulaire>VITESSEANGULAIREMAX){
    //     consignevittesseAngulaire = VITESSEANGULAIREMAX;
    //     usartprintf("3");
    // }
    // else if (consignevittesseAngulaire<-VITESSEANGULAIREMAX){
    //     consignevittesseAngulaire = -VITESSEANGULAIREMAX;
    //     usartprintf("4");
    // }
    // consignevittesseAngulairePrecedante = consignevittesseAngulaire;



   if(erreur){
    usartprintf("ERREUR ASSERVISEMENT");
    motorSpeedSignedL(0);
    motorSpeedSignedR(0);
   }
   else{
    motorSpeedSignedL(motorcontrolAngleFiltered);
    motorSpeedSignedR(-motorcontrolAngleFiltered);
   }
}

void setAngularAsservissement(double angle){
    consigneAngle = angle;
}

void setLinearAsservissement(double x,double y){
    consigneAngle = calculAngle(x,y);
    usartprintf(">angleClacul:%lf\n",consigneAngle);
}

double lowPassFilter(double input){
    filteredValue = (ALPHA * input) + ((1.0f - ALPHA) * filteredValue);
    return filteredValue;
}

double calculAngle(double x, double y){
    double angleTournerPoint;
    if((x - actualPostion.x) == 0){
        if((y - actualPostion.y)<0){
                angleTournerPoint = 90;
            }
            else{
                angleTournerPoint = -90;
            }
    }
    else{
        angleTournerPoint = -(atan((y - actualPostion.y)/(x - actualPostion.x))*RAD_TO_DEG);
        if((x - actualPostion.x)<0){
            angleTournerPoint -= 180;
        }
        //Gestion de la marche arrière
    }
    angleTournerPoint = mod_angle(angleTournerPoint);
    return angleTournerPoint;
}

double mod_angle(double a){
	a = fmod(a,360);
	if(a>180){
		a -=360;
	}
	else if(a<-180){
		a +=360;
	}
	return a;
}