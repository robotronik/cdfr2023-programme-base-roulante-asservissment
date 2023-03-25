#include "Asservissement.h"

asservissementType_t asservissementType = NONE;
position_t actualPostion;
position_t previousPosition;
double consigneAngle;
double consigneX;
double consigneY;
double consigneangleX;
double consigneangleY;

double xwait;
double ywait;

//variable d'asservissment
double consignevittesseAngulairePrecedante;
double intergralAngulaire;
double consignevittesseLineairePrecedante;
double intergralLineaire;

bool passageAngulaireLineaire = false;

bool erreurControlMoteur = true;
void asservissementControlMoteur(double controleMoteurR, double controleMoteurL, double angularSpeedSecu, double linearSpeedSecu);

static double getAngularSpeed(void);
static double getLinearSpeed(void);



void asservissementSetup(void){
    previousPosition = odometrieGetPosition();
    actualPostion = odometrieGetPosition();
    consignevittesseAngulairePrecedante = 0;
    erreurControlMoteur = false;
    consigneAngle = 0;
    consigneX = 0;
    consigneY = 0;
    xwait = 0;
    ywait = 0;
}

void asservissementLoop(void){
   actualPostion = odometrieGetPosition();
   double linearSpeed = getLinearSpeed();
   double angularSpeed = getAngularSpeed();
   previousPosition = actualPostion;
   

    //calacul des erreurs d'angle et de position lineaire
   double distanceRobotPoint = sqrt(pow((consigneX - actualPostion.x),2)+pow((consigneY - actualPostion.y),2));
   double consigneAngleTheorique = calculAngle(consigneangleX,consigneangleY,actualPostion);
   if(asservissementType == LINEAIREARRIERE){
        consigneAngleTheorique = mod_angle(consigneAngleTheorique + 180);
   }
   if(distanceRobotPoint>DISTANCEMINFINASSERVANGULAIRE && (asservissementType == LINEAIREAVANT || asservissementType == LINEAIREARRIERE) ){
        consigneAngle = consigneAngleTheorique;
   }
   double angleErreur = mod_angle(consigneAngle-actualPostion.teta);
   double angleErreurTheroique = mod_angle(consigneAngleTheorique-actualPostion.teta);
   double erreurPositionLineairePoint = distanceRobotPoint*cos(angleErreurTheroique*DEG_TO_RAD);
   if(asservissementType == LINEAIREARRIERE){
        erreurPositionLineairePoint = -erreurPositionLineairePoint;
   }

   //passage de l'asservisseemnt rotatif a lineair
   if(angleErreur>-10 && angleErreur<10){
      consigneX = xwait;
      consigneY = ywait;
   }
   


    //*********************
    //CALCUL ANGULAIRE
    //*********************

    //asservismsent de la position angulaire
    double consignevittesseAngulaire = angleErreur * KP_POSITIONANGULAIRE;
    if (consignevittesseAngulaire>consignevittesseAngulairePrecedante+ACCELERATIONANGULAIREMAX/FREQUENCE){
        consignevittesseAngulaire = consignevittesseAngulairePrecedante+ACCELERATIONANGULAIREMAX/FREQUENCE;
    }
    else if (consignevittesseAngulaire<consignevittesseAngulairePrecedante-ACCELERATIONANGULAIREMAX/FREQUENCE){
        consignevittesseAngulaire = consignevittesseAngulairePrecedante-ACCELERATIONANGULAIREMAX/FREQUENCE;
    }
    if(consignevittesseAngulaire>VITESSEANGULAIREMAX){
        consignevittesseAngulaire = VITESSEANGULAIREMAX;
    }
    else if (consignevittesseAngulaire<-VITESSEANGULAIREMAX){
        consignevittesseAngulaire = -VITESSEANGULAIREMAX;
    }
    consignevittesseAngulairePrecedante = consignevittesseAngulaire;
    
    //asservissemnt de la vitesse angulaire
    double erreurvittesseAngulaire = consignevittesseAngulaire-angularSpeed;
    intergralAngulaire += erreurvittesseAngulaire * (PERIODE);

    double motorcontrolAngle = erreurvittesseAngulaire * KP_VITESSEANGULAIRE + intergralAngulaire * KI_VITESSEANGULAIRE;



    //*********************
    //CALCUL LINEAIRE
    //*********************
    
    //asservismsent de la position Lineaire
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


    //asservissemnt de la vitesse Lineaire
    double erreurvittesseLineaire = consignevittesseLineaire-linearSpeed;
    intergralLineaire += erreurvittesseLineaire * (PERIODE);

    double motorcontrolLigne = erreurvittesseLineaire * KP_VITESSELINEAIRE + intergralLineaire * KI_VITESSELINEAIRE;


    //*********************
    //Debug
    //*********************

    // usartprintf("\n>vitesseAngulaire:%lf\n",angularSpeed);
    // usartprintf("\n>intergralAngulaire:%lf\n",intergralAngulaire);
    // usartprintf("\n>erreurvittesseAngulaire:%lf\n",erreurvittesseAngulaire);
    // usartprintf("\n>motorcontrolAngle:%lf\n",motorcontrolAngle);
    // usartprintf("\n>consignevittesseAngulaire:%lf\n",consignevittesseAngulaire);
	usartprintf("\n>consigneX:%lf\n",consigneX);
	usartprintf("\n>consigneY:%lf\n",consigneY);
    usartprintf("\n>consigneAngle:%lf\n",consigneAngle);
    usartprintf("\n>ErreurAngulaire:%lf\n",angleErreur);

    usartprintf("\n>consignevittesseLineaire:%lf\n",consignevittesseLineaire);
    usartprintf("\n>vitesseLineaire:%lf\n",linearSpeed);
    usartprintf("\n>intergralLineaire:%lf\n",intergralLineaire);
    usartprintf("\n>erreurvittesseLineaire:%lf\n",erreurvittesseLineaire);
    usartprintf("\n>distanceRobotPoint:%lf\n",distanceRobotPoint);    
    usartprintf("\n>erreurPositionLineairePoint:%lf\n",erreurPositionLineairePoint);

    usartprintf("\n>motorcontrolAngle:%lf\n",motorcontrolAngle);
    usartprintf("\n>motorcontrolLigne:%lf\n",motorcontrolLigne);

    //*********************
    //Controle des moteurs
    //*********************

    double controleMoteurR = -motorcontrolAngle + motorcontrolLigne;
    double controleMoteurL = motorcontrolAngle + motorcontrolLigne;
    asservissementControlMoteur(controleMoteurR,controleMoteurL,angularSpeed,linearSpeed);
    
}





