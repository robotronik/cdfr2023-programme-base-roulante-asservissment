#include "Asservissement.h"


Asservissement::Asservissement(position* pos):
    pidLineaire(2,0.000,200),
    pidAngulaire(4,0.0,400),
    pidLineaireBlock(1,0.001,100),
    pidAngulaireBlock(2,0.001,200)
{
    posRobot = pos;
    nextTime =  get_uptime_ms();
    currentState = Rotation::SHORTEST;

    positionControlLineaire.vitesseMaxAv.setRange(0,700);
    positionControlLineaire.accelerationMaxAv.setRange(0,600);
    positionControlLineaire.decelerationMaxAv.setRange(0,600);
    positionControlLineaire.vitesseMaxAr.setRange(0,700);
    positionControlLineaire.accelerationMaxAr.setRange(0,600);
    positionControlLineaire.decelerationMaxAr.setRange(0,600);
    positionControlLineaire.decelerationStopAv.setRange(0,1800);
    positionControlLineaire.decelerationStopAr.setRange(0,1800);

    positionControlAngulaire.vitesseMaxAv.setRange(0,200);
    positionControlAngulaire.accelerationMaxAv.setRange(0,200);
    positionControlAngulaire.decelerationMaxAv.setRange(0,200);
    positionControlAngulaire.vitesseMaxAr.setRange(0,200);
    positionControlAngulaire.accelerationMaxAr.setRange(0,200);
    positionControlAngulaire.decelerationMaxAr.setRange(0,200);
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

Asservissement::~Asservissement()
{
}

void Asservissement::setAsservissementLoopPeriod(int period){
    loopPeriod = period;
}

void Asservissement::loop(){
    if(nextTime < get_uptime_ms()){
        nextTime = get_uptime_ms() + loopPeriod;
        asservissementLoop();
    }
}


void Asservissement::asservissementLoop(){
    double targetAngle = calculAngle(consigne.x,consigne.y,posRobot->getPosition());
    bool reTargetAngle = false;
    double valPidLineaire;
    double valPidAngulaire;
    double realErrorAngular;
    double reduceErrorAngular;
    double realErrorLinear;
    double reduceErrorLinear;
    uint32_t timeLastPos = posRobot->getPosition_Time();
    positionControlLineaire.updatePostion();
    positionControlAngulaire.updatePostion();

    realErrorLinear = getLinearErrorReel();
    reduceErrorLinear = realErrorLinear-positionControlLineaire.getPostion();
    // usartprintf(">consignLin:%lf\n",positionControlLineaire.getPostion());
    // usartprintf(">errorLin:%lf\n",realErrorLinear);

    if(realErrorLinear >= 200){
        reTargetAngle = true;
        setConsigneAngulaire(targetAngle,Rotation::SHORTEST);
    }
    else if(realErrorLinear <= -200){
        reTargetAngle = true;
        setConsigneAngulaire(targetAngle+180,Rotation::SHORTEST);
    }

    realErrorAngular = getAngularErrorReel();
    reduceErrorAngular = realErrorAngular-positionControlAngulaire.getPostion();
    //usartprintf(">consignTheta:%lf\n",positionControlAngulaire.getPostion());
    //usartprintf(">errorTheta:%lf\n",realErrorAngular);

#ifdef ENABLE_STATISTIC
    statisticLinear.update(reduceErrorLinear,timeLastPos);
    statisticAngular.update(reduceErrorAngular,timeLastPos);
#endif

    //Calculate Linear commande
    if(positionControlLineaire.getPostion()==0){
        valPidLineaire = pidLineaireBlock.update(reduceErrorLinear,timeLastPos);
        pidLineaire.reset();
    }
    else{
        valPidLineaire = pidLineaire.update(reduceErrorLinear,timeLastPos);
        pidLineaireBlock.reset();
    }

    //Calculate Angular commande
    if(positionControlAngulaire.getPostion()==0 || reTargetAngle){
        valPidAngulaire = pidAngulaireBlock.update(reduceErrorAngular,timeLastPos);
        pidAngulaire.reset();
    }
    else{
        valPidAngulaire = pidAngulaire.update(reduceErrorAngular,timeLastPos);
        pidAngulaireBlock.reset();
    }

    //usartprintf(">speedL:%d\n>speedR:%d\n",(int)(valPidLineaire-valPidAngulaire),(int)(valPidLineaire+valPidAngulaire));
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

void Asservissement::setConsigneLookAt(double x, double y, Rotation rotation){
    double angleErreur = mod_angle(calculAngle(x,y,posRobot->getPosition())-posRobot->getPosition_Teta());
    if(angleErreur>0 && rotation == Rotation::CLOCKWISE){
        angleErreur -= 360;
    }
    else if(angleErreur<0 && rotation == Rotation::ANTICLOCKWISE){
        angleErreur += 360;
    }

    if(rotation == Rotation::SHORTEST)
        if(angleErreur<90 && angleErreur>-90){
            setProtectedConsigneAngulaire(calculAngle(x,y,posRobot->getPosition()),rotation);
        }
        else{
            setProtectedConsigneAngulaire(mod_angle(calculAngle(x,y,posRobot->getPosition())+180),rotation);
        }
    else{
        if(angleErreur<180 && angleErreur>-180){
            setProtectedConsigneAngulaire(calculAngle(x,y,posRobot->getPosition()),rotation);
        }
        else{
            setProtectedConsigneAngulaire(mod_angle(calculAngle(x,y,posRobot->getPosition())+180),rotation);
        }
    }
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
    statisticLinear.reset();
    statisticAngular.reset();
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

position_t Asservissement::getCurrentConsigne(void){
    return consigne;
};
