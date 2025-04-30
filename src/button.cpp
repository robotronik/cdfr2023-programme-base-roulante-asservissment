#include "button.h"

void buttonSetup(void){
    rcc_periph_clock_enable(RCC_GPIOC);
	gpio_mode_setup(port_PushButton, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, pin_PushButton);
	gpio_mode_setup(port_TestButton, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, pin_TestButton);
}

bool readPushButton(void){
    return gpio_get(port_PushButton, pin_PushButton);
}

bool readTestButton(void){
    return gpio_get(port_TestButton, pin_TestButton);
}