void setAngularAsservissement(double angle){
    consigneAngle = angle;
    asservissementType = ANGULAIRE;
}

void setLinearAsservissement(double x,double y,bool arriere){
    xwait = x;
    ywait = y;
	consigneangleX = x;
	consigneangleY = y;
    consigneAngle = calculAngle(xwait,ywait,actualPostion);
    if(arriere){
        asservissementType = LINEAIREARRIERE;
        consigneAngle = mod_angle(consigneAngle+180);
    }
    else{
        asservissementType = LINEAIREAVANT;
    }    
}

void asservissementControlMoteur(double controleMoteurR, double controleMoteurL, double angularSpeedSecu, double linearSpeedSecu){

    if(angularSpeedSecu>VITESSEANGULAIREMAXSECU*2.5){
        erreurControlMoteur = true;
    }
    else if(angularSpeedSecu<-VITESSEANGULAIREMAXSECU*2.5){
        erreurControlMoteur = true;
    }
    if(linearSpeedSecu>VITESSELINEAIREMAXSECU*2.5){
        erreurControlMoteur = true;
    }
    else if(linearSpeedSecu<-VITESSELINEAIREMAXSECU*2.5){
        erreurControlMoteur = true;
    }
    
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
    int controleMoteurL_i = controleMoteurL;
    int controleMoteurR_i = controleMoteurR;

    usartprintf("\n>controleMoteurL:%d\n",controleMoteurL_i);
    usartprintf("\n>controleMoteurR:%d\n",controleMoteurR_i);

   if(erreurControlMoteur){
    usartprintf("ERREUR ASSERVISEMENT");
    motorSpeedSignedL(0);
    motorSpeedSignedR(0);
   }
   else{
    motorSpeedSignedL(controleMoteurL_i);
    motorSpeedSignedR(controleMoteurR_i);
   }
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
    if((actualPostion.x - previousPosition.x) == 0){
        if((actualPostion.y - previousPosition.y)<0){
                anglevitesse = 90;
            }
            else{
                anglevitesse = -90;
            }
    }
    else{
        anglevitesse = (atan((actualPostion.y - previousPosition.y)/(actualPostion.x - previousPosition.x))*-RAD_TO_DEG);
        if((actualPostion.x - previousPosition.x)<0){
                anglevitesse = mod_angle(anglevitesse+180);
        }
        //erreurPositionAngulairePoint = mod_angle(angleAsserv-actualPostion.teta);  
    }
    //Gestion de la distance
	usartprintf("\n>anglevitesse:%lf\n",anglevitesse);
    if(anglevitesse-actualPostion.teta<-90 || anglevitesse-actualPostion.teta>90){
        return -longueurDeplacement;
    }
    return longueurDeplacement;
}