#include "led.h"

void ledSetup(void){
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(port_led1, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_led1);
	gpio_mode_setup(port_led2, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_led2);
}


void led1_clear(void){
    gpio_clear(port_led1,pin_led1);
}

void led2_clear(void){
    gpio_clear(port_led2,pin_led2);
}

void led1_set(void){
    gpio_set(port_led1,pin_led1);
}


void led2_set(void){
    gpio_set(port_led2,pin_led2);
}


void led1_toggle(void){
    gpio_toggle(port_led1,pin_led1);
}


void led2_toggle(void){
    gpio_toggle(port_led2,pin_led2);
}