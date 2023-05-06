#include "asservissement.h"
#include "pid.h"

asservissementType asservissementState = asservissementType::NONE;
position_t actualPosition;
position_t previousPosition;

uint64_t lastPIDus = 0;
			//kP, kI, kD, maxWindup (limit I value before kI), output min, output max
pid<double, false> VitesseLineairePID(KP_VITESSELINEAIRE, KI_VITESSELINEAIRE, KD_VITESSELINEAIRE, VITESSELINEAIREMAX/2);
pid<double, false> PositionLineairePID(KP_POSITIONLINEAIRE, 0, KD_POSITIONLINEAIRE, 50);
pid<double, true> VitesseAngulairePID(KP_VITESSEANGULAIRE, KI_VITESSEANGULAIRE, KD_VITESSEANGULAIRE, VITESSEANGULAIREMAX/2);
pid<double, true> PositionAngulairePID(KP_POSITIONANGULAIRE, 0, KD_POSITIONANGULAIRE, 45);

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

double linearSpeed;
double angularSpeed;
double motorcontrolLigne;
double motorcontrolAngle;
double controleMoteurL, controleMoteurR;



void asservissementLoopTime(void);
void asservissementControlMoteur(double controleMoteurR, double controleMoteurL, double angularSpeedSecu, double linearSpeedSecu);
static double getAngularSpeed(double DeltaTime);
static double getLinearSpeed(double DeltaTime);



void asservissementSetup(void){
	previousPosition = odometrieGetPosition();
	actualPosition = odometrieGetPosition();
	erreurControlMoteur = false;
	consigneAngle = actualPosition.teta;
	consigneX = actualPosition.x;
	consigneY = actualPosition.y;
	xwait = actualPosition.x;
	ywait = actualPosition.y;
	asservissementTime = get_uptime_ms();
	asservissementRun = true;
}

void asservissementLoop(void){
	if(asservissementTime < get_uptime_ms()){
		asservissementLoopTime();
		asservissementTime = get_uptime_ms() + 50;
	}
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
	asservissementState = asservissementType::ANGULAIRE;
}

void setLinearAsservissement(double x,double y,bool arriere){
	asservissementRun = true;
	motorBrakeL(0);
	motorBrakeR(0);
	xwait = x;
	ywait = y;
	consigneangleX = x;
	consigneangleY = y;
	consigneAngle = calculAngle(xwait,ywait,actualPosition);
	if(arriere){
		asservissementState = asservissementType::LINEAIREARRIERE;
		consigneAngle = mod_angle(consigneAngle+180);
	}
	else{
		asservissementState = asservissementType::LINEAIREAVANT;
	}    
}

void printAllInformation(void){
	usartprintf("\n>consigneAngle:%lf\n",consigneAngle);
	usartprintf("\n>consigneX:%lf\n",consigneX);
	usartprintf("\n>consigneY:%lf\n",consigneY);
	usartprintf("\n>consigneangleX:%lf\n",consigneangleX);
	usartprintf("\n>consigneangleY:%lf\n",consigneangleY);
	usartprintf("\n>xwait:%lf\n",xwait);
	usartprintf("\n>ywait:%lf\n",ywait);
	usartprintf("\n>linearSpeed:%lf\n",linearSpeed);
	usartprintf("\n>angularSpeed:%lf\n",angularSpeed);
	usartprintf("\n>motorcontrolAngle:%lf\n",motorcontrolAngle);
	usartprintf("\n>motorcontrolLigne:%lf\n",motorcontrolLigne);
	usartprintf("\n>controleMoteurR:%lf\n",controleMoteurR);
	usartprintf("\n>controleMoteurL:%lf\n",controleMoteurL);
	
}



