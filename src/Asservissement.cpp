#include "Asservissement.h"

asservissementType_t asservissementType = NONE;
position_t actualPostion;
position_t previousPosition;
double consigneAngle;
double consigneX;
double consigneY;
double consigneangleX;
double consigneangleY;

uint32_t asservissementTime;
bool erreurControlMoteur = true;
bool asservissementRun = false;

double xwait;
double ywait;



//variable d'asservissment
double consignevittesseAngulairePrecedante;
double intergralAngulaire;
double consignevittesseLineairePrecedante;
double intergralLineaire;

double linearSpeed;
double angularSpeed;
double distanceRobotPoint;
double consigneAngleTheorique;
double angleErreur;
double angleErreurPrecedente;
double angleErreurTheroique;
double erreurPositionLineairePoint;
double erreurPositionLineairePointPrecedente;
double consignevittesseAngulaire;
double erreurvittesseAngulaire;
double erreurvittesseAngulairePrecedente;
double motorcontrolAngle;
double consignevittesseLineaire;
double erreurvittesseLineaire;
double erreurvittesseLineaireprecedante;
double motorcontrolLigne;
double controleMoteurR;
double controleMoteurL;


motorSpeed_t asservissementLoopTime(robot* robot);
motorSpeed_t asservissementControlMoteur(double controleMoteurR, double controleMoteurL, double angularSpeedSecu, double linearSpeedSecu);
static double getAngularSpeed(void);
static double getLinearSpeed(void);



void asservissementSetup(void){
    previousPosition.teta = 0 ;previousPosition.x = 0; previousPosition.y = 0;
    actualPostion.teta = 0; actualPostion.x = 0; actualPostion.y=0;
    consignevittesseAngulairePrecedante = 0;
    consignevittesseLineairePrecedante = 0;
    intergralLineaire = 0;
    intergralAngulaire = 0;
    erreurControlMoteur = false;
    consigneAngle = actualPostion.teta;
    consigneX = actualPostion.x;
    consigneY = actualPostion.y;
    xwait = actualPostion.x;
    ywait = actualPostion.y;
    asservissementTime = get_uptime_ms();
    asservissementRun = true;
    erreurPositionLineairePointPrecedente = 0;
    erreurvittesseAngulairePrecedente = 0;
    erreurvittesseLineaireprecedante =0;
    angleErreurPrecedente = 0;
    motorBrakeL(0);
    motorBrakeR(0);
}

motorSpeed_t asservissementLoop(robot* robot){
    static motorSpeed_t motorSpeed = {0,0};
    if(asservissementTime < get_uptime_ms() && asservissementRun == true){
        motorSpeed = asservissementLoopTime(robot);
        asservissementTime = get_uptime_ms() + 50;
    }
    return motorSpeed;
}

void asservissmentStop(void){
    asservissementRun = false;
    motorBrakeL(1);
    motorBrakeR(1);
    asservissementControlMoteur(0,0,0,0);
}

void setAngularAsservissement(double angle){
    asservissementRun = true;
    motorBrakeL(0);
    motorBrakeR(0);
    consigneAngle = angle;
    asservissementType = ANGULAIRE;
}

