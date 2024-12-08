#include "Asservissement.h"


Asservissement::Asservissement(position* pos):
    pidLineaire(1,0.000,100),
    pidAngulaire(2,0.0,200),
    pidLineaireBlock(1,0.001,100),
    pidAngulaireBlock(2,0.001,200)
{
    posRobot = pos;

    nextTime =  get_uptime_ms();

    currentState = Rotation::SHORTEST;

    //Réglage
    positionControlLineaire.vitesseMaxAv =  10000;
    positionControlLineaire.accelerationMaxAv = 300;//acceleration max AR
    positionControlLineaire.decelerationMaxAv = 300; //decelaration max AR
    positionControlLineaire.vitesseMaxAr = 10000;
    positionControlLineaire.accelerationMaxAr = 500; //acceleration max AV
    positionControlLineaire.decelerationMaxAr = 500; //decelaration max AV
    positionControlLineaire.decelationLineair = true;
    positionControlLineaire.decelerationStop = 2000;

    //Réglage
    positionControlAngulaire.vitesseMaxAv = 360;
    positionControlAngulaire.accelerationMaxAv = 300;
    positionControlAngulaire.decelerationMaxAv = 300;
    positionControlAngulaire.vitesseMaxAr = 360;
    positionControlAngulaire.accelerationMaxAr = 300;
    positionControlAngulaire.decelerationMaxAr = 300;
    positionControlAngulaire.decelationLineair = true;
    positionControlLineaire.decelerationStop = 600;
}

Asservissement::~Asservissement()
{
}

void Asservissement::setAsservissementLoopPeriod(int period){
    loopPeriod = period;
}

void Asservissement::loop(){
    if(nextTime < get_uptime_ms()){
        if(posRobot->getPositionChanged()){
            setConsigne(posRobot->getPosition());
        }
        nextTime = get_uptime_ms() + loopPeriod;
        asservissementLoop();
	}
}


void Asservissement::asservissementLoop(){
    double test = calculAngle(consigne.x,consigne.y,posRobot->getPosition());
    bool reTargetAngle = false;
    if(getLinearErrorReel() >= 100){
        reTargetAngle = true;
        setConsigneAngulaire(test,Rotation::SHORTEST);
    }
    else if(getLinearErrorReel() <= -100){
        reTargetAngle = true;
        setConsigneAngulaire(test+180,Rotation::SHORTEST);
    }

    double valPidLineaire;
    double valPidAngulaire;
    if(positionControlLineaire.getPostion()==0){
        valPidLineaire = pidLineaireBlock.update(getLinearErrorReel()-positionControlLineaire.getPostion(),posRobot->getPosition_Time());
        pidLineaire.reset();
    }
    else{
        valPidLineaire = pidLineaire.update(getLinearErrorReel()-positionControlLineaire.getPostion(),posRobot->getPosition_Time());
        pidLineaireBlock.reset();
    }

    if(positionControlAngulaire.getPostion()==0 || reTargetAngle){
        valPidAngulaire = pidAngulaireBlock.update(getAngularErrorReel()-positionControlAngulaire.getPostion(),posRobot->getPosition_Time());
        pidAngulaire.reset();
    }
    else{
        valPidAngulaire = pidAngulaire.update(getAngularErrorReel()-positionControlAngulaire.getPostion(),posRobot->getPosition_Time());
        pidAngulaireBlock.reset();
    }

    motorSpeedSignedL((int)(valPidLineaire-valPidAngulaire));
	motorSpeedSignedR((int)(valPidLineaire+valPidAngulaire));
}


//******************************************************
//Set consigne
//******************************************************

void Asservissement::setProtectedConsigneAngulaire(double angle, Rotation rotation){
    if(positionControlLineaire.getPostion()!=0){
        consigne.x = posRobot->getPosition_X();
        consigne.y = posRobot->getPosition_Y();
        positionControlLineaire.setPosition(0);
        positionControlLineaire.setConsigne(0);
    }
    setConsigneAngulaire(angle,rotation);
}

void Asservissement::setConsigneAngulaire(double angle, Rotation rotation){
    double errorBefor = getAngularErrorReel()-positionControlAngulaire.getPostion();
    currentState = rotation;
    consigne.teta = mod_angle(angle);
    positionControlAngulaire.setPosition(getAngularErrorReel() - errorBefor);
    positionControlAngulaire.setConsigne(0);
}

