#include "odometrieSim.h"
#include "hardware_interface.h"
#include "motor.h"

#define MAXSPEED    500
#define SEUIL       1*COEFMULT


odometrieSim::odometrieSim(/* args */){
    threadOdoLeft = g_thread_new("odoLeftThread", threadFuncOdometrieLeft, this);
    threadOdoRight = g_thread_new("odoRightThread", threadFuncOdometrieRight, this);
}

void odometrieSim::setLeftSpeed(int speed){
    valueLeft = speed;
}
void odometrieSim::setRightSpeed(int speed){
    valueRight = speed;
}
void odometrieSim::setLeftDirection(bool directionForward){
    moveForwardLeft = directionForward;
}
void odometrieSim::setRightDirection(bool directionForward){
    moveForwardRight = directionForward;
}

double odometrieSim::map(double x, double in_min, double in_max, double out_min, double out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void odometrieSim::stopThread(void){
    stop_thread = true;
}

odometrieSim::~odometrieSim(){

}


gpointer odometrieSim::threadFuncOdometrieLeft(gpointer data) {
    odometrieSim* odometrie = (odometrieSim*)data;
    while (!odometrie->stop_thread) {
        int frequency = map(odometrie->valueLeft,0,100*COEFMULT,0,MAXSPEED);
        if(frequency!=0){            
            int wait_time = 1000000 / frequency;
            if(wait_time>500000){
                wait_time = 500000;
            }
            g_usleep(wait_time);
            set_gpio_get(port_odometrie2L,pin_odometrie2L,odometrie->moveForwardRight);
            exti4_isr();
        }
    }
    return NULL;
}

gpointer odometrieSim::threadFuncOdometrieRight(gpointer data) {
    odometrieSim* odometrie = (odometrieSim*)data;
    while (!odometrie->stop_thread) {
        int frequency = map(odometrie->valueRight,0,100*COEFMULT,0,MAXSPEED);
        if(frequency!=0){
            int wait_time = 1000000 / frequency;
            if(wait_time>500000){
                wait_time = 500000;
            }
            g_usleep(wait_time);
            set_gpio_get(port_odometrie2R,pin_odometrie2R,!odometrie->moveForwardLeft);
            exti2_isr();
        }          
    }
    return NULL;
}


        