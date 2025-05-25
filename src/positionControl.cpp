#include "../include/positionControl.h"

#ifdef SIMULATION_POSITION_CONTROL
    uint32_t TIME_;

    void SET_TIME_(uint32_t time){
        TIME_ = time;
    }

    uint32_t get_uptime_ms(){
        return TIME_;
    }
#endif


positionControl::positionControl(double initialValue):
    vitesseMaxAv(0,1000),
    accelerationMaxAv(0,1000),
    decelerationMaxAv(0,1000),
    vitesseMaxAr(0,1000),
    accelerationMaxAr(0,1000),
    decelerationMaxAr(0,1000),
    decelerationStopAv(0,1000),
    decelerationStopAr(0,1000)
{
    this->reset(initialValue);
    vitesseMaxAv.setOnChangeCallback        ([this](auto value) {(void)value; this->computeStroke();});
    accelerationMaxAv.setOnChangeCallback   ([this](auto value) {(void)value; this->computeStroke();});
    decelerationMaxAv.setOnChangeCallback   ([this](auto value) {(void)value; this->computeStroke();});
    vitesseMaxAr.setOnChangeCallback        ([this](auto value) {(void)value; this->computeStroke();});
    accelerationMaxAr.setOnChangeCallback   ([this](auto value) {(void)value; this->computeStroke();});
    decelerationMaxAr.setOnChangeCallback   ([this](auto value) {(void)value; this->computeStroke();});
    decelerationStopAv.setOnChangeCallback  ([this](auto value) {(void)value; this->computeStroke();});
    decelerationStopAr.setOnChangeCallback  ([this](auto value) {(void)value; this->computeStroke();});
}

positionControl::~positionControl(){

}

void positionControl::reset(double initialValue){
    consigne = initialValue;
    position = initialValue;
    vitesse = 0.0;
    t_accel = 0;
    t_cruise = 0;
    t_decel = 0;
    d_accel = 0;
    d_cruise = 0;
    d_decel = 0;
    startTimeMs = 0;
    startPosition = 0;
    maxSpeedOut = 0;
    startSpeed = 0;
}

//******************************************************
// commande
//******************************************************
void positionControl::stop(void){
    maxSpeedOut = 0;
    consigne = position+getBrakingDistance();
    computeStroke(true);
}

void positionControl::setPosition(double initialValue){
    usartprintf("setPostion : %lf\n",initialValue);
    position = initialValue;
    computeStroke();
}

void positionControl::setConsigne(double setConsigne){
    consigne = setConsigne;
    computeStroke();
}

void positionControl::setMaxSpeedOut(double max){
    maxSpeedOut = max;
    computeStroke();
}


void positionControl::computeStroke(bool forceStop) {
    decelerationAv = forceStop ? decelerationStopAv : decelerationMaxAv;
    decelerationAr = forceStop ? decelerationStopAr : decelerationMaxAr;
    double time = (double)(get_uptime_ms()-startTimeMs)/1000;
    //genstion du mouvement Avant
    if(consigne != position){
        if(consigne-position>0){
            computeDT(abs(consigne - position),vitesseMaxAv,accelerationMaxAv,decelerationAv);
        }
        //genstion du mouvement Arrière
        else if(consigne-position<0){
            computeDT(abs(consigne - position),vitesseMaxAr,accelerationMaxAr,decelerationAr);
        }
        startSpeed = vitesse;
        startTimeMs = get_uptime_ms();
        startPosition = position;
    }
    //Prevent stop when the robot not start to move
    else if(time < t_accel + t_cruise + t_decel){
        t_accel = 0;
        t_cruise = 0;
        t_decel = 0;
    }
}

