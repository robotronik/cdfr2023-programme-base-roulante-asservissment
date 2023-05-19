#include "asservissement.h"
#include "pid.h"
#include "rampgen.h"

asservissementType asservissementState = asservissementType::NONE;
positionD actualPosition;
positionD previousPosition;

uint64_t lastPIDus = 0;
			//kP, kI, kD, maxWindup (limit I value before kI), output min, output max
pid<double, 5> PositionAngulairePID(20, 0, 0.8, 45, true);
pid<double, 5> VitesseAngulairePID(1, 0.5, -0.03, VITESSEANGULAIREMAX*1.2, true);
pid<double, 5> PositionLineairePID(4, 0, 0, 50);
pid<double, 5> VitesseLineairePID(0.5, 0.1, 0, VITESSELINEAIREMAXAVANT*1.2);
positionasserv<double> PosLin(10, 10, VITESSELINEAIREMAXAVANT, ACCELERATIONLINEAIREMAXAVANT, ACCELERATIONLINEAIREMAXAVANT*1.2);
positionasserv<double> PosAng(1, 0.2, VITESSEANGULAIREMAX, ACCELERATIONANGULAIREMAX, ACCELERATIONANGULAIREMAX*1.2);

double consigneAngle;
double consigneX;
double consigneY;

uint32_t asservissementTime;
//Control if there was an error in the motor control values
bool erreurControlMoteur = true;
//Control if the loop is running
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

void SetPIDValues(int index, double kP, double kI, double kD)
{
	pid<double, 5>* selected = nullptr;
	switch (index)
	{
	case 0:
		selected = &PositionLineairePID;
		break;
	case 1: 
		selected = &VitesseLineairePID;
		break;
	case 2:
		selected = &PositionAngulairePID;
		break;
	case 3:
		selected = &VitesseAngulairePID;
		break;
	default:
		return;
	}
	selected->kP = kP;
	selected->kI = kI;
	selected->kD = kD;
}

void asservissementSetup(void){
	if (erreurControlMoteur || !asservissementRun)
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
		consigneAngle = actualPosition.theta;
		consigneX = actualPosition.x;
		consigneY = actualPosition.y;
		asservissementRun = true;
	}
}

void asservissementLoop(void){
	if(lastPIDus + 20 <= get_uptime_ms()){
		asservissementLoopTime();
	}
}

void asservissementStop(void)
{
	asservissementRun = false;
	asservissementState = asservissementType::NONE;
	asservissementControlMoteur(0,0);
}

