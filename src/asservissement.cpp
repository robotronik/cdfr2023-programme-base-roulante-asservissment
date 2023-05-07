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

uint32_t asservissementTime;
bool erreurControlMoteur = true;
bool asservissementRun = false;

double linearSpeed;
double angularSpeed;
double motorcontrolLigne;
double motorcontrolAngle;
double controleMoteurL, controleMoteurR;



void asservissementLoopTime(void);
void asservissementControlMoteur(double controleMoteurR, double controleMoteurL);
static double getAngularSpeed(double DeltaTime);
static double getLinearSpeed(double DeltaTime);



void asservissementSetup(void){
	if (erreurControlMoteur)
	{
		VitesseLineairePID.Reset();
		PositionLineairePID.Reset();
		VitesseAngulairePID.Reset();
		PositionAngulairePID.Reset();
		erreurControlMoteur = false;
	}
	if (!asservissementRun)
	{
		previousPosition = odometrieGetPosition();
		actualPosition = odometrieGetPosition();
		consigneAngle = actualPosition.teta;
		consigneX = actualPosition.x;
		consigneY = actualPosition.y;
		asservissementRun = true;
	}
}

void asservissementLoop(void){
	if(asservissementTime < get_uptime_ms()){
		asservissementLoopTime();
		asservissementTime = get_uptime_ms() + 20;
	}
}

void asservissmentStop(void){
	asservissementRun = false;
	asservissementState = asservissementType::NONE;
	motorBrakeL(1);
	motorBrakeR(1);
	asservissementControlMoteur(0,0);
}

void setAngularAsservissement(double angle){
	asservissementRun = true;
	motorBrakeL(0);
	motorBrakeR(0);
	consigneAngle = angle;
	consigneX = actualPosition.x;
	consigneY = actualPosition.y;
	asservissementState = asservissementType::ANGULAIRE;
}

void setLinearAsservissement(double x,double y,bool arriere){
	asservissementRun = true;
	motorBrakeL(0);
	motorBrakeR(0);
	consigneX = x;
	consigneY = y;
	consigneAngle = calculAngle(x,y,actualPosition);
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
	double dx = consigneX - actualPosition.x;
	double dy = consigneY - actualPosition.y;
	double distanceRobotPoint = sqrt(dx*dx+dy*dy);
	double consigneAngleTheorique = calculAngle(consigneX,consigneY,actualPosition);
	switch (asservissementState)
	{
	case asservissementType::LINEAIREARRIERE :
		consigneAngleTheorique = mod_angle(consigneAngleTheorique + 180);
		[[fallthrough]];
	case asservissementType::LINEAIREAVANT : 
		consigneAngle = consigneAngleTheorique; //override angle target
		break;
	
	default:
		break;
	}

	double angleErreur = mod_angle(consigneAngle-actualPosition.teta);
	double angleErreurTheorique = mod_angle(consigneAngleTheorique-actualPosition.teta);
	
   
	

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

	double consigneVitesseLineaire = 0;
	switch (asservissementState)
	{
	case asservissementType::ANGULAIRE : //try to maintain pos
		{
			double erreurPositionAngulairePoint = distanceRobotPoint*cos(angleErreur*DEG_TO_RAD);
			PositionLineairePID.target = erreurPositionAngulairePoint;
			consigneVitesseLineaire = PositionLineairePID.Tick(deltaTime, 0.0);
		}
		break;
	case asservissementType::LINEAIREARRIERE : //move if we're looking in the right-ish direction
	case asservissementType::LINEAIREAVANT : 
		{
			double erreurPositionLineairePoint = distanceRobotPoint*cos(angleErreurTheorique*DEG_TO_RAD);
			if(asservissementState == asservissementType::LINEAIREARRIERE){
				erreurPositionLineairePoint = -erreurPositionLineairePoint;
			}
			if(abs(angleErreur)<10){
				PositionLineairePID.target = erreurPositionLineairePoint;
				consigneVitesseLineaire = PositionLineairePID.Tick(deltaTime, 0.0);
				break;
			}
		}
		[[fallthrough]];
	default: //don't move
		PositionLineairePID.target = 0;
		consigneVitesseLineaire = PositionLineairePID.Tick(deltaTime, 0.0);
		break;
	}
	//asservismsent de la position Lineaire
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
	asservissementControlMoteur(controleMoteurR,controleMoteurL);
	
}

void asservissementControlMoteur(double fcontroleMoteurR, double fcontroleMoteurL){

	if(abs(angularSpeed)>VITESSEANGULAIREMAXSECU){
		erreurControlMoteur = true;
	}
	if(abs(linearSpeed)>VITESSELINEAIREMAXSECU){
		erreurControlMoteur = true;
	}
	double desatval = max(abs(fcontroleMoteurL), abs(fcontroleMoteurR)); //desaturate motors proportionnally
	desatval = max(desatval, 100.0)/100.0;
	
	fcontroleMoteurR = fcontroleMoteurR/desatval;
	fcontroleMoteurL = fcontroleMoteurL/desatval;

	if(erreurControlMoteur){
		usartprintf("ERREUR ASSERVISEMENT");
		motorSpeedSignedL(0);
		motorSpeedSignedR(0);
	}
	else{
		motorSpeedSignedL(fcontroleMoteurL);
		motorSpeedSignedR(fcontroleMoteurR);
	}
}


static double getAngularSpeed(double deltaTime){
	//*********************
	//CALCUL VITESSEANGULAIRE en deg/seconde
	//*********************
	return (mod_angle(actualPosition.teta - previousPosition.teta))/deltaTime;
}

static double getLinearSpeed(double deltaTime){
	//*********************
	//CALCUL VITESSELINEAIRE en mm/seconde
	//*********************
	double dx = actualPosition.x - previousPosition.x;
	double dy = actualPosition.y - previousPosition.y;
	double longueurDeplacement = sqrt(dx*dx+dy*dy)/deltaTime;
	
	double anglevitesse = atan2(actualPosition.y - previousPosition.y,actualPosition.x - previousPosition.x)*-RAD_TO_DEG;

	//Gestion de la distance
	usartprintf("\n>anglevitesse:%lf\n",anglevitesse);
	double deltaangle = anglevitesse-actualPosition.teta;
	deltaangle = mod_angle(deltaangle);
	if(abs(deltaangle) > 90){
		return -longueurDeplacement;
	}
	return longueurDeplacement;
}