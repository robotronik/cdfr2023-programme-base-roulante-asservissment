#pragma once

#include <stdint.h>
#include <stdbool.h>

#define ADDRI2CBASEROULANTE 42

#define BUFFERSIZE 256

typedef enum{
    DIRRECEIVE,
    DIRSEND
}commnucationDirection_t;

void i2c_setup(void);

void setCallbackReceive(void (*f)(uint8_t* data, int size));
void disableCallbackReceive(void);
void setCallbackSend(void (*f)(void));
void disableCallbackSend(void);

void I2CSetBuffer(uint8_t* data, int size);
void I2CGetBuffer(uint8_t* data, int size);
