#include "circularBufferOdo.h"

CircularBufferOdo::CircularBufferOdo(int size,uint8_t* buffer){
    m_buffer = buffer;
    m_size = size;
    this->init();
}


void CircularBufferOdo::init(void){
    m_startBuffer = 0;
    m_startbitCount = 0;
    m_endBuffer = 0;
    m_endbitCount = 0;
    m_freezePush = false;
    m_validRedord = false;
}


//since the buffer is large and never full, there is no need to check the last 4 data with m_endbitCount and m_startbitCount
bool CircularBufferOdo::isFull() const {
    return ((m_startBuffer == m_endBuffer-1) || (m_startBuffer == 0 && m_endBuffer == m_size-1));
}


bool CircularBufferOdo::isEmpty() const {
#ifdef OPTIMIZE_BUFFER
    return (m_startBuffer == m_endBuffer) && (m_endbitCount == m_startbitCount);
#else
    return (m_startBuffer == m_endBuffer);
#endif
}


bool CircularBufferOdo::push(uint8_t data) {
    if(m_freezePush){
        return false;
    }

    //buffer is large, optimisation not needed
    if(m_startBuffer == m_size){
        m_validRedord = false;
    }

    if (data > 3) {
        usartprintf("Erreur: données hors limites (doivent être sur 2 bits).\n");
        return false;
    }

    if (isFull()) {
        usartprintf("Erreur: Buffer plein.\n");
        return false;
    }

#ifdef OPTIMIZE_BUFFER
    //erase and write the 2 bit
    m_buffer[m_startBuffer] = (m_buffer[m_startBuffer] & ~(3 << m_startbitCount)) | ((data & 3) << m_startbitCount);
    m_startbitCount += 2;

    if (m_startbitCount == 8) {
        m_startbitCount = 0;
        m_startBuffer++;
        if(m_startBuffer == m_size){
            m_startBuffer = 0;
        }
    }
#else
    m_buffer[m_startBuffer] = data;
    m_startBuffer++;
    if(m_startBuffer == m_size){
        m_startBuffer = 0;
    }
#endif

    return true;
}

uint8_t CircularBufferOdo::pop(void){
    uint8_t data;
    this->pop(data);
    return data;
}

bool CircularBufferOdo::pop(uint8_t &data) {
    if (isEmpty()) {
        usartprintf("Erreur: Buffer vide.\n");
        return false;
    }

#ifdef OPTIMIZE_BUFFER
    data = ((m_buffer[m_endBuffer]>>m_endbitCount) & 0x03);
    m_endbitCount += 2;

    if (m_endbitCount == 8) {
        m_endbitCount = 0;
        m_endBuffer ++;
        if(m_endBuffer == m_size){
            m_endBuffer = 0;
        }
    }
#else
    data = (m_buffer[m_endBuffer]);
    m_endBuffer ++;
    if(m_endBuffer == m_size){
        m_endBuffer = 0;
    }
#endif

    return true;
}

void CircularBufferOdo::freezePush(bool freeze){
    m_freezePush = freeze;
}



void CircularBufferOdo::startRecording(void){
    this->init();
    m_endPopRecord = 0;
    m_endbitPopRecord = 0;
    m_validRedord = true;
}

bool CircularBufferOdo::recordIsValid(void){
    return  m_validRedord;
}

void CircularBufferOdo::stopRecording(void){
    m_freezePush = true;
}

bool CircularBufferOdo::popRecod(uint8_t &data){
    if (isEmpty()) {
        usartprintf("Erreur: Buffer vide.\n");
        return false;
    }

#ifdef OPTIMIZE_BUFFER
    data = ((m_buffer[m_endPopRecord]>>m_endbitPopRecord) & 0x03);
    m_endbitPopRecord += 2;

    if (m_endbitPopRecord == 8) {
        m_endbitPopRecord = 0;
        m_endPopRecord ++;
        if(m_endPopRecord == m_size){
            m_endPopRecord = 0;
        }
    }
#else
    data = (m_buffer[m_endPopRecord]);
    m_endPopRecord ++;
    if(m_endPopRecord == m_size){
        m_endPopRecord = 0;
    }
#endif

    return true;
}

bool CircularBufferOdo::recordIsEmpty() const {
#ifdef OPTIMIZE_BUFFER
    return (m_startBuffer == m_endPopRecord) && (m_endbitPopRecord == m_startbitCount);
#else
    return (m_startBuffer == m_endPopRecord);
#endif
}

uint8_t CircularBufferOdo::popRecod(void){
    uint8_t data;
    this->pop(data);
    return data;
}
void CircularBufferOdo::resetPopRecord(void){
    m_endPopRecord = 0;
    m_endbitPopRecord = 0;
}