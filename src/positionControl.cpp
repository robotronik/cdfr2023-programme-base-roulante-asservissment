#include "../include/positionControl.h"


positionControl::positionControl(double initialValue){
    this->reset(initialValue);
}

void positionControl::reset(double initialValue){
    consigne = initialValue;
    position = initialValue;
    vitesse = 0.0;
    PreviousTime = 0;
    stopStatus = false;
}

void positionControl::stop(void){
    stopStatus = true;
}

void positionControl::setPosition(double initialValue){
    position = initialValue;
}


double positionControl::getPostion(uint32_t timems){
    deltaTemps = ((double)(timems-PreviousTime)/1000);
    if((position<consigne && (position + vitesse*deltaTemps)>consigne) || (position>consigne && (position + vitesse*deltaTemps)<consigne) ){
        position = consigne;
        vitesse = maxSpeedOut;
    }
    else{
        position = position + vitesse*deltaTemps;
    }
    PreviousTime = timems;

    calculVitesse();
    return position;
}

void positionControl::setConsigne(double setConsigne){
    consigne = setConsigne;
    stopStatus = false;
}

void positionControl::setMaxSpeedOut(double max){
    maxSpeedOut = max;
}

#ifndef SIMULATION_POSITION_CONTROL
double positionControl::getPostion(){
    return getPostion(get_uptime_ms());
}
#endif

void positionControl::calculVitesse(){
    double vitessePrecedente = vitesse;

    //genstion du mouvement Avant
    if(stopStatus){
        vitesse = 0;
        if(decelerationStop != -1 && vitesse > vitessePrecedente + decelerationStop*deltaTemps){
            vitesse = vitessePrecedente + decelerationStop*deltaTemps;
        }
         if(decelerationStop != -1 && vitesse < vitessePrecedente - decelerationStop*deltaTemps){
            vitesse = vitessePrecedente - decelerationStop*deltaTemps;
        }
        return;
    }

    if(consigne-position>0){
        if(decelerationMaxAv!=-1){
            if(decelationLineair){
                vitesse = sqrt(4*(consigne-position)*(decelerationMaxAv/2)) + maxSpeedOut;
            }
            else{
                vitesse = (consigne-position);
            }
        }
        else{
            vitesse = consigne-position;
        }
        if(accelerationMaxAv != -1 && vitesse > vitessePrecedente + accelerationMaxAv*deltaTemps){
            vitesse = vitessePrecedente + accelerationMaxAv*deltaTemps;
        }
    }
    //genstion du mouvement Arrière
    else if(consigne-position<0){
        if(decelerationMaxAr != -1){
            if(decelationLineair){
                vitesse = -sqrt(4*(consigne-position)*(-decelerationMaxAr/2)) - maxSpeedOut;
            }
            else{
                vitesse = (consigne-position);
            }
        }
        else{
            vitesse = consigne-position;
        }
        if(accelerationMaxAr != -1 && vitesse < vitessePrecedente - accelerationMaxAr*deltaTemps){
            vitesse = vitessePrecedente - accelerationMaxAr*deltaTemps;
        }
    }
    //gestion vitesse 0
    else{
        vitesse = maxSpeedOut;
    }


    //Ecrétatge des vitesses Max
    if(vitesseMaxAv != -1 and vitesse > vitesseMaxAv){
        vitesse = vitesseMaxAv;
    }
    else if(vitesseMaxAr != -1 and vitesse < -vitesseMaxAr){
        vitesse = -vitesseMaxAr;
    }
}

bool positionControl::getMove(){
    return (consigne != position);
}

int positionControl::getBrakingDistance(){
    if(consigne-position>0){
        return (vitesse*vitesse)/(decelerationMaxAv);
    }
    else{
        return -(vitesse*vitesse)/(decelerationMaxAr);
    }
}

bool positionControl::getStatus(void){
    return stopStatus;
}

positionControl::~positionControl(){

}

