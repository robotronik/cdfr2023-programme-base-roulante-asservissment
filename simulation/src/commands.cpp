#include "commands.h"


void turn_on_LED_1() {
    uint8_t command = 10;
    uint8_t* data = nullptr;
    int length = 0;
    I2cSendData(command, data, length);
}

void turn_off_LED_1() {
    uint8_t command = 11;
    uint8_t* data = nullptr;
    int length = 0;
    I2cSendData(command, data, length);
}

void turn_on_LED_2() {
    uint8_t command = 12;
    uint8_t* data = nullptr;
    int length = 0;
    I2cSendData(command, data, length);
}

void turn_off_LED_2() {
    uint8_t command = 13;
    uint8_t* data = nullptr;
    int length = 0;
    I2cSendData(command, data, length);
}

void get_coordinates(int16_t &x, int16_t &y, int16_t &theta) {
    uint8_t command = 20;
    uint8_t data[6];
    int length = 6;
    I2cReceiveData(command, data, length);

    DataUnpacker unpacker(data, length);
    x = unpacker.popUint16();
    y = unpacker.popUint16();
    theta = unpacker.popUint16();
}

void set_coordinates(int16_t x, int16_t y, int16_t theta) {
    uint8_t command = 21;
    DataPacker packer;
    packer.addUint16(x);
    packer.addUint16(y);
    packer.addUint16(theta);
    I2cSendData(command, packer.getData(), packer.getSize());
}

void stop() {
    uint8_t command = 30;
    uint8_t* data = nullptr;
    int length = 0;
    I2cSendData(command, data, length);
}

void set_consigne_lineaire(int16_t x, int16_t y) {
    uint8_t command = 31;
    DataPacker packer;
    packer.addUint16(x);
    packer.addUint16(y);

    I2cSendData(command, packer.getData(), packer.getSize());
}

void set_consigne_angulaire(int16_t angle, int16_t rotation) {
    uint8_t command = 32;
    DataPacker packer;
    packer.addUint16(angle);
    packer.addUint16(rotation);

    I2cSendData(command, packer.getData(), packer.getSize());
}

void set_consigne_lookAt_forward(int16_t x, int16_t y, int16_t rotation) {
    uint8_t command = 33;
    DataPacker packer;
    packer.addUint16(x);
    packer.addUint16(y);
    packer.addUint16(rotation);

    I2cSendData(command, packer.getData(), packer.getSize());
}

void set_consigne_lookAt_backward(int16_t x, int16_t y, int16_t rotation) {
    uint8_t command = 34;
    DataPacker packer;
    packer.addUint16(x);
    packer.addUint16(y);
    packer.addUint16(rotation);

    I2cSendData(command, packer.getData(), packer.getSize());
}

void robot_moving_is_finish(int16_t &resbool) {
    uint8_t command = 40;
    uint8_t data[2];
    int length = 2;
    I2cReceiveData(command, data, length);

    DataUnpacker unpacker(data, length);
    resbool = unpacker.popUint16();
}

void robot_running_is_finish(int16_t &resbool) {
    uint8_t command = 41;
    uint8_t data[2];
    int length = 2;
    I2cReceiveData(command, data, length);

    DataUnpacker unpacker(data, length);
    resbool = unpacker.popUint16();
}

void robot_turning_is_finish(int16_t &resbool) {
    uint8_t command = 42;
    uint8_t data[2];
    int length = 2;
    I2cReceiveData(command, data, length);

    DataUnpacker unpacker(data, length);
    resbool = unpacker.popUint16();
}

void get_linear_error(int16_t &error) {
    uint8_t command = 43;
    uint8_t data[2];
    int length = 2;
    I2cReceiveData(command, data, length);

    DataUnpacker unpacker(data, length);
    error = unpacker.popUint16();
}

void get_angular_error(int16_t &error) {
    uint8_t command = 44;
    uint8_t data[2];
    int length = 2;
    I2cReceiveData(command, data, length);

    DataUnpacker unpacker(data, length);
    error = unpacker.popUint16();
}

void get_braking_distance(int16_t &distance) {
    uint8_t command = 45;
    uint8_t data[2];
    int length = 2;
    I2cReceiveData(command, data, length);

    DataUnpacker unpacker(data, length);
    distance = unpacker.popUint16();
}

void disable_motor() {
    uint8_t command = 50;
    uint8_t* data = nullptr;
    int length = 0;
    I2cSendData(command, data, length);
}

void enable_motor() {
    uint8_t command = 51;
    uint8_t* data = nullptr;
    int length = 0;
    I2cSendData(command, data, length);
}

void set_max_speed_forward(int16_t speed) {
    uint8_t command = 60;
    DataPacker packer;
    packer.addUint16(speed);

    I2cSendData(command, packer.getData(), packer.getSize());
}

void set_max_speed_backward(int16_t speed) {
    uint8_t command = 61;
    DataPacker packer;
    packer.addUint16(speed);

    I2cSendData(command, packer.getData(), packer.getSize());
}

void set_max_speed_trigo(int16_t speed) {
    uint8_t command = 62;
    DataPacker packer;
    packer.addUint16(speed);

    I2cSendData(command, packer.getData(), packer.getSize());
}

void set_max_speed_horloge(int16_t speed) {
    uint8_t command = 63;
    DataPacker packer;
    packer.addUint16(speed);

    I2cSendData(command, packer.getData(), packer.getSize());
}