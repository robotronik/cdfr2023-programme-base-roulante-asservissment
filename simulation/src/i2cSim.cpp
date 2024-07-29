#include "i2cSim.h"

char I2C1_SR1 = 0;
char I2C1_SR2 = 0;
uint8_t dataGlobal_send;
uint8_t dataGlobal_receive;
std::atomic<bool> lockI2C(false); // Variable atomique pour le verrou

void I2cSendData (uint8_t command, uint8_t* data, int length){
    while (lockI2C.exchange(true)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    I2C1_SR1 = I2C_SR1_ADDR;
    I2C1_SR2 = 0;
    i2c1_ev_isr();
    I2C1_SR1 = I2C_SR1_RxNE;
    dataGlobal_send = command;
    i2c1_ev_isr();
    for(int i = 0 ; i < length; i++){
        I2C1_SR1 = I2C_SR1_RxNE;
        dataGlobal_send = data[i];
        i2c1_ev_isr();
    }
    I2C1_SR1 = I2C_SR1_STOPF;
    i2c1_ev_isr();
    lockI2C.store(false);
}

void I2cReceiveData (uint8_t command, uint8_t* data, int length){
    while (lockI2C.exchange(true)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    I2C1_SR1 = I2C_SR1_ADDR;
    I2C1_SR2 = 0;
    i2c1_ev_isr();
    I2C1_SR1 = I2C_SR1_RxNE;
    dataGlobal_send = command;
    i2c1_ev_isr();
    I2C1_SR1 = I2C_SR1_STOPF;
    i2c1_ev_isr();

    I2C1_SR1 = I2C_SR1_ADDR;
    I2C1_SR2 = I2C_SR2_TRA;
    i2c1_ev_isr();
    for(int i = 0; i< length; i++){
        I2C1_SR1 = I2C_SR1_TxE;
        i2c1_ev_isr();
        data[i] = dataGlobal_receive;
    }
    I2C1_SR1 = I2C_SR1_STOPF;
    i2c1_ev_isr();
    lockI2C.store(false);
}


void i2c_send_data(uint32_t i2c, uint8_t data){dataGlobal_receive = data;}
uint8_t i2c_get_data(uint32_t i2c){return dataGlobal_send;}