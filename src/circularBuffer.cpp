#include "circularBuffer.h"

CircularBuffer::CircularBuffer(int size,uint8_t* buffer){
    m_buffer = buffer;
    m_size = size;
    this->init();
}


void CircularBuffer::init(void){
    m_startBuffer = 0;
    m_startbitCount = 0;
    m_endBuffer = 0;
    m_endbitCount = 0;
}


//since the buffer is large and never full, there is no need to check the last 4 data with m_endbitCount and m_startbitCount
bool CircularBuffer::isFull() const {
    return ((m_startBuffer == m_endBuffer-1) || (m_startBuffer == 0 && m_endBuffer == m_size-1));
}


bool CircularBuffer::isEmpty() const {
#ifdef OPTIMIZE_BUFFER
    return (m_startBuffer == m_endBuffer) && (m_endbitCount == m_startbitCount);
#else
    return (m_startBuffer == m_endBuffer);
#endif
}


bool CircularBuffer::push(uint8_t data) {
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

uint8_t CircularBuffer::pop(void){
    uint8_t data;
    this->pop(data);
    return data;
}

bool CircularBuffer::pop(uint8_t &data) {
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
