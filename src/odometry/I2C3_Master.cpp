#include "odometry/I2C3_Master.h"

#include <errno.h>
#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>

int _write(int file, char *ptr, int len);

static void i2c3_setup(void)
{
	rcc_periph_clock_enable(RCC_I2C3);
	rcc_periph_clock_enable(RCC_GPIOB);





	/* Setup GPIO pin GPIO_USART2_TX/GPIO9 on GPIO port A for transmit. */
	gpio_mode_setup(port_I2CClk, GPIO_MODE_AF, GPIO_PUPD_NONE, pin_I2CClk | pin_I2CSda);
	gpio_set_output_options(port_I2CClk, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, pin_I2CClk | pin_I2CSda);
	gpio_set_af(port_I2CClk, GPIO_AF4, pin_I2CClk | pin_I2CSda);
	i2c_peripheral_disable(I2C3);

	nvic_enable_irq(NVIC_I2C3_EV_IRQ);
	nvic_enable_irq(NVIC_I2C3_ER_IRQ);
    nvic_set_priority(NVIC_I2C3_ER_IRQ, 16);
    nvic_set_priority(NVIC_I2C3_EV_IRQ, 17);

	/* HSI is at 8Mhz */
	i2c_set_speed(I2C3, i2c_speed_sm_100k, 8);

	i2c_enable_interrupt(I2C3, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN);

	///I2C3_CR1 |= I2C_CR1_NOSTRETCH;

	//addressing mode
    i2c_set_own_7bit_slave_address(I2C3, ADDRI2CBASEROULANTE);

	i2c_peripheral_enable(I2C3);
	i2c_enable_ack(I2C3);






	rcc_periph_reset_pulse(RST_I2C3);
	/* Setup GPIO pin GPIO_USART2_TX/GPIO9 on GPIO port A for transmit. */
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6 | GPIO7);
	gpio_set_af(GPIOB, GPIO_AF4, GPIO6 | GPIO7);
	i2c_peripheral_disable(I2C3);
	//configure ANFOFF DNF[3:0] in CR1
	i2c_enable_analog_filter(I2C3);
	i2c_set_digital_filter(I2C3, 0);
	/* HSI is at 8Mhz */
	i2c_set_speed(I2C3, i2c_speed_sm_100k, 8);
	//configure No-Stretch CR1 (only relevant in slave mode)
	i2c_enable_stretching(I2C3);
	//addressing mode
	i2c_set_7bit_addr_mode(I2C3);
	i2c_peripheral_enable(I2C3);
}

static void usart_setup(void)
{
	/* Enable clocks for GPIO port A (for GPIO_USART2_TX) and USART2. */
	rcc_periph_clock_enable(RCC_USART2);
	rcc_periph_clock_enable(RCC_GPIOA);

	/* Setup GPIO pin GPIO_USART2_TX/GPIO9 on GPIO port A for transmit. */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
	gpio_set_af(GPIOA, GPIO_AF7, GPIO2| GPIO3);

	/* Setup UART parameters. */
	usart_set_baudrate(USART2, 115200);
	usart_set_databits(USART2, 8);
	usart_set_stopbits(USART2, USART_STOPBITS_1);
	usart_set_mode(USART2, USART_MODE_TX_RX);
	usart_set_parity(USART2, USART_PARITY_NONE);
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

	/* Finally enable the USART. */
	usart_enable(USART2);
}

static void gpio_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOE);
	gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
		GPIO8 | GPIO9 | GPIO10 | GPIO11 | GPIO12 | GPIO13 |
		GPIO14 | GPIO15);
}

int _write(int file, char *ptr, int len)
{
        int i;

        if (file == 1) {
                for (i = 0; i < len; i++) {
			if (ptr[i] == '\n') {
				usart_send_blocking(USART2, '\r');
			}
                        usart_send_blocking(USART2, ptr[i]);
		}
                return i;
        }
        errno = EIO;
        return -1;
}


static void clock_setup(void)
{
	rcc_clock_setup_hsi(&rcc_hsi_configs[RCC_CLOCK_HSI_64MHZ]);
}

#define I2C_ACC_ADDR 0x19
#define I2C_MAG_ADDR 0x1E
#define ACC_STATUS 0x27
#define ACC_CTRL_REG1_A 0x20
#define ACC_CTRL_REG1_A_ODR_SHIFT 4
#define ACC_CTRL_REG1_A_ODR_MASK 0xF
#define ACC_CTRL_REG1_A_XEN (1 << 0)
#define ACC_CTRL_REG4_A 0x23

#define ACC_OUT_X_L_A 0x28
#define ACC_OUT_X_H_A 0x29