void positionControl::computeDT(double distance,double vitesseMax, double acceleration, double deceleration){
    double vitesseInitial = vitesse;
    double vitesseFinal = getMaxSpeedOut();
    double maxSpeed;

#ifndef SIMULATION_POSITION_CONTROL
    if(vitesseInitial>vitesseMax){
        usartprintf("Error : initial speed greater than maximum speed \n");
        vitesseMax = vitesseInitial; //to avoid bigger problems
    }
#endif

    t_accel = (vitesseMax - vitesseInitial) / acceleration;
    d_accel = vitesseInitial * t_accel + 0.5 * acceleration * t_accel * t_accel;

    t_decel = (vitesseMax - vitesseFinal) / deceleration;
    d_decel = vitesseFinal * t_decel + 0.5 * deceleration * t_decel * t_decel;

    d_cruise = distance - (d_accel + d_decel);
    t_cruise = d_cruise / vitesseMax;

    if (d_cruise < 0) {
        maxSpeed = sqrt((2*distance + (vitesseInitial*vitesseInitial)/acceleration + (vitesseFinal*vitesseFinal)/deceleration) / (1/acceleration + 1/deceleration));

        t_accel = (maxSpeed - vitesseInitial) / acceleration;
        t_decel = (maxSpeed - vitesseFinal) / deceleration;

        if(t_decel < 0){
            t_decel = 0;
            maxSpeed = sqrt(vitesseInitial * vitesseInitial + 2 * acceleration * distance);
            t_accel = (maxSpeed - vitesseInitial) / acceleration;
        }
        t_cruise = 0;
        d_accel = 0.5 * acceleration * t_accel * t_accel + t_accel * vitesseInitial;
        d_decel = 0.5 * deceleration * t_decel * t_decel + t_decel * vitesseFinal;
        d_cruise = 0;
    }
}


double positionControl::getPostion(){
    updatePosition();
    updateSpeed();
    return position;
}

void positionControl::updatePosition(){
    double time = (double)(get_uptime_ms()-startTimeMs)/1000;
    //genstion du mouvement Avant
    if(consigne-position>0){
        if (time < t_accel) {
            double distance = startSpeed * time + 0.5 * accelerationMaxAv * time * time;
            position = startPosition + distance;
        }
        else if (time < (t_accel + t_cruise)) {
            double distance = d_accel + vitesseMaxAv * (time - t_accel);
            position = startPosition + distance;
        }
        else if (time < t_accel + t_cruise + t_decel) {
            double t_d = (t_accel + t_cruise + t_decel) - time;
            double distance = getMaxSpeedOut() * t_d + 0.5 * decelerationAv * t_d * t_d;
            position = consigne - distance;
        }
        else {
            position = consigne;
        }
    }
    //genstion du mouvement Arrière
    else{
        if (time < t_accel) {
            double distance = startSpeed * time + 0.5 * accelerationMaxAr * time * time;
            position = startPosition - distance;
        }
        else if (time < (t_accel + t_cruise)) {
            double distance = d_accel + vitesseMaxAr * (time - t_accel);
            position = startPosition - distance;
        }
        else if (time < t_accel + t_cruise + t_decel) {
            double t_d = (t_accel + t_cruise + t_decel) - time;
            double distance = getMaxSpeedOut() * t_d + 0.5 * decelerationAr * t_d * t_d;
            position = consigne + distance;
        }
        else {
            position = consigne;
        }
    }
}

void positionControl::updateSpeed(){
    double time = (double)(get_uptime_ms()-startTimeMs)/1000;
    //genstion du mouvement Avant
    if(consigne-position>0){
        if (time < t_accel) {
            vitesse = accelerationMaxAv * time + startSpeed;
        }
        else if (time < (t_accel + t_cruise)) {
            vitesse = vitesseMaxAv;
        }
        else if (time < t_accel + t_cruise + t_decel) {
            double t_d = (t_accel + t_cruise + t_decel) - time;
            vitesse = getMaxSpeedOut() + decelerationAv * t_d;
        }
    }
    //genstion du mouvement Arrière
    else{
        if (time < t_accel) {
            vitesse = accelerationMaxAr * time + startSpeed;
        }
        else if (time < (t_accel + t_cruise)) {
            vitesse = vitesseMaxAr;
        }
        else if (time < t_accel + t_cruise + t_decel) {
            double t_d = (t_accel + t_cruise + t_decel) - time;
            vitesse = getMaxSpeedOut() + decelerationAr * t_d;
        }
    }
}

double positionControl::getMaxSpeedOut(){
    double localmaxSpeedOut = maxSpeedOut;
    if(consigne-position>0){
        if(maxSpeedOut > vitesseMaxAv){
            localmaxSpeedOut = vitesseMaxAv;
        }
    }
    //genstion du mouvement Arrière
    else if(consigne-position<0){
        if(maxSpeedOut > vitesseMaxAr){
            localmaxSpeedOut = vitesseMaxAr;
        }
    }
    return localmaxSpeedOut;
}


bool positionControl::getMove(){
    return (consigne != position);
}

int positionControl::getBrakingDistance(){
    if(consigne-position>0){
        return (vitesse*vitesse)/(decelerationStopAv);
    }
    else{
        return -(vitesse*vitesse)/(decelerationStopAr);
    }
}

