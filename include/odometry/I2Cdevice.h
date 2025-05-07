#pragma once

#include "stdint.h"

void setupDeviceI2C();

class I2CDevice
{
    public:
        I2CDevice(uint8_t address = 0x17);
        int readRegisters(uint8_t reg, uint8_t *data, uint8_t size, int &bytesRead);
        int readRegister(uint8_t reg, uint8_t &data);
        int writeRegisters(uint8_t reg, uint8_t *data, uint8_t size);
        int writeRegister(uint8_t reg, uint8_t data);
        void setAddress(uint8_t address);
        uint8_t address(){ return _address; }
        void setRetries(uint8_t retries) { _retries = retries; }
        uint8_t getRetries(){ return _retries; }
    private:
        int _i2c;
        uint8_t _address;
        uint8_t _retries;
};
