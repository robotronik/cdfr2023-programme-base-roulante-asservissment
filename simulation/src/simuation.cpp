#include "simulation.h"

std::vector<FunctionContainer> functionVector;

gpointer simulationProcess(gpointer data){
    sleep(4);
    //defaulttest();
    for (const auto& func : functionVector) {
        std::cout << "Calling function: " << func.getName() << std::endl;
        func.call();
    }
    return NULL;
}