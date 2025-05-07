#pragma once

#include "stdint.h"
#include "odometry/I2C3_Master.h"

class I2CDevice
{
    public:
        I2CDevice(int i2c_base, uint8_t address = 0x17);
        void begin();
        int readRegister(uint8_t reg, uint8_t *data, uint8_t size, int &bytesRead);
        int readRegister(uint8_t reg, uint8_t &data);
        int writeRegister(uint8_t reg, const uint8_t *data, uint8_t size);
        int writeRegister(uint8_t reg, const uint8_t &data);
        uint8_t address() { return _address; }
        void setAddress(uint8_t address);
        void setTimeout(uint32_t timeout);
        uint32_t getTimeout() { return _timeout; }
        void setRetries(uint8_t retries) { _retries = retries; }
        uint8_t getRetries() { return _retries; }
        void setClockSpeed(uint32_t clockSpeed);
        uint32_t getClockSpeed() { return _clockSpeed; }
    private:
        int _i2c;
        uint8_t _address;
        uint32_t _timeout;
        uint8_t _retries;
        uint32_t _clockSpeed;
        uint32_t _i2cTimeout;
};
