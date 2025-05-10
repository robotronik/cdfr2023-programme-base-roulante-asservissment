#include "Asservissement.h"
#include "Motor.h"
#include "AsservissementMath.h"
#include "config.h"
#include "uart.h"
#include "clock.h"
#include <math.h>


Asservissement::Asservissement():
    pidLineaire(1,0.000,100),
    pidAngulaire(2,0.0,200),
    pidLineaireBlock(1,0.001,100),
    pidAngulaireBlock(2,0.001,200)
{
    nextTime =  get_uptime_ms();
    currentState = Rotation::SHORTEST;

    positionControlLineaire.vitesseMaxAv.setRange(0,10000);
    positionControlLineaire.accelerationMaxAv.setRange(0,300);
    positionControlLineaire.decelerationMaxAv.setRange(0,300);
    positionControlLineaire.vitesseMaxAr.setRange(0,10000);
    positionControlLineaire.accelerationMaxAr.setRange(0,300);
    positionControlLineaire.decelerationMaxAr.setRange(0,300);
    positionControlLineaire.decelerationStopAv.setRange(0,2000);
    positionControlLineaire.decelerationStopAr.setRange(0,2000);

    positionControlAngulaire.vitesseMaxAv.setRange(0,360);
    positionControlAngulaire.accelerationMaxAv.setRange(0,300);
    positionControlAngulaire.decelerationMaxAv.setRange(0,300);
    positionControlAngulaire.vitesseMaxAr.setRange(0,360);
    positionControlAngulaire.accelerationMaxAr.setRange(0,300);
    positionControlAngulaire.decelerationMaxAr.setRange(0,300);
    positionControlAngulaire.decelerationStopAv.setRange(positionControlAngulaire.decelerationMaxAv);
    positionControlAngulaire.decelerationStopAr.setRange(positionControlAngulaire.decelerationMaxAr);


    positionControlLineaire.vitesseMaxAv.setMax();
    positionControlLineaire.accelerationMaxAv.setMax();
    positionControlLineaire.decelerationMaxAv.setMax();
    positionControlLineaire.vitesseMaxAr.setMax();
    positionControlLineaire.accelerationMaxAr.setMax();
    positionControlLineaire.decelerationMaxAr.setMax();
    positionControlLineaire.decelerationStopAv.setMax();
    positionControlLineaire.decelerationStopAr.setMax();

    positionControlAngulaire.vitesseMaxAv.setMax();
    positionControlAngulaire.accelerationMaxAv.setMax();
    positionControlAngulaire.decelerationMaxAv.setMax();
    positionControlAngulaire.vitesseMaxAr.setMax();
    positionControlAngulaire.accelerationMaxAr.setMax();
    positionControlAngulaire.decelerationMaxAr.setMax();
    positionControlAngulaire.decelerationStopAv.setMax();
    positionControlAngulaire.decelerationStopAr.setMax();
}

void Asservissement::setAsservissementLoopPeriod(int period) {
    loopPeriod = period;
}

void Asservissement::loop(){
    // if (nextTime < get_uptime_ms()) {
    //     if (getPositionChanged()) {
    //         setTarget(pos);
    //     }
    //     nextTime = get_uptime_ms() + loopPeriod;
    //     asservissementLoop();
    // }
    if (getPositionChanged()) {
        setTarget(pos);
    }
    asservissementLoop();
}


