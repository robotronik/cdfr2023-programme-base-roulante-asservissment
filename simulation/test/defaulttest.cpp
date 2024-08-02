#include "simulation.h"


void printPos (void){
    int16_t x;
    int16_t y;
    int16_t theta;
    get_coordinates(x,y,theta);
    printf("%d %d %d\n",x,y,theta);
}

FUNCTIONSIM(defaultTest){
    printPos();
    set_consigne_lineaire(-500,0);
    sleep(5);
    set_consigne_angulaire(90,0);
    sleep(1);
    printPos();
}
