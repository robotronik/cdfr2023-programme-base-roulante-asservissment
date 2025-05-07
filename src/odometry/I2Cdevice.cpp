#include "odometry/I2Cdevice.h"
#include "clock.h"
#include "config.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>

const int i2c = I2C3;

void setupDeviceI2C(){
    rcc_periph_clken rcc_i2c = RCC_I2C3;

	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOC);
	//rcc_periph_clock_enable(RCC_GPIOH);
	//rcc_set_i2c_clock_hsi(i2c);

	gpio_mode_setup(port_OdoClk, GPIO_MODE_AF, GPIO_PUPD_NONE, pin_OdoClk);
	gpio_mode_setup(port_OdoSda, GPIO_MODE_AF, GPIO_PUPD_NONE, pin_OdoSda);

	// it's important to set the pins to open drain
	gpio_set_output_options(port_OdoClk, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, pin_OdoClk);
	gpio_set_output_options(port_OdoSda, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, pin_OdoSda);
	gpio_set_af(port_OdoClk, GPIO_AF4, pin_OdoClk);
	gpio_set_af(port_OdoSda, GPIO_AF4, pin_OdoSda);
	rcc_periph_clock_enable(rcc_i2c);
	i2c_peripheral_disable(i2c);
	/* HSI is at 8Mhz */
	i2c_set_speed(i2c, i2c_speed_sm_100k, 8);
	//addressing mode
	//i2c_set_7bit_addr_mode(i2c);
	i2c_set_standard_mode(i2c);
	i2c_peripheral_enable(i2c);
}

I2CDevice::I2CDevice(uint8_t address)
{
    setAddress(address);
    setRetries(3);
}


int I2CDevice::readRegisters(uint8_t reg, uint8_t *data, uint8_t size, int &bytesRead)
{
    i2c_transfer7(i2c, _address, &reg, 1, data, size);
	bytesRead = size;
    return 0;
}

int I2CDevice::readRegister(uint8_t reg, uint8_t &data)
{
    int bytesRead;
    return readRegisters(reg, &data, 1, bytesRead);
}

int I2CDevice::writeRegisters(uint8_t reg, uint8_t *data, uint8_t size)
{
    i2c_transfer7(i2c, _address, &reg, 1, data, size);
    return 0;
}

int I2CDevice::writeRegister(uint8_t reg, uint8_t data)
{
    return writeRegisters(reg, &data, 1);
}

void I2CDevice::setAddress(uint8_t address)
{
    _address = address;
}