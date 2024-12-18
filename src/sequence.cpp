#include "sequence.h"

// Fonction qui prend en entrée un pointeur de fonction


sequence::sequence(/* args */){
    startTime = get_uptime_ms();
}

void sequence::start(void){
    Time = startTime;
    count = 0;
}

void sequence::reset(void){
    startTime = get_uptime_ms();
}

void sequence::delay(FunctionPointer func, uint32_t delay) {
    Time += delay;
    if(Time < get_uptime_ms()){
        if(avancement <= count){
            avancement++;
            func();
        }
    }
    count ++;
}

void sequence::interval(FunctionPointer func, uint32_t delay) {
    if(startTime < get_uptime_ms()){
        startTime = get_uptime_ms() + delay;
        func();
    }
}

sequence::~sequence(){

}