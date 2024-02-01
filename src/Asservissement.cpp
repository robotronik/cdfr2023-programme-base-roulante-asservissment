#include "Asservissement.h"


Asservissement::Asservissement(robot* bot):
    pidLineaire(0.5,0.0001,0),
    pidAngulaire(1,0.0001,0),
    robotAsservi(bot)
{

    currentState = ROTATION_DIRECT;

    //Réglage
    positionControlLineaire.vitesseMaxAv =  500;
    positionControlLineaire.accelerationMaxAv = 250; 
    positionControlLineaire.decelerationMaxAv = 250;
    positionControlLineaire.vitesseMaxAr = 500;
    positionControlLineaire.accelerationMaxAr = 250; 
    positionControlLineaire.decelerationMaxAr = 250;

    //Réglage
    positionControlAngulaire.vitesseMaxAv = 90;
    positionControlAngulaire.accelerationMaxAv = 90; 
    positionControlAngulaire.decelerationMaxAv = 90;
    positionControlAngulaire.vitesseMaxAr = 90;
    positionControlAngulaire.accelerationMaxAr = 90; 
    positionControlAngulaire.decelerationMaxAr = 90;

}

Asservissement::~Asservissement()
{
}


motorSpeed_t Asservissement::asservissementLoop(){
    position_t actualPostion = robotAsservi->getPosition();
    double test = calculAngle(consigne.x,consigne.y,actualPostion);
    if(getLinearError() >= 300){
        setConsigneAngulaire(test,ROTATION_DIRECT);
    }

    double valPidLineaire = pidLineaire.update(getLinearError()-positionControlLineaire.getPostion(),robotAsservi->getPosition_Time());
    double valPidAngulaire = pidAngulaire.update(getAngularError()-positionControlAngulaire.getPostion(),robotAsservi->getPosition_Time());
    
    //usartprintf(">erreurAngulaire:%lf\n>erreurLineaire:%lf\n>teta:%lf\n",getAngularError(),getLinearError());
    //usartprintf(">pidLineaire:%lf\n>pidLineaire:%lf\n",valPidLineaire,valPidAngulaire);
    //usartprintf(">rotatif:%lf\n",consigne.teta);
    
    motorSpeed_t speed = {(int)(valPidLineaire+valPidAngulaire),(int)(valPidLineaire-valPidAngulaire)};
    
    return speed;
}


//******************************************************
//Set consigne
//******************************************************

void Asservissement::setConsigneAngulaire(double angle, sensRotation_t rotation){
    double errorBefor = getAngularError()-positionControlAngulaire.getPostion();
    currentState = rotation;
    consigne.teta = mod_angle(angle);
    positionControlAngulaire.setPosition(getAngularError() - errorBefor);
    positionControlAngulaire.setConsigne(0);
}

void Asservissement::setConsigneLineaire(double x, double y){
    double errorBefor = getLinearError()-positionControlLineaire.getPostion();
    consigne.x = x;
    consigne.y = y;
    positionControlLineaire.setPosition(getLinearError() - errorBefor);
    positionControlLineaire.setConsigne(0);
}

void Asservissement::setConsigneLookAt(double x, double y, sensRotation_t rotation){
    setConsigneAngulaire(calculAngle(x,y,robotAsservi->getPosition()),rotation);
}

 void Asservissement::setConsigneStop(void){
    positionControlLineaire.stop();
    positionControlAngulaire.stop();
}

void Asservissement::setConsigne(position_t position){
    consigne = position;
}


//******************************************************
//Recupération de l'erreur
//******************************************************

double Asservissement::getAngularError(void){
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

double Asservissement::getLinearError(void){
    //Première partie, connaitre l'angle formé entre le robot et le point de la consigne
    double erreurAngulaireSurPoint = calculAngle(consigne.x,consigne.y,robotAsservi->getPosition());
    
    // Calculer la distance entre le robot et la droite orthogonal qui passe par le point de la consigne
    double distanceRobotPoint = sqrt(pow((consigne.x - robotAsservi->getPosition_X()),2)+pow((consigne.y -robotAsservi->getPosition_Y()),2));
    return (distanceRobotPoint*cos((erreurAngulaireSurPoint-robotAsservi->getPosition_Teta())*DEG_TO_RAD));
}



//******************************************************
//Robot Status
//******************************************************

bool Asservissement::robotIsMoving(void){
    return positionControlAngulaire.getPostion()!=0 || positionControlLineaire.getPostion()!=0;
}
bool Asservissement::robotIsTurning(void){
    return positionControlAngulaire.getPostion()!=0;
}
bool Asservissement::robotIsRunning(void){
    return positionControlLineaire.getPostion()!=0;
}
