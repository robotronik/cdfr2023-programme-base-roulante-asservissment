#pragma once
#include "uart.h"
#include <cstdint>

#define OPTIMIZE_BUFFER

class CircularBuffer {
private:
    uint8_t* m_buffer;
    int m_startBuffer;
    int m_startbitCount;
    int m_endBuffer;
    int m_endbitCount;
    int m_size;

public:
    CircularBuffer(int size,uint8_t* buffer);
    void init(void);
    bool isFull() const;
    bool isEmpty() const;
    bool push(uint8_t data);
    bool pop(uint8_t &data);
    uint8_t pop(void);

};