int main(void)
{
	clock_setup();
	gpio_setup();
	usart_setup();
	printf("Hello, we're running\n");
	i2c_setup();
	uint8_t cmd = ACC_CTRL_REG1_A;
	uint8_t data;
	i2c_transfer7(I2C3, I2C_ACC_ADDR, &cmd, 1, &data, 1);
	cmd = ACC_CTRL_REG4_A;
	i2c_transfer7(I2C3, I2C_ACC_ADDR, &cmd, 1, &data, 1);
	int16_t acc_x;

	while (1) {

		cmd = ACC_STATUS;
		i2c_transfer7(I2C3, I2C_ACC_ADDR, &cmd, 1, &data, 1);
		cmd = ACC_OUT_X_L_A;
		i2c_transfer7(I2C3, I2C_ACC_ADDR, &cmd, 1, &data, 1);
		acc_x = data;
		cmd = ACC_OUT_X_H_A;
		i2c_transfer7(I2C3, I2C_ACC_ADDR, &cmd, 1, &data, 1);
		acc_x |= ((uint16_t)data << 8);
		printf("data was %d\n", acc_x);
	}

	return 0;
}





























#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>

#include <string.h>

#define LED PC13


const uint8_t addr = 0x70; // set this to whatever your slave address is
uint8_t ledmat_grid[8];


void ledmat_send_data(uint8_t *data, int len) 
{
	i2c_transfer7(I2C3, addr, data, len, 0 , 0);
}


void ledmat_send_cmd(uint8_t cmd)
	//void LedMat::send_cmd(uint8_t cmd)
{
	//i2c_write_blocking(m_i2c_inst, m_addr, &cmd, 1, false);
	//HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
	ledmat_send_data(&cmd, 1);
}

void ledmat_init(void) 
{
	ledmat_send_cmd(0x20 | 1); // turn on oscillator
	ledmat_show(); // clear out any display that is there
	ledmat_send_cmd(0x81); // display on
	ledmat_send_cmd(0xE0 | 0); // brightness to dimmest (but you should probably set it)

}

void ledmat_set(uint8_t row, uint8_t col, bool on) 
{
	// the LED has a strange co-ordinate system
	int pos1 = 7 - col; // actually a "row"
	int pos2 = row - 1;
	if (pos2 == -1)
		pos2 = 7;
	//m_grid[pos1].set(pos2, on);
	ledmat_grid[pos1] &= ~(1 << pos2);
	if (on)
		ledmat_grid[pos1] |= (1 << pos2);

}

void ledmat_show(void) 
{
	for (int r = 0; r < 8; r++) {
		uint8_t data[2];
		data[0] = r * 2;
		//data[1] = m_grid[r].to_ulong();
		data[1] = ledmat_grid[r];
		ledmat_send_data(data, 2);
	}

}


static void i2c_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOB);
	//rcc_periph_clock_enable(RCC_GPIOH);
	//rcc_set_i2c_clock_hsi(I2C3);

	i2c_reset(I2C3);
	/* Setup GPIO pin GPIO_USART2_TX/GPIO9 on GPIO port A for transmit. */
	//gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6 | GPIO7);
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO6 | GPIO7);
	//gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO6 | GPIO7);

	// it's important to set the pins to open drain
	gpio_set_output_options(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, GPIO6 | GPIO7);
	gpio_set_af(GPIOB, GPIO_AF4, GPIO6 | GPIO7);
	rcc_periph_clock_enable(RCC_I2C3);
	i2c_peripheral_disable(I2C3);
	//configure ANFOFF DNF[3:0] in CR1
	//i2c_enable_analog_filter(I2C3);
	//i2c_set_digital_filter(I2C3, 0);
	/* HSI is at 8Mhz */
	i2c_set_speed(I2C3, i2c_speed_sm_100k, 8);
	//configure No-Stretch CR1 (only relevant in slave mode)
	//i2c_enable_stretching(I2C3);
	//addressing mode
	//i2c_set_7bit_addr_mode(I2C3);
	i2c_set_standard_mode(I2C3); // mcarter added 2021-11-23
	i2c_peripheral_enable(I2C3);
}

int main(void)
{
	pin_out(LED);
	mal_usart_init();

	myputs("");
	myputs("=============================");
	myputs("I2C example: master");

#if 1
	i2c_setup();
#else
	rcc_periph_clock_enable(RCC_I2C3);
	rcc_periph_clock_enable(RCC_GPIOB);
	//i2c_reset(I2C3);
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6 | GPIO7);
	gpio_set_af(GPIOB, GPIO_AF4, GPIO6 | GPIO7);
	//i2c_peripheral_disable(I2C3);
	//i2c_enable_ack(I2C3);
	i2c_peripheral_enable(I2C3);
#endif
	uint8_t data;

	ledmat_init();
	//initalise display
	for (int r = 0; r < 8; r++) {
		uint8_t row = patternX[r];
		for (int c = 0; c < 8; c++) {
			ledmat_set(r, c, row >> 7);
			row <<= 1;
		}
	}


	while(1) {
		ledmat_show();

		nop();
		mal_delayish(1);
		pin_toggle(LED);
	}

}