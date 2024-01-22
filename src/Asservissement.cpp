#include "Asservissement.h"

double ValeDepart;

Asservissement::Asservissement(robot* bot):
    pidLineaire(1,0.0001,0),
pidAngulaire(1,0.0001,0),
    robotAsservi(bot)
{
    //Réglage
    positionControlLineaire.vitesseMaxAv =  500;
    positionControlLineaire.accelerationMaxAv = 250; 
    positionControlLineaire.decelerationMaxAv = 250;
    positionControlLineaire.vitesseMaxAr = 500;
    positionControlLineaire.accelerationMaxAr = 250; 
    positionControlLineaire.decelerationMaxAr = 250;


    positionControlAngulaire.setPostion(0);
    consigne.x = 1000;
    consigne.y = 0;
    positionControlLineaire.setPostion(getLinearError());
    ValeDepart = getLinearError();
}

Asservissement::~Asservissement()
{
}


motorSpeed_t Asservissement::asservissementLoop(robot* robot){

    consigne.teta = positionControlAngulaire.getPostion();
    double diminutionErreurLineaire = positionControlLineaire.getPostion();

    double valPidLineaire = pidLineaire.update(getLinearError()-ValeDepart+diminutionErreurLineaire,robotAsservi->getPosition_Time());
    double valPidAngulaire = pidAngulaire.update(getAngularError(),robotAsservi->getPosition_Time());

    
    usartprintf(">erreurAngulaire:%lf\n>erreurLineaire:%lf\n>teta:%lf\n",getAngularError(),getLinearError());
    usartprintf(">pidLineaire:%lf\n>pidLineaire:%lf\n",valPidLineaire,valPidAngulaire);
    usartprintf(">rotatif:%lf\n",consigne.teta);
    
    
    motorSpeed_t speed = {valPidLineaire+valPidAngulaire,valPidLineaire-valPidAngulaire};
    
    return speed;
}





double Asservissement::getAngularError(void){
    return mod_angle(consigne.teta-robotAsservi->getPosition_Teta());
}

double Asservissement::getLinearError(void){
    //Première partie, connaitre l'angle formé entre le robot et le point de la consigne
    position_t position = robotAsservi->getPosition();
    double erreurAngulaireSurPoint;
    if((consigne.x - position.x) == 0){
        if((consigne.y - position.y)<0){
                erreurAngulaireSurPoint = 90;
            }
            else{
                erreurAngulaireSurPoint = -90;
            }
    }
    else{
        erreurAngulaireSurPoint = -(atan((consigne.y - position.y)/(consigne.x - position.x))*RAD_TO_DEG);
        if((consigne.x - position.x)<0){
            erreurAngulaireSurPoint -= 180;
        }
        //Gestion de la marche arrière
    }
    erreurAngulaireSurPoint = mod_angle(erreurAngulaireSurPoint);

    // Calculer la distance entre le robot et la droite orthogonal qui passe par le point de la consigne
    double distanceRobotPoint = sqrt(pow((consigne.x - position.x),2)+pow((consigne.y - position.y),2));
    return (distanceRobotPoint*cos(erreurAngulaireSurPoint*DEG_TO_RAD));
}
