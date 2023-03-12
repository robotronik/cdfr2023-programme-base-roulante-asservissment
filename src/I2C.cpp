#include "I2C.h"

#include "clock.h"

void (*callbacki2cRec)(uint8_t* data, int size);
bool callbackinitialiseRec = false;
void (*callbacki2cTrans)(void);
bool callbackinitialiseTrans =  false;

uint8_t bufrec[BUFFERSIZE];
uint8_t bufsend[BUFFERSIZE];
int reading;
int sending;
commnucationDirection_t communicationType;

//https://github.com/amitesh-singh/i2c-slave-stm32f1/blob/master/main.cpp

void i2c_setup(void){
    rcc_periph_clock_enable(RCC_I2C1);
	rcc_periph_clock_enable(RCC_GPIOB);

	//rcc_periph_reset_pulse(RST_I2C1);
	/* Setup GPIO pin GPIO_USART2_TX/GPIO9 on GPIO port A for transmit. */
	gpio_mode_setup(port_I2CClk, GPIO_MODE_AF, GPIO_PUPD_NONE, pin_I2CClk | pin_I2CSda);
	gpio_set_output_options(port_I2CClk, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, pin_I2CClk | pin_I2CSda);
	gpio_set_af(port_I2CClk, GPIO_AF4, pin_I2CClk | pin_I2CSda);
	i2c_peripheral_disable(I2C1);

	nvic_enable_irq(NVIC_I2C1_EV_IRQ);
	nvic_enable_irq(NVIC_I2C1_ER_IRQ);

	/* HSI is at 8Mhz */
	i2c_set_speed(I2C1, i2c_speed_sm_100k, 8);

	i2c_enable_interrupt(I2C1, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN);

	//I2C1_CR1 |= I2C_CR1_NOSTRETCH;

	//addressing mode	
    i2c_set_own_7bit_slave_address(I2C1,ADDRI2CBASEROULANTE);

	i2c_peripheral_enable(I2C1);
	i2c_enable_ack(I2C1);
}

void i2c1_er_isr(void){
	uint32_t sr1, sr2;
	sr1 = I2C1_SR1;
	sr2 = I2C1_SR2;
	(void)sr2;
	if(sr1 & I2C_SR1_SMBALERT){
		usartprintf("I2C ERROR SMBALERT\n");
		sr1 |= I2C_SR1_SMBALERT;
	}
	if(sr1 & I2C_SR1_TIMEOUT){
		usartprintf("I2C ERROR TIMEOUT\n");
		sr1 |= I2C_SR1_TIMEOUT;
	}
	if(sr1 & I2C_SR1_PECERR){
		usartprintf("I2C ERROR PECERR\n");
		sr1 |= I2C_SR1_PECERR;
	}
	if(sr1 & I2C_SR1_OVR){
		usartprintf("I2C ERROR OVR\n");
		sr1 |= I2C_SR1_OVR;
	}
	if(sr1 & I2C_SR1_AF){
		usartprintf("I2C ERROR SMBAFALERT\n");
		sr1 |= I2C_SR1_AF;
	}
	if(sr1 & I2C_SR1_BERR){
		usartprintf("I2C ERROR BERR\n");
		sr1 |= I2C_SR1_BERR;
	}
}

void i2c1_ev_isr(void){
	uint32_t sr1, sr2;
	sr1 = I2C1_SR1;
	sr2 = I2C1_SR2;
	(void)sr2;

	if(sr1 & I2C_SR1_ADDR){
		reading = 0;
		sending = 0;
		if(I2C1_SR2 & I2C_SR2_TRA){
			communicationType = DIRSEND;
			if(callbackinitialiseTrans){
				callbacki2cTrans();
			}			
		}
		else{
			communicationType = DIRRECEIVE;
		}
	}
	else if(sr1 & I2C_SR1_TxE){
		if(sending<BUFFERSIZE){
			i2c_send_data(I2C1,bufsend[sending]);
			sending++;
		}
		else{
			i2c_send_data(I2C1,0);
		}
	}
	else if(sr1 & I2C_SR1_RxNE){
		if(reading<BUFFERSIZE){
			bufrec[reading] = i2c_get_data(I2C1);
			reading++;
		}
		else{
			i2c_get_data(I2C1);
		}
	}
	else if(sr1 & I2C_SR1_STOPF){
		i2c_peripheral_enable(I2C1);
		if(callbackinitialiseRec && communicationType == DIRRECEIVE){
			callbacki2cRec(bufrec,reading);
		}
	}
}

void I2CGetBufffer(uint8_t* data, int size){
	if(size>BUFFERSIZE){
		size = BUFFERSIZE;
	}
	memcpy(data,bufrec,BUFFERSIZE);
}

void I2CSetBuffer(uint8_t* data, int size){
	if(size>BUFFERSIZE){
		size = BUFFERSIZE;
	}
	memcpy(bufsend,data,BUFFERSIZE);
}

void setCallbackReceive(void (*f)(uint8_t* data, int size)){
	callbacki2cRec = f;
	callbackinitialiseRec =  true;
}

void disableCallbackReceive(void){
	callbackinitialiseRec =  false;
}

void setCallbackTransmit(void (*f)(void)){
	callbacki2cTrans = f;
	callbackinitialiseTrans =  true;
}

void disableCallbackTransmit(void){
	callbackinitialiseTrans =  false;
}