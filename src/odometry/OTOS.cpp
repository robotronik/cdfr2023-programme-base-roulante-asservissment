
#include "odometry/OTOS.h"
#include "clock.h"

OTOS::OTOS()
{
    _commBus = nullptr;
    _angularUnit = kOtosAngularUnitDegrees;
    _meterToUnit = 1000.0;
    _radToUnit = kRadianToDegree;
}

return_t OTOS::begin(I2CDevice& commBus)
{
    commBus.setAddress(kDefaultAddress);
    // Check the device address
    if (commBus.address() != kDefaultAddress)
        return ret_FAIL;

    // Set bus pointer
    _commBus = &commBus;
    // Just check if the device is connected, no other setup is needed
    return isConnected();
}

return_t OTOS::isConnected()
{
    // Read the product ID
    uint8_t prodId;
    int err = _commBus->readRegister(kRegProductId, prodId);
    if (err != 0)
        return ret_FAIL;

    // Check if the product ID is correct
    if (prodId != kProductId)
        return ret_FAIL;

    // Everything checks out, we must be connected!
    return ret_OK;
}

return_t OTOS::getVersionInfo(otos_version_t &hwVersion, otos_version_t &fwVersion)
{
    // Read hardware and firmware version registers
    uint8_t rawData[2];
    int readBytes;
    int err = _commBus->readRegisters(kRegHwVersion, rawData, sizeof(rawData), readBytes);
    if (err != 0)
        return ret_FAIL;

    // Check if we read the correct number of bytes
    if (readBytes != 2)
        return ret_FAIL;

    // Store the version info
    hwVersion.value = rawData[0];
    fwVersion.value = rawData[1];

    // Done!
    return ret_OK;
}

return_t OTOS::selfTest()
{
    // Write the self-test register to start the test
    otos_self_test_config_t selfTest;
    selfTest.start = 1;
    int err = _commBus->writeRegister(kRegSelfTest, selfTest.value);
    if (err != 0)
        return ret_FAIL;

    // Loop until self-test is done, should only take ~20ms as of firmware v1.0
    for (int i = 0; i < 10; i++)
    {
        // Give a short delay between reads
        delay_ms(5);

        // Read the self-test register
        err = _commBus->readRegister(kRegSelfTest, selfTest.value);
        if (err != 0)
            return ret_FAIL;

        // Check if the self-test is done
        if (selfTest.inProgress == 0)
        {
            break;
        }
    }

    // Check if the self-test passed
    return (selfTest.pass == 1) ? ret_OK : ret_FAIL;
}

return_t OTOS::calibrateImu(uint8_t numSamples, bool waitUntilDone)
{
    // Write the number of samples to the device
    int err = _commBus->writeRegister(kRegImuCalib, numSamples);
    if (err != 0)
        return ret_FAIL;

    // Wait 1 sample period (2.4ms) to ensure the register updates
    delay_ms(3);

    // Do we need to wait until the calibration finishes?
    if (!waitUntilDone)
        return ret_OK;

    // Wait for the calibration to finish, which is indicated by the IMU
    // calibration register reading zero, or until we reach the maximum number
    // of read attempts
    for (uint8_t numAttempts = numSamples; numAttempts > 0; numAttempts--)
    {
        // Read the gryo calibration register value
        uint8_t calibrationValue;
        err = _commBus->readRegister(kRegImuCalib, calibrationValue);
        if (err != 0)
            return ret_FAIL;

        // Check if calibration is done
        if (calibrationValue == 0)
            return ret_OK;

        // Give a short delay between reads. As of firmware v1.0, samples take
        // 2.4ms each, so 3ms should guarantee the next sample is done. This
        // also ensures the max attempts is not exceeded in normal operation
        delay_ms(3);
    }

    // Max number of attempts reached, calibration failed
    return ret_FAIL;
}

return_t OTOS::getImuCalibrationProgress(uint8_t &numSamples)
{
    // Read the IMU calibration register
    return _commBus->readRegister(kRegImuCalib, numSamples) == 0 ? ret_OK : ret_FAIL;
}

otos_angular_unit_t OTOS::getAngularUnit()
{
    return _angularUnit;
}

void OTOS::setAngularUnit(otos_angular_unit_t unit)
{
    // Check if this unit is already set
    if (unit == _angularUnit)
        return;

    // Store new unit
    _angularUnit = unit;

    // Compute conversion factor to new units
    _radToUnit = (unit == kOtosAngularUnitRadians) ? 1.0f : kRadianToDegree;
}

