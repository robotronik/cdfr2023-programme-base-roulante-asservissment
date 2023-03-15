#include "Asservissement.h"

asservissementType_t asservissementType = NONE;
position_t actualPostion;
position_t previousPosition;
double consigneAngle;
double consigneX;
double consigneY;
bool erreur = true;

double filteredValue = 0.0f;

double consignevittesseAngulairePrecedante;
double intergralAngulaire;

double consignevittesseLineairePrecedante;
double intergralLineaire;

double lowPassFilter(double input);
double mod_angle(double a);
double calculAngle(double x, double y);

void asservissementSetup(void){
    previousPosition = odometrieGetPosition();
    actualPostion = odometrieGetPosition();
    consignevittesseAngulairePrecedante = 0;
    erreur = false;
    consigneAngle = 0;
    consigneX = 0;
    consigneY = 0;
}


static double getAngularSpeed(void){
    //*********************
    //CALCUL VITESSEANGULAIRE en mm seconde
    //*********************
    return (mod_angle(actualPostion.teta - previousPosition.teta)) * FREQUENCE;
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
   if(linearSpeed>VITESSELINEAIREMAX*2.5){
    erreur = true;
   }
   else if(linearSpeed<-VITESSELINEAIREMAX*2.5){
    erreur = true;
   }

   double distanceRobotPoint = sqrt(pow((consigneX - actualPostion.x),2)+pow((consigneY - actualPostion.y),2));
   if(distanceRobotPoint>DISTANCEMINFINASSERVANGULAIRE && asservissementType == LINEAIRE ){
        consigneAngle = calculAngle(consigneX,consigneY);
   }
   double angleErreur = mod_angle(consigneAngle-actualPostion.teta);
   double erreurPositionLineairePoint = distanceRobotPoint*cos(angleErreur*DEG_TO_RAD);
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

    
    // usartprintf("\n>vitesseAngulaire:%lf\n",angularSpeed);
    // usartprintf("\n>intergralAngulaire:%lf\n",intergralAngulaire);
    // usartprintf("\n>erreurvittesseAngulaire:%lf\n",erreurvittesseAngulaire);
    // usartprintf("\n>motorcontrolAngle:%lf\n",motorcontrolAngle);
    // usartprintf("\n>consignevittesseAngulaire:%lf\n",consignevittesseAngulaire);
    usartprintf("\n>consigneAngle:%lf\n",consigneAngle);
    usartprintf("\n>erreurPosition:%lf\n",angleErreur);
    


    //CALCUL VITESSE Lineair

    double consignevittesseLineaire = erreurPositionLineairePoint * KP_POSITIONLINEAIRE;
    if (consignevittesseLineaire>consignevittesseLineairePrecedante+ACCELERATIONLINEAIREMAX/FREQUENCE){
        consignevittesseLineaire = consignevittesseLineairePrecedante+ACCELERATIONLINEAIREMAX/FREQUENCE;
    }
    else if (consignevittesseLineaire<consignevittesseLineairePrecedante-ACCELERATIONLINEAIREMAX/FREQUENCE){
        consignevittesseLineaire = consignevittesseLineairePrecedante-ACCELERATIONLINEAIREMAX/FREQUENCE;
    }
    if(consignevittesseLineaire>VITESSELINEAIREMAX){
        consignevittesseLineaire = VITESSELINEAIREMAX;
    }
    else if (consignevittesseLineaire<-VITESSELINEAIREMAX){
        consignevittesseLineaire = -VITESSELINEAIREMAX;
    }
    consignevittesseLineairePrecedante = consignevittesseLineaire;


    //intergralLineaire
    double erreurvittesseLineaire = consignevittesseLineaire-linearSpeed;
    intergralLineaire += erreurvittesseLineaire * (PERIODE);

    double motorcontrolLigne = erreurvittesseLineaire * KP_VITESSELINEAIRE + intergralLineaire * KI_VITESSELINEAIRE;

    motorcontrolLigne;


    usartprintf("\n>vitesseLineaire:%lf\n",linearSpeed);
    usartprintf("\n>intergralLineaire:%lf\n",intergralLineaire);
    usartprintf("\n>erreurvittesseLineaire:%lf\n",erreurvittesseLineaire);
    usartprintf("\n>motorcontrolLigne:%lf\n",motorcontrolLigne);
    usartprintf("\n>consignevittesseLineaire:%lf\n",consignevittesseLineaire);
    usartprintf("\n>erreurPositionLineairePoint:%lf\n",erreurPositionLineairePoint);



    double controleMoteurR = -motorcontrolAngle + motorcontrolLigne;
    double controleMoteurL = motorcontrolAngle + motorcontrolLigne;

    if(controleMoteurR>100){
        controleMoteurR = 100;
    }
    else if (controleMoteurR<-100){
        controleMoteurR = -100;
    }
    if(controleMoteurL>100){
        controleMoteurL = 100;
    }
    else if (controleMoteurL<-100){
        controleMoteurL = -100;
    }

   if(erreur){
    usartprintf("ERREUR ASSERVISEMENT");
    motorSpeedSignedL(0);
    motorSpeedSignedR(0);
   }
   else{
    motorSpeedSignedL(controleMoteurL);
    motorSpeedSignedR(controleMoteurR);
   }
}

void setAngularAsservissement(double angle){
    consigneAngle = angle;
    asservissementType = ANGULAIRE;
}

void setLinearAsservissement(double x,double y){
    consigneAngle = calculAngle(x,y);
    consigneX = x;
    consigneY = y;
    asservissementType = LINEAIRE;
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