#pragma once
#include <cstdint>
#include <stdio.h>
#include "odometrieSim.h"
#include "config.h"
#include "ledSim.h"


//*********************************************************************
// HARDWARE
//*********************************************************************

#define GPIO0   0
#define GPIO1   1
#define GPIO2   2
#define GPIO3   3
#define GPIO4   4
#define GPIO5   5
#define GPIO6   6
#define GPIO7   7
#define GPIO8   8
#define GPIO9   9
#define GPIO10  10
#define GPIO11  11
#define GPIO12  12
#define GPIO13  13
#define GPIO14  14
#define GPIO15  15

#define GPIOA   0
#define GPIOB   1
#define GPIOC   2
#define GPIOD   3


#define RCC_I2C1               0
#define RCC_GPIOB              0
#define GPIO_MODE_AF           0
#define GPIO_PUPD_NONE         0
#define GPIO_OTYPE_OD          0
#define GPIO_OSPEED_25MHZ      0
#define GPIO_AF4               0
#define I2C1                   0
#define NVIC_I2C1_EV_IRQ       0
#define NVIC_I2C1_ER_IRQ       0
#define i2c_speed_sm_100k      0
#define I2C_CR2_ITEVTEN        0
#define I2C_CR2_ITBUFEN        0
#define RCC_GPIOA              0
#define RCC_GPIOC              0
#define GPIO_MODE_OUTPUT       0
#define GPIO_MODE_INPUT        0
#define GPIO_AF1               0
#define GPIO_OTYPE_PP          0
#define GPIO_OSPEED_2MHZ       0
#define RCC_TIM1               0
#define RST_TIM1               0
#define TIM_CR1_CKD_CK_INT     0
#define TIM_CR1_CMS_EDGE       0
#define TIM_CR1_DIR_UP         0
#define TIM_OC2N               0
#define TIM_OC1N               0
#define TIM_OCM_PWM1           0
#define USART2                 0
#define USART_STOPBITS_1       0
#define USART_MODE_TX          0
#define USART_PARITY_NONE      0
#define USART_FLOWCONTROL_NONE 0
#define RCC_USART2             0
#define GPIO_AF7               0
#define RCC_CLOCK_3V3_84MHZ    0
#define STK_CSR_CLKSOURCE_AHB  0
#define RCC_GPIOD              0
#define RCC_SYSCFG             0
#define NVIC_EXTI2_IRQ         0
#define EXTI2                  0
#define EXTI_TRIGGER_RISING    0
#define NVIC_EXTI4_IRQ         0
#define EXTI4                  0


#define TIM1                   1
#define TIM_OC2                2
#define TIM_OC1                1



#define I2C_SR1_SMBALERT    (1 << 15)
#define I2C_SR1_TIMEOUT     (1 << 14)
#define I2C_SR1_PECERR      (1 << 12)
#define I2C_SR1_OVR         (1 << 11)
#define I2C_SR1_AF          (1 << 10)
#define I2C_SR1_ARLO        (1 << 9)
#define I2C_SR1_BERR        (1 << 8)
#define I2C_SR1_TxE         (1 << 7)
#define I2C_SR1_RxNE        (1 << 6)
#define I2C_SR1_STOPF       (1 << 4)
#define I2C_SR1_ADD10       (1 << 3)
#define I2C_SR1_BTF         (1 << 2)
#define I2C_SR1_ADDR        (1 << 1)
#define I2C_SR1_SB          (1 << 0)
#define I2C_SR2_DUALF       (1 << 7)
#define I2C_SR2_SMBHOST     (1 << 6)
#define I2C_SR2_SMBDEFAULT  (1 << 5)
#define I2C_SR2_GENCALL     (1 << 4)
#define I2C_SR2_TRA         (1 << 2)
#define I2C_SR2_BUSY        (1 << 1)
#define I2C_SR2_MSL         (1 << 0)

extern const int* rcc_hsi_configs;

extern char I2C1_SR1;
extern char I2C1_SR2;

void nvic_enable_irq(int a);
void nvic_set_priority(int a, int b);


void exti_select_source(int a, int b);
void exti_set_trigger(int a, int b);
void exti_enable_request(int a);
void exti_reset_request(int a);
void exti2_isr(void);
void exti4_isr(void);

void rcc_clock_setup_pll(const int* a);
void rcc_periph_clock_enable(int a);
void rcc_periph_reset_pulse(int a);


void timer_set_mode(int a, int b,int c, int d);
void timer_set_prescaler(int a, int b);
void timer_set_repetition_counter(int a, int b);
void timer_continuous_mode(int a);
void timer_set_period(int a, int b);
void timer_disable_oc_output(int a, int b);
void timer_disable_oc_clear(int a, int b);
void timer_enable_oc_preload(int a, int b);
void timer_set_oc_slow_mode(int a, int b);
void timer_set_oc_mode(int a, int b, int c);
void timer_set_oc_polarity_high(int a, int b);
void timer_enable_oc_output(int a, int b);
void timer_enable_preload(int a);
void timer_enable_break_main_output(int a);
void timer_enable_counter(int a);
void timer_set_oc_value (uint32_t timer_peripheral, uint32_t oc_id, uint32_t value);

	
void gpio_mode_setup(int a, int b, int c, int d);
void gpio_set(int port,int pin);
void gpio_clear(int port,int pin);
void gpio_toggle(int port,int pin);
uint16_t gpio_get(uint32_t gpioport, uint16_t gpios);
void gpio_set_af(int a, int b, int c);
void gpio_set_output_options(int a, int b, int c, int d);


void usart_set_baudrate(int a, int b);
void usart_set_databits(int a, int b);
void usart_set_stopbits(int a, int b);
void usart_set_mode(int a, int b);
void usart_set_parity(int a, int b);
void usart_set_flow_control(int a, int b);
void usart_enable(int a);
void usart_send_blocking(uint32_t usart, uint16_t data);


void i2c_peripheral_disable(int a);
void i2c_set_speed(int a, int b, int c);
void i2c_enable_interrupt(int a, int b);
void i2c_set_own_7bit_slave_address(int a,int b);
void i2c_peripheral_enable(int a);
void i2c_enable_ack(int a);
void i2c_send_data(uint32_t i2c, uint8_t data);
uint8_t i2c_get_data(uint32_t i2c);
void i2c1_ev_isr(void);


void systick_set_clocksource(int a);
void systick_set_reload(int a);
void systick_clear();
void systick_counter_enable();
void systick_interrupt_enable();
uint32_t systick_get_value();
void sys_tick_handler();

int stm_main(void);

//*********************************************************************
// SIMULATION
//*********************************************************************


extern ledSim* simLed1;
extern ledSim* simLed2;
extern odometrieSim* odometrieGlobal;

void updateGPIO(int port,int pin);
void set_gpio_get(int port,int pin,bool value);