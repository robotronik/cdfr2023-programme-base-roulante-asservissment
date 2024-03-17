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
    positionControlAngulaire.vitesseMaxAv = 180;
    positionControlAngulaire.accelerationMaxAv = 180; 
    positionControlAngulaire.decelerationMaxAv = 180;
    positionControlAngulaire.vitesseMaxAr = 180;
    positionControlAngulaire.accelerationMaxAr = 180; 
    positionControlAngulaire.decelerationMaxAr = 180;

}

Asservissement::~Asservissement()
{
}


motorSpeed_t Asservissement::asservissementLoop(){
    position_t actualPostion = robotAsservi->getPosition();
    double test = calculAngle(consigne.x,consigne.y,actualPostion);
    if(getLinearErrorReel() >= 300){
        setConsigneAngulaire(test,ROTATION_DIRECT);
    }

    double valPidLineaire = pidLineaire.update(getLinearErrorReel()-positionControlLineaire.getPostion(),robotAsservi->getPosition_Time());
    double valPidAngulaire = pidAngulaire.update(getAngularErrorReel()-positionControlAngulaire.getPostion(),robotAsservi->getPosition_Time());
    
    //usartprintf(">erreurAngulaire:%lf\n>erreurLineaire:%lf\n>teta:%lf\n",getAngularErrorReel(),getLinearErrorReel());
    //usartprintf(">pidLineaire:%lf\n>pidLineaire:%lf\n",valPidLineaire,valPidAngulaire);
    //usartprintf(">rotatif:%lf\n",consigne.teta);
    
    motorSpeed_t speed = {(int)(valPidLineaire+valPidAngulaire),(int)(valPidLineaire-valPidAngulaire)};
    
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
    return positionControlAngulaire.getPostion()!=0 || positionControlLineaire.getPostion()!=0;
}
bool Asservissement::robotTurningIsFinish(void){
    return positionControlAngulaire.getPostion()!=0;
}
bool Asservissement::robotRunningIsFinish(void){
    return positionControlLineaire.getPostion()!=0;
}
int Asservissement::getBrakingDistance(void){
    return positionControlLineaire.getBrakingDistance();
}
