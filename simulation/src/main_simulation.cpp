#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <chrono>
#include <sys/ioctl.h>
#include "simulation.h"

void loop_sys_tick(void){
    while (1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        sys_tick_handler();
    }    
}

void printMessage(const std::string& message) {
    std::cout << message << std::endl;
}

int main() {
    std::thread t1(stm_main);
    std::thread t2(loop_sys_tick);

    t1.join(); // Attend la fin du thread t1
    t2.join(); // Attend la fin du thread t2

    return 0;
}