void Asservissement::asservissementLoop(){
    double targetAngle = calculAngle(target.x,target.y,pos);
    bool reTargetAngle = false;
    double valPidLineaire;
    double valPidAngulaire;
    double realErrorAngular;
    double reduceErrorAngular;
    double realErrorLinear = getLinearErrorReel();
    double reduceErrorLinear = realErrorLinear-positionControlLineaire.getPostion();
    uint32_t timeLastPos = pos.time;

    if (realErrorLinear >= 100) {
        reTargetAngle = true;
        setTargetAngulaire(targetAngle,Rotation::SHORTEST);
    }
    else if (realErrorLinear <= -100) {
        reTargetAngle = true;
        setTargetAngulaire(targetAngle+180,Rotation::SHORTEST);
    }

    realErrorAngular = getAngularErrorReel();
    reduceErrorAngular = realErrorAngular-positionControlAngulaire.getPostion();

    //Calculate Linear command
    if (positionControlLineaire.getPostion()==0) {
        valPidLineaire = pidLineaireBlock.update(reduceErrorLinear,timeLastPos);
        pidLineaire.reset();
    }
    else {
        valPidLineaire = pidLineaire.update(reduceErrorLinear,timeLastPos);
        pidLineaireBlock.reset();
    }

    //Calculate Angular command
    if (positionControlAngulaire.getPostion()==0 || reTargetAngle) {
        valPidAngulaire = pidAngulaireBlock.update(reduceErrorAngular,timeLastPos);
        pidAngulaire.reset();
    }
    else {
        valPidAngulaire = pidAngulaire.update(reduceErrorAngular,timeLastPos);
        pidAngulaireBlock.reset();
    }

    //usartprintf(">speedL:%d\n>speedR:%d\n",(int)(valPidLineaire-valPidAngulaire),(int)(valPidLineaire+valPidAngulaire));
    motorA.SetSpeedSigned((int)(valPidLineaire-valPidAngulaire)); // TODO
    motorB.SetSpeedSigned((int)(valPidLineaire+valPidAngulaire));
    motorC.SetSpeedSigned((int)(valPidLineaire-valPidAngulaire));
}


//******************************************************
//Set target
//******************************************************

void Asservissement::setProtectedTargetAngulaire(double angle, Rotation rotation) {
    if (positionControlLineaire.getPostion()!=0) {
        target.x = pos.x;
        target.y = pos.y;
        positionControlLineaire.setPosition(0);
        positionControlLineaire.setTarget(0);
    }
    setTargetAngulaire(angle,rotation);
}

void Asservissement::setTargetAngulaire(double angle, Rotation rotation) {
    double errorBefor = getAngularErrorReel()-positionControlAngulaire.getPostion();
    currentState = rotation;
    target.a = mod_angle(angle);
    positionControlAngulaire.setPosition(getAngularErrorReel() - errorBefor);
    positionControlAngulaire.setTarget(0);
}

void Asservissement::setProtectedTargetLineaire(double x, double y) {
    if (positionControlAngulaire.getPostion()!=0) {
        target.a = mod_angle(pos.a);
        positionControlAngulaire.setPosition(0);
        positionControlAngulaire.setTarget(0);
    }
    setTargetLineaire(x,y);
}

void Asservissement::setTargetLineaire(double x, double y) {
    double errorBefor = getLinearErrorReel()-positionControlLineaire.getPostion();
    target.x = x;
    target.y = y;
    positionControlLineaire.setPosition(getLinearErrorReel() - errorBefor);
    positionControlLineaire.setTarget(0);
}

void Asservissement::setTargetLookAt(double x, double y, Rotation rotation) {
    double angleErreur = mod_angle(calculAngle(x,y,pos)-pos.a);
    if (angleErreur>0 && rotation == Rotation::CLOCKWISE) {
        angleErreur -= 360;
    }
    else if (angleErreur<0 && rotation == Rotation::ANTICLOCKWISE) {
        angleErreur += 360;
    }

    if (rotation == Rotation::SHORTEST)
        if (angleErreur<90 && angleErreur>-90) {
            setProtectedTargetAngulaire(calculAngle(x,y,pos),rotation);
        }
        else {
            setProtectedTargetAngulaire(mod_angle(calculAngle(x,y,pos)+180),rotation);
        }
    else {
        if (angleErreur<180 && angleErreur>-180) {
            setProtectedTargetAngulaire(calculAngle(x,y,pos),rotation);
        }
        else {
            setProtectedTargetAngulaire(mod_angle(calculAngle(x,y,pos)+180),rotation);
        }
    }
}

