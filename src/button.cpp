#include "button.h"

void buttonSetup(void){
    rcc_periph_clock_enable(RCC_GPIOC);
	gpio_mode_setup(port_endstopL, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, pin_endStopL);
	gpio_mode_setup(port_endStopR, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, pin_endStopR);
}

bool readButton1(void){
    return gpio_get(port_endstopL,pin_endStopL);
}

bool readButton2(void){
    return gpio_get(port_endStopR,pin_endStopR);
}
