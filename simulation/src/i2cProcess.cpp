#include "i2cProcess.h"
#include <unistd.h>


void simI2c (void){
    // uint8_t data[1] = {52};
    // I2cSendData(52,data,1);
    printPos();
    //set_coordinates(-1000,20,50);
    //set_consigne_lineaire(-500,0);
    //set_consigne_lookAt_forward(-500,500,0);
    set_consigne_angulaire(90,0);
    sleep(1);
    printPos();
}

void printPos (void){
    int16_t x;
    int16_t y;
    int16_t theta;
    get_coordinates(x,y,theta);
    printf("%d %d %d\n",x,y,theta);
}