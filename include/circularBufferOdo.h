#pragma once
#include "uart.h"
#include <cstdint>

#define OPTIMIZE_BUFFER

#define NB_RECORD_SECTION 8

class CircularBufferOdo {
private:
    volatile uint8_t* m_buffer;
    volatile int m_startBuffer;
    volatile int m_startbitCount;
    long int m_endBuffer;
    long int m_endbitCount;
    long int m_size;
    bool m_freezePush;

    long int m_endPopRecord;
    long int m_endbitPopRecord;
    int m_record_section;
    bool m_validRedord;
    long int m_endbitSection[NB_RECORD_SECTION];
    long int m_endSection[NB_RECORD_SECTION];

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
    void addSection(void);
    void stopRecording(void);
    bool recordIsValid(void);
    bool popRecod(uint8_t &data);
    uint8_t popRecod(void);
    bool recordIsEmpty() const;
    long int getEndPointSection(int section);
    int getNumberSetion();
    long int getSartPointSection(int section);
    void resetPopRecord(void);

};