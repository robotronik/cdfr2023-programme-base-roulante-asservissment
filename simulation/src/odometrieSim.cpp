#include "odometrieSim.h"


odometrieSim::odometrieSim(/* args */){
    threadOdoLeft = g_thread_new("odoLeftThread", threadFuncOdometrieLeft, this);
    threadOdoRight = g_thread_new("odoRightThread", threadFuncOdometrieRight, this);
}

void odometrieSim::setLeftSpeed(int speed){
    valueLeft = speed;
    int wait_time = map(valueLeft,10,100*COEFMULT,500000,500);
    printf("%d\n",wait_time);
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
    printf("START\n");
    g_usleep(5000000);
    while (!odometrie->stop_thread) {
        if(odometrie->valueLeft<SEUIL){
            printf("START3\n");
            g_usleep(7000);
        }
        else{
            int wait_time = map(odometrie->valueLeft,10,100*COEFMULT,500000,500);
            g_usleep(500);
            set_gpio_get(port_odometrie2R,pin_odometrie2R,odometrie->moveForwardLeft);
            exti2_isr();
        }
    }
    printf("FIN\n");
    return NULL;
}

gpointer odometrieSim::threadFuncOdometrieRight(gpointer data) {
    odometrieSim* odometrie = (odometrieSim*)data;
    while (!odometrie->stop_thread) {
        if(odometrie->valueRight<SEUIL){
            g_usleep(7000);
        }
        else{
            int wait_time = map(odometrie->valueRight,10,100*COEFMULT,500000,500);
            g_usleep(wait_time);
            set_gpio_get(port_odometrie2L,pin_odometrie2L,odometrie->moveForwardRight);
            exti4_isr();
        }        
    }
    return NULL;
}


        