void asservissementLoopTime(void){
	uint64_t usNow = get_uptime_us();
	double deltaTime = (usNow-lastPIDus)/1e6;
	lastPIDus = usNow;
	actualPosition = odometrieGetPosition();
	linearSpeed = getLinearSpeed(deltaTime);
	angularSpeed = getAngularSpeed(deltaTime);
	previousPosition = actualPosition;
   

	//calacul des erreurs d'angle et de position lineaire
	double distanceRobotPoint = sqrt(pow((consigneX - actualPosition.x),2)+pow((consigneY - actualPosition.y),2));
	double consigneAngleTheorique = calculAngle(consigneangleX,consigneangleY,actualPosition);
	if(asservissementState == asservissementType::LINEAIREARRIERE){
		consigneAngleTheorique = mod_angle(consigneAngleTheorique + 180);
	}
	if(distanceRobotPoint>DISTANCEMINFINASSERVANGULAIRE && (asservissementState == asservissementType::LINEAIREAVANT || asservissementState == asservissementType::LINEAIREARRIERE) ){
		consigneAngle = consigneAngleTheorique;
	}
	double angleErreur = mod_angle(consigneAngle-actualPosition.teta);
	double angleErreurTheorique = mod_angle(consigneAngleTheorique-actualPosition.teta);
	double erreurPositionLineairePoint = distanceRobotPoint*cos(angleErreurTheorique*DEG_TO_RAD);
	if(asservissementState == asservissementType::LINEAIREARRIERE){
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
	PositionAngulairePID.target = consigneAngle;
	double consigneVitesseAngulaire = PositionAngulairePID.Tick(deltaTime, actualPosition.teta);
	consigneVitesseAngulaire = clamp<double>(consigneVitesseAngulaire, VitesseAngulairePID.target-ACCELERATIONANGULAIREMAX*deltaTime, 
		VitesseAngulairePID.target+ACCELERATIONANGULAIREMAX*deltaTime);
	VitesseAngulairePID.target = clamp<double>(consigneVitesseAngulaire, -VITESSEANGULAIREMAX, VITESSEANGULAIREMAX);

	motorcontrolAngle = VitesseAngulairePID.Tick(deltaTime, angularSpeed);


	//*********************
	//CALCUL LINEAIRE
	//*********************
	
	//asservismsent de la position Lineaire
	PositionLineairePID.target = erreurPositionLineairePoint;
	double consigneVitesseLineaire = PositionLineairePID.Tick(deltaTime, 0.0);
	consigneVitesseLineaire = clamp<double>(consigneVitesseLineaire, 
		VitesseLineairePID.target-ACCELERATIONLINEAIREMAX*deltaTime, 
		VitesseLineairePID.target+ACCELERATIONLINEAIREMAX*deltaTime);
	VitesseLineairePID.target = clamp<double>(consigneVitesseLineaire,
		-VITESSELINEAIREMAX,
		VITESSELINEAIREMAX);


	//asservissemnt de la vitesse Lineaire

	motorcontrolLigne = VitesseLineairePID.Tick(deltaTime, linearSpeed);


	//*********************
	//Controle des moteurs
	//*********************

	controleMoteurR = -motorcontrolAngle + motorcontrolLigne;
	controleMoteurL = motorcontrolAngle + motorcontrolLigne;
	asservissementControlMoteur(controleMoteurR,controleMoteurL,angularSpeed,linearSpeed);
	
}

void asservissementControlMoteur(double fcontroleMoteurR, double fcontroleMoteurL, double angularSpeedSecu, double linearSpeedSecontrol){

	if(abs(angularSpeedSecu)>VITESSEANGULAIREMAXSECU*2.5){
		erreurControlMoteur = true;
	}
	if(abs(linearSpeedSecontrol)>VITESSELINEAIREMAXSECU*2.5){
		erreurControlMoteur = true;
	}
	fcontroleMoteurR = clamp<double>(fcontroleMoteurR, -100, 100);
	fcontroleMoteurL = clamp<double>(fcontroleMoteurL, -100, 100);

	if(erreurControlMoteur){
		usartprintf("ERREUR ASSERVISEMENT");
		motorSpeedSignedL(0);
		motorSpeedSignedR(0);
	}
	else{
		motorSpeedSignedL((int)fcontroleMoteurL);
		motorSpeedSignedR((int)fcontroleMoteurR);
	}
}


static double getAngularSpeed(double deltaTime){
	//*********************
	//CALCUL VITESSEANGULAIRE en mm seconde
	//*********************
	return (mod_angle(actualPosition.teta - previousPosition.teta))/deltaTime;
}

static double getLinearSpeed(double deltaTime){
	//*********************
	//CALCUL VITESSELINEAIRE en mm seconde
	//*********************
	double anglevitesse;
	double longueurDeplacement = sqrt(pow(actualPosition.y - previousPosition.y,2)+pow(actualPosition.x - previousPosition.x,2))/deltaTime;
	
	anglevitesse = atan2(actualPosition.y - previousPosition.y,actualPosition.x - previousPosition.x)*-RAD_TO_DEG;

	//Gestion de la distance
	usartprintf("\n>anglevitesse:%lf\n",anglevitesse);
	if(anglevitesse-actualPosition.teta<-90 || anglevitesse-actualPosition.teta>90){
		return -longueurDeplacement;
	}
	return longueurDeplacement;
}