return_t OTOS::getLinearScalar(float &scalar)
{
    // Read the linear scalar from the device
    uint8_t rawScalar;
    int err = _commBus->readRegister(kRegScalarLinear, rawScalar);
    if (err != 0)
        return ret_FAIL;

    // Convert to float, multiples of 0.1%
    scalar = (((int8_t)rawScalar) * 0.001f) + 1.0f;

    // Done!
    return ret_OK;
}

return_t OTOS::setLinearScalar(float scalar)
{
    // Check if the scalar is out of bounds
    if (scalar < kMinScalar || scalar > kMaxScalar)
        return ret_FAIL;

    // Convert to integer, multiples of 0.1% (+0.5 to round instead of truncate)
    uint8_t rawScalar = (int8_t)((scalar - 1.0f) * 1000 + 0.5f);

    // Write the scalar to the device
    return _commBus->writeRegister(kRegScalarLinear, rawScalar) == 0 ? ret_OK : ret_FAIL;
}

return_t OTOS::getAngularScalar(float &scalar)
{
    // Read the angular scalar from the device
    uint8_t rawScalar;
    int err = _commBus->readRegister(kRegScalarAngular, rawScalar);
    if (err != 0)
        return ret_FAIL;

    // Convert to float, multiples of 0.1%
    scalar = (((int8_t)rawScalar) * 0.001f) + 1.0f;

    // Done!
    return ret_OK;
}

return_t OTOS::setAngularScalar(float scalar)
{
    // Check if the scalar is out of bounds
    if (scalar < kMinScalar || scalar > kMaxScalar)
        return ret_FAIL;

    // Convert to integer, multiples of 0.1% (+0.5 to round instead of truncate)
    uint8_t rawScalar = (int8_t)((scalar - 1.0f) * 1000 + 0.5f);

    // Write the scalar to the device
    return _commBus->writeRegister(kRegScalarAngular, rawScalar) == 0 ? ret_OK : ret_FAIL;
}

return_t OTOS::resetTracking()
{
    // Set tracking reset bit
    return _commBus->writeRegister(kRegReset, (uint8_t)0x01) == 0 ? ret_OK : ret_FAIL;
}

return_t OTOS::getSignalProcessConfig(otos_signal_process_config_t &config)
{
    // Read the signal process register
    return _commBus->readRegister(kRegSignalProcess, config.value) == 0 ? ret_OK : ret_FAIL;
}

return_t OTOS::setSignalProcessConfig(otos_signal_process_config_t &config)
{
    // Write the signal process register
    return _commBus->writeRegister(kRegSignalProcess, config.value) == 0 ? ret_OK : ret_FAIL;
}

return_t OTOS::getStatus(otos_status_t &status)
{
    return _commBus->readRegister(kRegStatus, status.value) == 0 ? ret_OK : ret_FAIL;
}

return_t OTOS::getOffset(position_t &pose)
{
    return readPoseRegs(kRegOffXL, pose, kInt16ToMeter, kInt16ToRad);
}

return_t OTOS::setOffset(position_t &pose)
{
    return writePoseRegs(kRegOffXL, pose, kMeterToInt16, kRadToInt16);
}

return_t OTOS::getPosition(position_t &pose)
{
    return readPoseRegs(kRegPosXL, pose, kInt16ToMeter, kInt16ToRad);
}

return_t OTOS::setPosition(position_t &pose)
{
    return writePoseRegs(kRegPosXL, pose, kMeterToInt16, kRadToInt16);
}

return_t OTOS::getVelocity(position_t &pose)
{
    return readPoseRegs(kRegVelXL, pose, kInt16ToMps, kInt16ToRps);
}

return_t OTOS::getAcceleration(position_t &pose)
{
    return readPoseRegs(kRegAccXL, pose, kInt16ToMpss, kInt16ToRpss);
}

return_t OTOS::getPositionStdDev(position_t &pose)
{
    return readPoseRegs(kRegPosStdXL, pose, kInt16ToMeter, kInt16ToRad);
}

return_t OTOS::getVelocityStdDev(position_t &pose)
{
    return readPoseRegs(kRegVelStdXL, pose, kInt16ToMps, kInt16ToRps);
}

return_t OTOS::getAccelerationStdDev(position_t &pose)
{
    return readPoseRegs(kRegAccStdXL, pose, kInt16ToMpss, kInt16ToRpss);
}

return_t OTOS::getPosVelAcc(position_t &pos, position_t &vel, position_t &acc)
{
    // Read all pose registers
    uint8_t rawData[18];
    int bytesRead;
    int err = _commBus->readRegisters(kRegPosXL, rawData, 18, bytesRead);
    if (err != 0)
        return ret_FAIL;

    // Check if we read the correct number of bytes
    if (bytesRead != 18)
        return ret_FAIL;

    // Convert raw data to pose units
    regsToPose(rawData, pos, kInt16ToMeter, kInt16ToRad);
    regsToPose(rawData + 6, vel, kInt16ToMps, kInt16ToRps);
    regsToPose(rawData + 12, acc, kInt16ToMpss, kInt16ToRpss);

    // Done!
    return ret_OK;
}

