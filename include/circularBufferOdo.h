#pragma once
#include "uart.h"
#include <cstdint>

#define OPTIMIZE_BUFFER

class CircularBufferOdo {
private:
    uint8_t* m_buffer;
    int m_startBuffer;
    int m_startbitCount;
    int m_endBuffer;
    int m_endbitCount;
    int m_size;
    bool m_freezePush;

    int m_endPopRecord;
    int m_endbitPopRecord;
    bool m_validRedord;

public:
    CircularBufferOdo(int size,uint8_t* buffer);
    void init(void);
    bool isFull() const;
    bool isEmpty() const;
    bool push(uint8_t data);
    bool pop(uint8_t &data);
    uint8_t pop(void);
    void freezePush(bool);

    void startRecording(void);
    void stopRecording(void);
    bool recordIsValid(void);
    bool popRecod(uint8_t &data);
    uint8_t popRecod(void);
    bool recordIsEmpty() const;
    void resetPopRecord(void);

};