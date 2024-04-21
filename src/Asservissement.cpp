#include "Asservissement.h"


Asservissement::Asservissement(robot* bot):
    pidLineaire(1,0.000,100),
    pidAngulaire(2,0.0,200),
    pidLineaireBlock(1,0.001,100),
    pidAngulaireBlock(2,0.001,200),
    robotAsservi(bot)
{

    currentState = ROTATION_DIRECT;

    //Réglage
    positionControlLineaire.vitesseMaxAv =  10000;
    positionControlLineaire.accelerationMaxAv = 300;//acceleration max AR
    positionControlLineaire.decelerationMaxAv = 300; //decelaration max AR
    positionControlLineaire.vitesseMaxAr = 10000;
    positionControlLineaire.accelerationMaxAr = 500; //acceleration max AV
    positionControlLineaire.decelerationMaxAr = 500; //decelaration max AV
    positionControlLineaire.decelationLineair = true;

    //Réglage
    positionControlAngulaire.vitesseMaxAv = 360;
    positionControlAngulaire.accelerationMaxAv = 600; 
    positionControlAngulaire.decelerationMaxAv = 600;
    positionControlAngulaire.vitesseMaxAr = 360;
    positionControlAngulaire.accelerationMaxAr = 600; 
    positionControlAngulaire.decelerationMaxAr = 600;
    positionControlAngulaire.decelationLineair = true;
}

Asservissement::~Asservissement()
{
}


motorSpeed_t Asservissement::asservissementLoop(){
    position_t actualPostion = robotAsservi->getPosition();
    double test = calculAngle(consigne.x,consigne.y,actualPostion);
    bool reTargetAngle = false;
    if(getLinearErrorReel() >= 100){
        reTargetAngle = true;
        setConsigneAngulaire(test,ROTATION_DIRECT);
    }
    else if(getLinearErrorReel() <= -100 ){
        reTargetAngle = true;
        setConsigneAngulaire(test+180,ROTATION_DIRECT);
    }

    double valPidLineaire;
    double valPidAngulaire;    
    if(positionControlLineaire.getPostion()==0){
        valPidLineaire = pidLineaireBlock.update(getLinearErrorReel()-positionControlLineaire.getPostion(),robotAsservi->getPosition_Time());
        pidLineaire.reset();
    }
    else{
        valPidLineaire = pidLineaire.update(getLinearErrorReel()-positionControlLineaire.getPostion(),robotAsservi->getPosition_Time());
        pidLineaireBlock.reset();
    }

    if(positionControlAngulaire.getPostion()==0 || reTargetAngle){
        valPidAngulaire = pidAngulaireBlock.update(getAngularErrorReel()-positionControlAngulaire.getPostion(),robotAsservi->getPosition_Time());
        pidAngulaire.reset();
    }
    else{
        valPidAngulaire = pidAngulaire.update(getAngularErrorReel()-positionControlAngulaire.getPostion(),robotAsservi->getPosition_Time());
        pidAngulaireBlock.reset();
    }
    
    // usartprintf(">erreurAngulaire:%lf\n>erreurLineaire:%lf\n",getAngularErrorReel(),getLinearErrorReel());
    // usartprintf(">pidLineaire:%lf\n>valPidAngulaire:%lf\n",valPidLineaire,valPidAngulaire);
    // usartprintf(">p:%lf\n>i:%lf\n>d:%lf\n",pidAngulaireBlock.valP,pidAngulaireBlock.valI,pidAngulaireBlock.valD);
    //usartprintf(">rotatif:%lf\n",consigne.teta);
    
    motorSpeed_t speed = {(int)(valPidLineaire+valPidAngulaire),(int)(valPidLineaire-valPidAngulaire)};
    // usartprintf(">speedL:%d\n>speedR:%d\n",speed.R,speed.L);
    
    return speed;
}


//******************************************************
//Set consigne
//******************************************************

