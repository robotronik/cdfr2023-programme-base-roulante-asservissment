#include "ControlePositionAsservissement.h"

double ValeDepart;


ControlePositionAsservissement::ControlePositionAsservissement(robot* bot):
    robotAsservi(bot),
    systemAsservisement(bot)
{
    //Réglage
    positionControlLineaire.vitesseMaxAv =  500;
    positionControlLineaire.accelerationMaxAv = 250; 
    positionControlLineaire.decelerationMaxAv = 250;
    positionControlLineaire.vitesseMaxAr = 500;
    positionControlLineaire.accelerationMaxAr = 250; 
    positionControlLineaire.decelerationMaxAr = 250;

    //Réglage
    positionControlAngulaire.vitesseMaxAv = 45;
    positionControlAngulaire.accelerationMaxAv = 25; 
    positionControlAngulaire.decelerationMaxAv = 25;
    positionControlAngulaire.vitesseMaxAr = 45;
    positionControlAngulaire.accelerationMaxAr = 25; 
    positionControlAngulaire.decelerationMaxAr = 25;

}

ControlePositionAsservissement::~ControlePositionAsservissement()
{
}


motorSpeed_t ControlePositionAsservissement::ControlePositionAsservissementLoop(){

    //currentConsigne.teta = positionControlAngulaire.getPostion();
    double attenuationLineaire = positionControlLineaire.getPostion();

    //systemAsservisement.setConsigne(currentConsigne);  
    return systemAsservisement.asservissementLoop(attenuationLineaire,0);
}


void ControlePositionAsservissement::setConsigneAngulaire(double angle){
    positionControlAngulaire.setPostion(angle);
    finalConsigne.teta = angle;
}
void ControlePositionAsservissement::setConsigneLineaire(double x, double y){
    double errorBefor = systemAsservisement.getLinearError()-positionControlLineaire.getPostion();
    systemAsservisement.consigne.x = x;
    systemAsservisement.consigne.y = y;
    positionControlLineaire.initialisePosition(systemAsservisement.getLinearError() - errorBefor);
    positionControlLineaire.setPostion(0);
}