void asservissementBrake(void)
{
	asservissementStop();
	motorBrakeL(1);
	motorBrakeR(1);
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
	uint64_t usNow = get_uptime_ms();
	double deltaTime = (usNow-lastPIDus)/1e3;
	lastPIDus = usNow;
	actualPosition = odometrieGetPosition();
	linearSpeed = getLinearSpeed(deltaTime);
	angularSpeed = getAngularSpeed(deltaTime);
	previousPosition = actualPosition;

	if (!asservissementRun || erreurControlMoteur)
	{
		asservissementControlMoteur(0,0);
		return;
	}
	
	switch (asservissementState)
	{
	case asservissementType::ANGULAIRE :
		//usartprintf("asserv angle %lf\n", consigneAngle);
		break;
	case asservissementType::LINEAIREAVANT :
		//usartprintf("asserv lin fwd %lf %lf\n", consigneX, consigneY);
		break;
	case asservissementType::LINEAIREARRIERE :
		//usartprintf("asserv lin bwd %lf %lf\n", consigneX, consigneY);
		break;
	
	default:
		//usartprintf("asserv stop\n");
		break;
	}

	
	//calacul des erreurs d'angle et de position lineaire
	double dx = consigneX - actualPosition.x;
	double dy = consigneY - actualPosition.y;
	double distanceRobotPoint = sqrt(dx*dx+dy*dy);
	double forwarderror = dx*cos(actualPosition.theta*DEG_TO_RAD)-dy*sin(actualPosition.theta*DEG_TO_RAD);
	double consigneAngleTheorique = calculAngle(consigneX,consigneY,actualPosition);
	switch (asservissementState)
	{
	case asservissementType::LINEAIREARRIERE :
		consigneAngleTheorique = mod_angle(consigneAngleTheorique + 180);
		[[fallthrough]];
	case asservissementType::LINEAIREAVANT : 
		if (distanceRobotPoint > DISTANCEMINFINASSERVANGULAIRE) //far enough from target to justify rotating
		{
			consigneAngle = consigneAngleTheorique; //override angle target
			//usartprintf("lin angle trgt\n");
		}
		break;
	
	default:
		//usartprintf("angle hold\n");
		break;
	}

	double angleErreur = mod_angle(consigneAngle-actualPosition.theta);
	double angleErreurTheorique = mod_angle(consigneAngleTheorique-actualPosition.theta);
	
   
	
	//usartprintf("robot delta x%lf y%lf t%lf\n", dx, dy, angleErreur);

	//*********************
	//CALCUL ANGULAIRE
	//*********************


	//asservismsent de la position angulaire
	/*PositionAngulairePID.target = consigneAngle;
	double consigneVitesseAngulaire = PositionAngulairePID.Tick(deltaTime, actualPosition.theta);
	consigneVitesseAngulaire = clamp<double>(consigneVitesseAngulaire, VitesseAngulairePID.target-ACCELERATIONANGULAIREMAX*deltaTime, 
		VitesseAngulairePID.target+ACCELERATIONANGULAIREMAX*deltaTime);
	VitesseAngulairePID.target = clamp<double>(consigneVitesseAngulaire, -VITESSEANGULAIREMAX, VITESSEANGULAIREMAX);*/

	VitesseAngulairePID.target = PosAng.Tick(deltaTime, angleErreur, angularSpeed);
	usartprintf("Target angular speed is %lf, current %lf, error %lf\n", VitesseAngulairePID.target, angularSpeed, angleErreur);
	motorcontrolAngle = VitesseAngulairePID.Tick(deltaTime, angularSpeed);
	//motorcontrolAngle = 0;

	//*********************
	//CALCUL LINEAIRE
	//*********************
	//usartprintf("angle error is %lf\n", angleErreur);
	//double consigneVitesseLineaire = 0;
	double ErreurLin = 0.0;
	switch (asservissementState)
	{
	case asservissementType::ANGULAIRE : //try to maintain pos
		{
			double erreurPositionAngulairePoint = forwarderror;

			//test random
			if(erreurPositionAngulairePoint < TOLERANCE*erreurPositionAngulairePoint)
			{
				ErreurLin=0;
			}
			else
			{
				ErreurLin = erreurPositionAngulairePoint;
			}
			//usartprintf("angle position hold\n");
		}
		break;
	case asservissementType::LINEAIREARRIERE : //move if we're looking in the right-ish direction
	case asservissementType::LINEAIREAVANT : 
		{
			double erreurPositionLineairePoint = forwarderror;
			if(abs(angleErreur)<10 || distanceRobotPoint < DISTANCEMINFINASSERVANGULAIRE){
				ErreurLin = erreurPositionLineairePoint;
				//usartprintf("linear move step\n");
				break;
			}
		}
		[[fallthrough]];
	default: //don't move
		ErreurLin = 0;
		//usartprintf("linear stop\n");
		break;
	}
	//usartprintf("Linear error is %lf\n", PositionLineairePID.target);
	/*PositionLineairePID.target = ErreurLin;
	consigneVitesseLineaire = PositionLineairePID.Tick(deltaTime, 0.0);
	//asservismsent de la position Lineaire
	consigneVitesseLineaire = clamp<double>(consigneVitesseLineaire, 
		VitesseLineairePID.target-ACCELERATIONLINEAIREMAXARRIERE*deltaTime, 
		VitesseLineairePID.target+ACCELERATIONLINEAIREMAXAVANT*deltaTime);
	VitesseLineairePID.target = clamp<double>(consigneVitesseLineaire,
		-VITESSELINEAIREMAXARRIERE,
		VITESSELINEAIREMAXAVANT);*/
	
	VitesseLineairePID.target = PosLin.Tick(deltaTime, ErreurLin, linearSpeed);


	//asservissemnt de la vitesse Lineaire

	motorcontrolLigne = VitesseLineairePID.Tick(deltaTime, linearSpeed);
	//motorcontrolLigne = 0;

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
	return (mod_angle(actualPosition.theta - previousPosition.theta))/deltaTime;
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
	//usartprintf("\n>anglevitesse:%lf\n",anglevitesse);
	double deltaangle = anglevitesse-actualPosition.theta;
	deltaangle = mod_angle(deltaangle);
	if(abs(deltaangle) > 90){
		return -longueurDeplacement;
	}
	return longueurDeplacement;
}