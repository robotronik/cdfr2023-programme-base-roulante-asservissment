#include "simulation.h"

gpointer simulationProcess(gpointer data){
    sleep(4);
    simI2c();
    return NULL;
}