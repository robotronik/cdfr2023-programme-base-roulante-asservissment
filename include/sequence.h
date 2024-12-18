#pragma once

#include "clock.h"

typedef void (*FunctionPointer)();

class sequence
{
private:
    /* data */
    uint32_t startTime = 0;
    uint32_t Time = 0;
    int avancement = 0;
    int count = 0;

public:
    sequence(/* args */);
    void start(void);
    void reset(void);
    void delay(FunctionPointer func, uint32_t delay);
    void interval(FunctionPointer func, uint32_t delay);
    ~sequence();
};