void Asservissement::setTargetLookAtForward(double x, double y, Rotation rotation) {
    setProtectedTargetAngulaire(calculAngle(x,y,pos),rotation);
}

void Asservissement::setTargetLookAtBackward(double x, double y, Rotation rotation) {
    setProtectedTargetAngulaire(mod_angle(calculAngle(x,y,pos)+180),rotation);
}

void Asservissement::setTargetStop(void){
    positionControlLineaire.stop();
    positionControlAngulaire.stop();
}

void Asservissement::setTarget(position_t position) {
    target = position;
}

double Asservissement::getAngularError(void){
    return positionControlAngulaire.getPostion();
}

double Asservissement::getLinearError(void){
    return positionControlLineaire.getPostion();
}

Rotation Asservissement::getRotationSide(void){
    if (positionControlAngulaire.getPostion()>0) {
        return Rotation::CLOCKWISE;
    }
    else if (positionControlAngulaire.getPostion()<0) {
        return Rotation::ANTICLOCKWISE;
    }
    else {
        return Rotation::NONE;
    }
}

Direction Asservissement::getDirectionSide(void){
    if (positionControlLineaire.getPostion()>0) {
        return Direction::FORWARD;
    }
    else if (positionControlLineaire.getPostion()<0) {
        return Direction::BACKWARD;
    }
    else {
        return Direction::NONE;
    }
}

void Asservissement::reset(void){
    currentState = Rotation::SHORTEST;
    target = pos;
    positionControlLineaire.reset(0);
    positionControlAngulaire.reset(0);
    pidAngulaire.reset();
    pidAngulaireBlock.reset();
    pidLineaireBlock.reset();
    pidLineaire.reset();
    nextTime =  get_uptime_ms();
    motorA.SetSpeedSigned(0);
    motorB.SetSpeedSigned(0);
    motorC.SetSpeedSigned(0);
}

//******************************************************
//Recupération de l'erreur Réel
//******************************************************

double Asservissement::getAngularErrorReel(void){
    double angleErreur = mod_angle(target.a-pos.a);

    if (angleErreur>0 && currentState == Rotation::CLOCKWISE) {
        angleErreur -= 360;
    }
    else if (angleErreur<0 && currentState == Rotation::ANTICLOCKWISE) {
        angleErreur += 360;
    }
    if (angleErreur<90 && angleErreur>-90 && currentState != Rotation::SHORTEST ) {
        currentState = Rotation::SHORTEST;
    }
    return angleErreur;
}

double Asservissement::getLinearErrorReel(void){
    //Première partie, connaitre l'angle formé entre le robot et le point de la target
    double erreurAngulaireSurPoint = calculAngle(target.x,target.y,pos);

    // Calculer la distance entre le robot et la droite orthogonal qui passe par le point de la target
    double distanceRobotPoint = sqrt(pow((target.x - pos.x),2)+pow((target.y -pos.y),2));
    return (distanceRobotPoint*cos((erreurAngulaireSurPoint-pos.a)*DEG_TO_RAD));
}



//******************************************************
//Robot Status
//******************************************************

bool Asservissement::robotMoving(void){
    return robotTurning() || robotRunning();
}
bool Asservissement::robotTurning(void){
    return positionControlAngulaire.getPostion()!=0 || (getAngularErrorReel()>3 || getAngularErrorReel()<-3);
}
bool Asservissement::robotRunning(void){
    return positionControlLineaire.getPostion()!=0 || (getLinearError()>5 || getLinearError()<-5);
}
int Asservissement::getBrakingDistance(void){
    return -positionControlLineaire.getBrakingDistance();
}

position_t Asservissement::getCurrentTarget(void){
    return target;
};
