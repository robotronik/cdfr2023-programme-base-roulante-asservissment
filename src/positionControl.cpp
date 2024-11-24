#include "../include/positionControl.h"


positionControl::positionControl(double initialValue){
    this->reset(initialValue);
}

void positionControl::reset(double initialValue){
    consigne = initialValue;
    position = initialValue;
    vitesse = 0.0;
    stopStatus = false;
}

void positionControl::stop(void){
    stopStatus = true;
}

void positionControl::setPosition(double initialValue){
    position = initialValue;
}

void positionControl::setConsigne(double setConsigne, int timems){
    consigne = setConsigne;
    PreviousTime = timems;
    stopStatus = false;
}
double positionControl::getPostion(int timems){
    deltaTemps = ((double)(timems-PreviousTime)/1000);
    if((position<consigne && (position + vitesse*deltaTemps)>consigne) || (position>consigne && (position + vitesse*deltaTemps)<consigne) ){
        position = consigne;
        vitesse = 0;
    }
    else{
        position = position + vitesse*deltaTemps;
        PreviousTime = timems;
    }

    calculVitesse();
    return position;
}

void positionControl::setConsigne(double setConsigne){
    consigne = setConsigne;
    //PreviousTime = get_uptime_ms();
    stopStatus = false;
}

double positionControl::getPostion(){
    deltaTemps = ((double)(get_uptime_ms()-PreviousTime)/1000);
    if((position<consigne && (position + vitesse*deltaTemps)>consigne) || (position>consigne && (position + vitesse*deltaTemps)<consigne) ){
        position = consigne;
        vitesse = 0;
    }
    else{
        position = position + vitesse*deltaTemps;
    }
    PreviousTime = get_uptime_ms();

    calculVitesse();
    return position;
}

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
                vitesse = sqrt(4*(consigne-position)*(decelerationMaxAv/2));
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
                vitesse = -sqrt(4*(consigne-position)*(-decelerationMaxAr/2));
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
        vitesse = 0;
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

