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


#define SEQUENCE_BEGIN() static int _seq_state = 0; switch(_seq_state) { case 0:

#define DELAY(ms) { \
        static uint32_t _last = 0; \
        _last = get_uptime_ms(); \
        _seq_state = __LINE__; case __LINE__: \
        if (get_uptime_ms() - _last < (ms)) return EXIT_TASKNOTFINISH; \
    }

#define WAIT_UNTIL(cond) do { \
        _seq_state = __LINE__; case __LINE__: \
        if (!(cond)) return EXIT_TASKNOTFINISH; \
    } while(0)

#define SEQUENCE_END() }