void setLinearAsservissement(double x,double y,bool arriere){
    asservissementRun = true;
    motorBrakeL(0);
    motorBrakeR(0);
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

double getLinearError(void){
    return erreurPositionLineairePoint;
}
double getAngularError(void){
    return angleErreur;
}

void printAllInformation(void){
	usartprintf("\n>consigneAngle:%lf\n",consigneAngle);
    usartprintf("\n>consigneX:%lf\n",consigneX);
    usartprintf("\n>consigneY:%lf\n",consigneY);
    usartprintf("\n>consigneangleX:%lf\n",consigneangleX);
    usartprintf("\n>consigneangleY:%lf\n",consigneangleY);
    usartprintf("\n>xwait:%lf\n",xwait);
    usartprintf("\n>ywait:%lf\n",ywait);
    usartprintf("\n>consignevittesseAngulairePrecedante:%lf\n",consignevittesseAngulairePrecedante);
    usartprintf("\n>intergralAngulaire:%lf\n",intergralAngulaire);
    usartprintf("\n>consignevittesseLineairePrecedante:%lf\n",consignevittesseLineairePrecedante);
    usartprintf("\n>intergralLineaire:%lf\n",intergralLineaire);
    usartprintf("\n>linearSpeed:%lf\n",linearSpeed);
    usartprintf("\n>angularSpeed:%lf\n",angularSpeed);
    usartprintf("\n>angleErreurTheroique:%lf\n",angleErreurTheroique);
    usartprintf("\n>angleErreur:%lf\n",angleErreur);
    usartprintf("\n>erreurPositionLineairePoint:%lf\n",erreurPositionLineairePoint);
    usartprintf("\n>consignevittesseAngulaire:%lf\n",consignevittesseAngulaire);
    usartprintf("\n>motorcontrolAngle:%lf\n",motorcontrolAngle);
    usartprintf("\n>consignevittesseLineaire:%lf\n",consignevittesseLineaire);
    usartprintf("\n>erreurvittesseLineaire:%lf\n",erreurvittesseLineaire);
    usartprintf("\n>motorcontrolLigne:%lf\n",motorcontrolLigne);
    usartprintf("\n>controleMoteurR:%lf\n",controleMoteurR);
    usartprintf("\n>controleMoteurL:%lf\n",controleMoteurL);
	
}



motorSpeed_t asservissementLoopTime(robot* robot){

   actualPostion = robot->getPosition();
   linearSpeed = getLinearSpeed();
   angularSpeed = getAngularSpeed();
   previousPosition = actualPostion;
   

    //calacul des erreurs d'angle et de position lineaire
   distanceRobotPoint = sqrt(pow((consigneX - actualPostion.x),2)+pow((consigneY - actualPostion.y),2));
   consigneAngleTheorique = calculAngle(consigneangleX,consigneangleY,actualPostion);
   if(asservissementType == LINEAIREARRIERE){
        consigneAngleTheorique = mod_angle(consigneAngleTheorique + 180);
   }
   if(distanceRobotPoint>DISTANCEMINFINASSERVANGULAIRE && (asservissementType == LINEAIREAVANT || asservissementType == LINEAIREARRIERE) ){
        consigneAngle = consigneAngleTheorique;
   }
   angleErreur = mod_angle(consigneAngle-actualPostion.teta);
   angleErreurTheroique = mod_angle(consigneAngleTheorique-actualPostion.teta);
   erreurPositionLineairePoint = distanceRobotPoint*cos(angleErreurTheroique*DEG_TO_RAD);
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
    consignevittesseAngulaire = angleErreur * KP_POSITIONANGULAIRE + (KD_POSITIONANGULAIRE*FREQUENCE)*(angleErreur - angleErreurPrecedente);
    angleErreurPrecedente = angleErreur;
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
    erreurvittesseAngulaire = consignevittesseAngulaire-angularSpeed;
    intergralAngulaire += erreurvittesseAngulaire * (PERIODE);

    motorcontrolAngle = erreurvittesseAngulaire * KP_VITESSEANGULAIRE + intergralAngulaire * KI_VITESSEANGULAIRE + (KD_VITESSEANGULAIRE/FREQUENCE)*(erreurvittesseAngulaire - erreurvittesseAngulairePrecedente);
    erreurvittesseAngulairePrecedente = erreurvittesseAngulaire;


    //*********************
    //CALCUL LINEAIRE
    //*********************
    
    //asservismsent de la position Lineaire
    consignevittesseLineaire = erreurPositionLineairePoint * KP_POSITIONLINEAIRE + (KD_POSITIONLINEAIRE*FREQUENCE)*(erreurPositionLineairePoint - erreurPositionLineairePointPrecedente);
    erreurPositionLineairePointPrecedente = erreurPositionLineairePoint;
    if (consignevittesseLineaire>consignevittesseLineairePrecedante+ACCELERATIONLINEAIREMAX/FREQUENCE){
        consignevittesseLineaire = consignevittesseLineairePrecedante+ACCELERATIONLINEAIREMAX/FREQUENCE;
    }
    else if (consignevittesseLineaire<consignevittesseLineairePrecedante-ACCELERATIONLINEAIREMAX/FREQUENCE){
        consignevittesseLineaire = consignevittesseLineairePrecedante-ACCELERATIONLINEAIREMAX/FREQUENCE;
    }
    if(consignevittesseLineaire>VITESSELINEAIREMAX && asservissementType == LINEAIREAVANT){
        consignevittesseLineaire = VITESSELINEAIREMAX;
    }
    else if (consignevittesseLineaire<-VITESSELINEAIREMAX && asservissementType == LINEAIREAVANT){
        consignevittesseLineaire = -VITESSELINEAIREMAX;
    }
    if(consignevittesseLineaire> VITESSELINEAIREMAXARR && asservissementType == LINEAIREARRIERE){
        consignevittesseLineaire = VITESSELINEAIREMAXARR;
    }
    else if (consignevittesseLineaire<-VITESSELINEAIREMAXARR && asservissementType == LINEAIREARRIERE){
        consignevittesseLineaire = -VITESSELINEAIREMAXARR;
    }
    consignevittesseLineairePrecedante = consignevittesseLineaire;


    //asservissemnt de la vitesse Lineaire
    erreurvittesseLineaire = consignevittesseLineaire-linearSpeed;
    intergralLineaire += erreurvittesseLineaire * (PERIODE);

    motorcontrolLigne = erreurvittesseLineaire * KP_VITESSELINEAIRE + intergralLineaire * KI_VITESSELINEAIRE + (KD_VITESSELINEAIRE/FREQUENCE) * (erreurvittesseLineaire - erreurvittesseLineaireprecedante);
    erreurvittesseLineaireprecedante = erreurvittesseLineaire;


    //*********************
    //Controle des moteurs
    //*********************

    controleMoteurR = -motorcontrolAngle + motorcontrolLigne;
    controleMoteurL = motorcontrolAngle + motorcontrolLigne;
    return asservissementControlMoteur(controleMoteurR,controleMoteurL,angularSpeed,linearSpeed);
    
}

motorSpeed_t asservissementControlMoteur(double fcontroleMoteurR, double fcontroleMoteurL, double angularSpeedSecu, double linearSpeedSecontrol){
    motorSpeed_t motorSpeed;

    if(angularSpeedSecu>VITESSEANGULAIREMAXSECU*2.5){
        erreurControlMoteur = true;
    }
    else if(angularSpeedSecu<-VITESSEANGULAIREMAXSECU*2.5){
        erreurControlMoteur = true;
    }
    if(linearSpeedSecontrol>VITESSELINEAIREMAXSECU*2.5){
        erreurControlMoteur = true;
    }
    else if(linearSpeedSecontrol<-VITESSELINEAIREMAXSECU*2.5){
        erreurControlMoteur = true;
    }
    
    if(fcontroleMoteurR>100){
        fcontroleMoteurR = 100;
    }
    else if (fcontroleMoteurR<-100){
        fcontroleMoteurR = -100;
    }
    if(fcontroleMoteurL>100){
        fcontroleMoteurL = 100;
    }
    else if (fcontroleMoteurL<-100){
        fcontroleMoteurL = -100;
    }

   if(erreurControlMoteur){
    usartprintf("ERREUR ASSERVISEMENT");
    motorSpeed.L = 0;
    motorSpeed.R = 0;
   }
   else{
    motorSpeed.L = (int)fcontroleMoteurL;
    motorSpeed.R = (int)fcontroleMoteurR;
   }
   return motorSpeed;
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
    if(anglevitesse-actualPostion.teta<-90 || anglevitesse-actualPostion.teta>90){
        return -longueurDeplacement;
    }
    return longueurDeplacement;
}