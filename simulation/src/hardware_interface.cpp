#include "hardware_interface.h"

const int* rcc_hsi_configs = 0;

bool registreTab[16*4];

ledSim* simLed1 = nullptr;
ledSim* simLed2 = nullptr;
odometrieSim* odometrieGlobal = nullptr;

void nvic_enable_irq(int a){}
void nvic_set_priority(int a, int b){}


void exti_select_source(int a, int b){}
void exti_set_trigger(int a, int b){}
void exti_enable_request(int a){}
void exti_reset_request(int a){}

void rcc_clock_setup_pll(const int* a){}
void rcc_periph_clock_enable(int a){}
void rcc_periph_reset_pulse(int a){}


void timer_set_mode(int a, int b,int c, int d){}
void timer_set_prescaler(int a, int b){}
void timer_set_repetition_counter(int a, int b){}
void timer_continuous_mode(int a){}
void timer_set_period(int a, int b){}
void timer_disable_oc_output(int a, int b){}
void timer_disable_oc_clear(int a, int b){}
void timer_enable_oc_preload(int a, int b){}
void timer_set_oc_slow_mode(int a, int b){}
void timer_set_oc_mode(int a, int b, int c){}
void timer_set_oc_polarity_high(int a, int b){}
void timer_enable_oc_output(int a, int b){}
void timer_enable_preload(int a){}
void timer_enable_break_main_output(int a){}
void timer_enable_counter(int a){}
void timer_set_oc_value (uint32_t timer_peripheral, uint32_t oc_id, uint32_t value){
    if(timer_peripheral == TIM1){
        if(oc_id == TIM_OC1){
            odometrieGlobal->setLeftSpeed(value);
        }
        else if(oc_id == TIM_OC2){
            odometrieGlobal->setRightSpeed(value);
        }        
    }
}

	
void gpio_mode_setup(int a, int b, int c, int d){}
uint16_t gpio_get(uint32_t gpioport, uint16_t gpios){
    return registreTab[gpioport*16+gpios] ;
}
void gpio_set_af(int a, int b, int c){}
void gpio_set_output_options(int a, int b, int c, int d){}

void gpio_set(int port,int pin){
    registreTab[port*16+pin] = 1;
    updateGPIO(port,pin);    
}
void gpio_clear(int port,int pin){
    registreTab[port*16+pin] = 0;
    updateGPIO(port,pin);
}
void gpio_toggle(int port,int pin){
    registreTab[port*16+pin] = !registreTab[port*16+pin];
    updateGPIO(port,pin);
}

void updateGPIO(int port,int pin){
    if(port == port_led1 && pin == pin_led1){
        simLed1->ledSetStatus(registreTab[port*16+pin]);
    }
    else if(port == port_led2 && pin == pin_led2){
        simLed2->ledSetStatus(registreTab[port*16+pin]);
    }
    else if(port == port_directionR && pin == pin_directionR){
        odometrieGlobal->setLeftDirection(!registreTab[port*16+pin]);
    }
    else if(port == port_directionL && pin == pin_directionL){
        odometrieGlobal->setRightDirection(registreTab[port*16+pin]);
    }
}

void set_gpio_get(int port,int pin,bool value){
    registreTab[port*16+pin] = value;
}

void usart_set_baudrate(int a, int b){}
void usart_set_databits(int a, int b){}
void usart_set_stopbits(int a, int b){}
void usart_set_mode(int a, int b){}
void usart_set_parity(int a, int b){}
void usart_set_flow_control(int a, int b){}
void usart_enable(int a){}
void usart_send_blocking(uint32_t usart, uint16_t data){
    printf("%c",data);
}


void i2c_peripheral_disable(int a){}
void i2c_set_speed(int a, int b, int c){}
void i2c_enable_interrupt(int a, int b){}
void i2c_set_own_7bit_slave_address(int a,int b){}
void i2c_peripheral_enable(int a){}
void i2c_enable_ack(int a){}



void systick_set_clocksource(int a){}
void systick_set_reload(int a){}
void systick_clear(){}
void systick_counter_enable(){}
void systick_interrupt_enable(){}
uint32_t systick_get_value(){return 0;}