void Asservissement::setConsigneAngulaire(double angle, sensRotation_t rotation){
    double errorBefor = getAngularErrorReel()-positionControlAngulaire.getPostion();
    currentState = rotation;
    consigne.teta = mod_angle(angle);
    positionControlAngulaire.setPosition(getAngularErrorReel() - errorBefor);
    positionControlAngulaire.setConsigne(0);
}

void Asservissement::setConsigneLineaire(double x, double y){
    double errorBefor = getLinearErrorReel()-positionControlLineaire.getPostion();
    consigne.x = x;
    consigne.y = y;
    positionControlLineaire.setPosition(getLinearErrorReel() - errorBefor);
    positionControlLineaire.setConsigne(0);
}

void Asservissement::setConsigneLookAtForward(double x, double y, sensRotation_t rotation){
    setConsigneAngulaire(calculAngle(x,y,robotAsservi->getPosition()),rotation);
}

void Asservissement::setConsigneLookAtBackward(double x, double y, sensRotation_t rotation){
    setConsigneAngulaire(mod_angle(calculAngle(x,y,robotAsservi->getPosition())+180),rotation);
}

void Asservissement::setConsigneStop(void){
    positionControlLineaire.stop();
    positionControlAngulaire.stop();
}

void Asservissement::setConsigne(position_t position){
    consigne = position;
}

double Asservissement::getAngularError(void){
    return positionControlLineaire.getPostion();
}

double Asservissement::getLinearError(void){
    return positionControlAngulaire.getPostion();
}

void Asservissement::reset(void){
    currentState = ROTATION_DIRECT;
    consigne.teta = robotAsservi->getPosition_Teta();
    consigne.x = robotAsservi->getPosition_X();
    consigne.y = robotAsservi->getPosition_Y();
    positionControlLineaire.reset(0);
    positionControlAngulaire.reset(0);
    pidAngulaire.reset();
    pidAngulaireBlock.reset();
    pidLineaireBlock.reset();
    pidLineaire.reset();
}

//******************************************************
//Recupération de l'erreur Réel
//******************************************************

double Asservissement::getAngularErrorReel(void){
    double angleErreur = mod_angle(consigne.teta-robotAsservi->getPosition_Teta());
    
    if(angleErreur>0 && currentState == ROTATION_TRIGO){
        angleErreur -= 360;
    }
    else if(angleErreur<0 && currentState == ROTATION_HORRAIRE){
        angleErreur += 360;
    }
    if(angleErreur<90 && angleErreur>-90 && currentState != ROTATION_DIRECT ){
        currentState = ROTATION_DIRECT;
    }
    return angleErreur;
}

double Asservissement::getLinearErrorReel(void){
    //Première partie, connaitre l'angle formé entre le robot et le point de la consigne
    double erreurAngulaireSurPoint = calculAngle(consigne.x,consigne.y,robotAsservi->getPosition());
    
    // Calculer la distance entre le robot et la droite orthogonal qui passe par le point de la consigne
    double distanceRobotPoint = sqrt(pow((consigne.x - robotAsservi->getPosition_X()),2)+pow((consigne.y -robotAsservi->getPosition_Y()),2));
    return (distanceRobotPoint*cos((erreurAngulaireSurPoint-robotAsservi->getPosition_Teta())*DEG_TO_RAD));
}



//******************************************************
//Robot Status
//******************************************************

bool Asservissement::robotMovingIsFinish(void){
    return robotTurningIsFinish() || robotRunningIsFinish();
}
bool Asservissement::robotTurningIsFinish(void){
    return positionControlAngulaire.getPostion()!=0 || (getAngularErrorReel()>1 || getAngularErrorReel()<-1);
}
bool Asservissement::robotRunningIsFinish(void){
    return positionControlLineaire.getPostion()!=0 || (getLinearError()>1 || getLinearError()<-1);
}
int Asservissement::getBrakingDistance(void){
    return -positionControlLineaire.getBrakingDistance();
}
