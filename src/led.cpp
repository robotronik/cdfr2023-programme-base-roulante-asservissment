#include "led.h"

#ifdef SIMULATION
    #include <hardware_interface.h>
#else
    #include <libopencm3/stm32/rcc.h>
    #include <libopencm3/stm32/gpio.h>
    #include <libopencm3/cm3/nvic.h>
#endif

#include "config.h"

void ledSetup(void){
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(port_RedLED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_RedLED);
	gpio_mode_setup(port_GreenLED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_GreenLED);
}


void RedLED_Clear(void){
    gpio_clear(port_RedLED, pin_RedLED);
}

void GreenLED_Clear(void){
    gpio_clear(port_GreenLED, pin_GreenLED);
}

void RedLED_Set(void){
    gpio_set(port_RedLED, pin_RedLED);
}


void GreenLED_Set(void){
    gpio_set(port_GreenLED, pin_GreenLED);
}


void RedLED_Toggle(void){
    gpio_toggle(port_RedLED, pin_RedLED);
}


void GreenLED_Toggle(void){
    gpio_toggle(port_GreenLED, pin_GreenLED);
}