return_t OTOS::getPosVelAccStdDev(position_t &pos, position_t &vel, position_t &acc)
{
    // Read all pose registers
    uint8_t rawData[18];
    int bytesRead;
    int err = _commBus->readRegisters(kRegPosStdXL, rawData, 18, bytesRead);
    if (err != 0)
        return ret_FAIL;

    // Check if we read the correct number of bytes
    if (bytesRead != 18)
        return ret_FAIL;

    // Convert raw data to pose units
    regsToPose(rawData, pos, kInt16ToMeter, kInt16ToRad);
    regsToPose(rawData + 6, vel, kInt16ToMps, kInt16ToRps);
    regsToPose(rawData + 12, acc, kInt16ToMpss, kInt16ToRpss);

    // Done!
    return ret_OK;
}

return_t OTOS::getPosVelAccAndStdDev(position_t &pos, position_t &vel, position_t &acc,
                                             position_t &posStdDev, position_t &velStdDev,
                                             position_t &accStdDev)
{
    // Read all pose registers
    uint8_t rawData[36];
    int bytesRead;
    int err = _commBus->readRegisters(kRegPosXL, rawData, 36, bytesRead);
    if (err != 0)
        return ret_FAIL;

    // Check if we read the correct number of bytes
    if (bytesRead != 36)
        return ret_FAIL;

    // Convert raw data to pose units
    regsToPose(rawData, pos, kInt16ToMeter, kInt16ToRad);
    regsToPose(rawData + 6, vel, kInt16ToMps, kInt16ToRps);
    regsToPose(rawData + 12, acc, kInt16ToMpss, kInt16ToRpss);
    regsToPose(rawData + 18, posStdDev, kInt16ToMeter, kInt16ToRad);
    regsToPose(rawData + 24, velStdDev, kInt16ToMps, kInt16ToRps);
    regsToPose(rawData + 30, accStdDev, kInt16ToMpss, kInt16ToRpss);

    // Done!
    return ret_OK;
}

return_t OTOS::readPoseRegs(uint8_t reg, position_t &pose, float rawToXY, float rawToH)
{
    int bytesRead;
    uint8_t rawData[6];

    // Attempt to read the raw pose data
    int err = _commBus->readRegisters(reg, rawData, 6, bytesRead);
    if (err != 0)
        return ret_FAIL;

    // Check if we read the correct number of bytes
    if (bytesRead != 6)
        return ret_FAIL;

    regsToPose(rawData, pose, rawToXY, rawToH);

    // Done!
    return ret_OK;
}

return_t OTOS::writePoseRegs(uint8_t reg, position_t &pose, float xyToRaw, float hToRaw)
{
    // Store raw data in a temporary buffer
    uint8_t rawData[6];
    poseToRegs(rawData, pose, xyToRaw, hToRaw);

    // Write the raw data to the device
    return _commBus->writeRegisters(reg, rawData, 6) == 0 ? ret_OK : ret_FAIL;
}

void OTOS::regsToPose(uint8_t *rawData, position_t &pose, float rawToXY, float rawToH)
{
    // Store raw data
    int16_t rawX = (rawData[1] << 8) | rawData[0];
    int16_t rawY = (rawData[3] << 8) | rawData[2];
    int16_t rawH = (rawData[5] << 8) | rawData[4];

    // Store in pose and convert to units
    pose.x = rawX * rawToXY * _meterToUnit;
    pose.y = rawY * rawToXY * _meterToUnit;
    pose.a = rawH * rawToH * _radToUnit;
}

void OTOS::poseToRegs(uint8_t *rawData, position_t &pose, float xyToRaw, float hToRaw)
{
    // Convert pose units to raw data
    int16_t rawX = pose.x * xyToRaw / _meterToUnit;
    int16_t rawY = pose.y * xyToRaw / _meterToUnit;
    int16_t rawH = pose.a * hToRaw / _radToUnit;

    // Store raw data in buffer
    rawData[0] = rawX & 0xFF;
    rawData[1] = (rawX >> 8) & 0xFF;
    rawData[2] = rawY & 0xFF;
    rawData[3] = (rawY >> 8) & 0xFF;
    rawData[4] = rawH & 0xFF;
    rawData[5] = (rawH >> 8) & 0xFF;
}