void Asservissement::setProtectedConsigneLineaire(double x, double y){
    if(positionControlAngulaire.getPostion()!=0){
        consigne.teta = mod_angle(posRobot->getPosition_Teta());
        positionControlAngulaire.setPosition(0);
        positionControlAngulaire.setConsigne(0);
    }
    setConsigneLineaire(x,y);
}

void Asservissement::setConsigneLineaire(double x, double y){
    double errorBefor = getLinearErrorReel()-positionControlLineaire.getPostion();
    consigne.x = x;
    consigne.y = y;
    positionControlLineaire.setPosition(getLinearErrorReel() - errorBefor);
    positionControlLineaire.setConsigne(0);
}

void Asservissement::setConsigneLookAtForward(double x, double y, Rotation rotation){
    setProtectedConsigneAngulaire(calculAngle(x,y,posRobot->getPosition()),rotation);
}

void Asservissement::setConsigneLookAtBackward(double x, double y, Rotation rotation){
    setProtectedConsigneAngulaire(mod_angle(calculAngle(x,y,posRobot->getPosition())+180),rotation);
}

void Asservissement::setConsigneStop(void){
    positionControlLineaire.stop();
    positionControlAngulaire.stop();
}

void Asservissement::setConsigne(position_t position){
    consigne = position;
}

double Asservissement::getAngularError(void){
    return positionControlAngulaire.getPostion();
}

double Asservissement::getLinearError(void){
    return positionControlLineaire.getPostion();
}

Rotation Asservissement::getRotationSide(void){
    if(positionControlAngulaire.getPostion()>0){
        return Rotation::CLOCKWISE;
    }
    else if(positionControlAngulaire.getPostion()<0){
        return Rotation::ANTICLOCKWISE;
    }
    else{
        return Rotation::NONE;
    }
}

Direction Asservissement::getDirectionSide(void){
    if(positionControlLineaire.getPostion()>0){
        return Direction::FORWARD;
    }
    else if(positionControlLineaire.getPostion()<0){
        return Direction::BACKWARD;
    }
    else{
        return Direction::NONE;
    }
}

void Asservissement::reset(void){
    currentState = Rotation::SHORTEST;
    consigne.teta = posRobot->getPosition_Teta();
    consigne.x = posRobot->getPosition_X();
    consigne.y = posRobot->getPosition_Y();
    positionControlLineaire.reset(0);
    positionControlAngulaire.reset(0);
    pidAngulaire.reset();
    pidAngulaireBlock.reset();
    pidLineaireBlock.reset();
    pidLineaire.reset();
    nextTime =  get_uptime_ms();
    motorSpeedSignedL(0);
    motorSpeedSignedR(0);
}

//******************************************************
//Recupération de l'erreur Réel
//******************************************************

double Asservissement::getAngularErrorReel(void){
    double angleErreur = mod_angle(consigne.teta-posRobot->getPosition_Teta());

    if(angleErreur>0 && currentState == Rotation::CLOCKWISE){
        angleErreur -= 360;
    }
    else if(angleErreur<0 && currentState == Rotation::ANTICLOCKWISE){
        angleErreur += 360;
    }
    if(angleErreur<90 && angleErreur>-90 && currentState != Rotation::SHORTEST ){
        currentState = Rotation::SHORTEST;
    }
    return angleErreur;
}

double Asservissement::getLinearErrorReel(void){
    //Première partie, connaitre l'angle formé entre le robot et le point de la consigne
    double erreurAngulaireSurPoint = calculAngle(consigne.x,consigne.y,posRobot->getPosition());

    // Calculer la distance entre le robot et la droite orthogonal qui passe par le point de la consigne
    double distanceRobotPoint = sqrt(pow((consigne.x - posRobot->getPosition_X()),2)+pow((consigne.y -posRobot->getPosition_Y()),2));
    return (distanceRobotPoint*cos((erreurAngulaireSurPoint-posRobot->getPosition_Teta())*DEG_TO_RAD));
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

position_t Asservissement::getCurrentConsigne(void){
    return consigne;
};
