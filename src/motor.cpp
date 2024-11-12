#include "motor.h"
#include "uart.h"
#include "clock.h"

//local variables
int modeL;
int modeR;
uint16_t adc_value_L = 0;
uint16_t adc_value_R = 0;
uint8_t current_channel = adc_channelL;
int maxTorqueR = 4096;
int maxTorqueL = 4096;
bool motorEnR = true;
bool motorEnL = true;
int maxSpeedR = 100;
int maxSpeedL = 100;
bool enableMaxTorque = false;

//local fonctions
void setuptimer(void);
void adc_setup(void);
void setupGPIO(void);
void motorSetSpeedL(int speed);
void motorSetSpeedR(int speed);


void motorSetup(void){
	setupGPIO();
	setuptimer();
	adc_setup();
}

//direction inverted for the left motor
void motorDirectionL(int direction){
	if(direction == 0){
		gpio_set(port_directionL,pin_directionL);
	}
	else{
		gpio_clear(port_directionL,pin_directionL);
	}
}

void motorSpeedSignedL(int speed){
	if(speed<1){
		motorDirectionL(1);
		motorSetSpeedL(-speed);
	}
	else{
		motorDirectionL(0);
		motorSetSpeedL(speed);
	}
}
	

void motorSpeedUnsignedL(int speed,int direction){
	if(direction == 1){
		motorDirectionL(1);
	}
	else if(direction == 0){
		motorDirectionL(0);
	}
	motorSetSpeedL(speed);
}


void motorDirectionR(int direction){
	if(direction == 0){
		gpio_clear(port_directionR,pin_directionR);
	}
	else{
		gpio_set(port_directionR,pin_directionR);
	}
}

void motorSpeedSignedR(int speed){
	if(speed<1){
		motorDirectionR(1);
		motorSetSpeedR(-speed);
	}
	else{
		motorDirectionR(0);
		motorSetSpeedR(speed);
	}
}
	

void motorSpeedUnsignedR(int speed,int direction){
	if(direction == 1){
		motorDirectionR(1);
	}
	else if(direction == 0){
		motorDirectionR(0);
	}
	motorSetSpeedR(speed);
	
}

void motorBrakeL(int brake){
	if(brake == 0){
		gpio_set(port_brakeL,pin_brakeL);
	}
	else{
		gpio_clear(port_brakeL,pin_brakeL);
	}
}

void motorBrakeR(int brake){
	if(brake == 0){
		gpio_set(port_brakeR,pin_brakeR);
	}
	else{
		gpio_clear(port_brakeR,pin_brakeR);
	}
}


void motorSetModeL(int mode){
	if(mode == 1){
		gpio_set(port_ModeL,pin_ModeL);
		modeL = 1;
	}
	else{
		gpio_clear(port_ModeL,pin_ModeL);
		modeL = 0;
	}
}
void motorSetModeR(int mode){
	if(mode == 1){
		gpio_set(port_ModeR,pin_ModeR);
		modeR = 1;
	}
	else{
		gpio_clear(port_ModeR,pin_ModeR);
		modeR = 0;
	}
}

void motorSetSpeedL(int speed){
	if(speed<0){
		speed = 0;
	}
	else if(speed>100){
		speed = 100;
	}
	speed = CLIP(speed,(maxSpeedL/2)-50,(maxSpeedL/2)+50);
	if(modeL==0){
		speed =	speed/2 + 50;
	}
	timer_set_oc_value(TIM1, TIM_OC2, speed*COEFMULT);
}
void motorSetSpeedR(int speed){
	if(speed<0){
		speed = 0;
	}
	else if(speed>100){
		speed = 100;
	}
	speed = CLIP(speed,(maxSpeedR/2)-50,(maxSpeedR/2)+50);
	if(modeR==0){
		speed = speed/2 + 50;
	}
	timer_set_oc_value(TIM1, TIM_OC1, speed*COEFMULT);
}

void disableMotor(void){
	gpio_clear(port_CoastL,pin_CoastL);
	gpio_clear(port_CoastR,pin_CoastR);
	motorEnR = false;
	motorEnL = false;
}
void enableMotor(void){
	gpio_set(port_CoastL,pin_CoastL);
	gpio_set(port_CoastR,pin_CoastR);
	motorEnR = true;
	motorEnL = true;
}

void setupGPIO(void){
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_ADC1);

    /* Configure PA5 en entrée analogique */
    gpio_mode_setup(port_infoSpeedL, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, pin_infoSpeedL);
	gpio_mode_setup(port_infoSpeedR, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, pin_infoSpeedR);
	
	gpio_mode_setup(port_ModeL, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_ModeL);
	gpio_mode_setup(port_brakeL, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_brakeL);
	gpio_mode_setup(port_directionL, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_directionL);
	gpio_mode_setup(port_resetL, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_resetL);
	gpio_mode_setup(port_CoastL, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_CoastL);
	gpio_mode_setup(port_esfL, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_esfL);
	gpio_mode_setup(port_info1L, GPIO_MODE_INPUT, GPIO_PUPD_NONE, pin_info1L);
	gpio_mode_setup(port_info2L, GPIO_MODE_INPUT, GPIO_PUPD_NONE, pin_info2L);

	gpio_mode_setup(port_ModeR, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_ModeR);
	gpio_mode_setup(port_brakeR, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_brakeR);
	gpio_mode_setup(port_directionR, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_directionR);
	gpio_mode_setup(port_resetR, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_resetR);
	gpio_mode_setup(port_CoastR, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_CoastR);
	gpio_mode_setup(port_esfR, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_esfR);
	gpio_mode_setup(port_info1R, GPIO_MODE_INPUT, GPIO_PUPD_NONE, pin_info1R);
	gpio_mode_setup(port_info2R, GPIO_MODE_INPUT, GPIO_PUPD_NONE, pin_info2R);


	//Select the mode
	gpio_set(port_ModeL,pin_ModeL);
	gpio_set(port_ModeR,pin_ModeR);
	modeL = 1;
	modeR = 1;
	//Brake disable = 1 (0 by default to brake the robot when booting)
	gpio_clear(port_brakeL,pin_brakeL);
	gpio_clear(port_brakeR,pin_brakeR);
	//Select Direction
	gpio_clear(port_directionL,pin_directionL);
	gpio_clear(port_directionR,pin_directionR);
	//disble reset = 1 (reset : reset all the chip)
	gpio_set(port_resetL,pin_resetL);
	gpio_set(port_resetR,pin_resetR);
	//coast disable = 1 (coast : reset only the gate)
	gpio_set(port_CoastL,pin_CoastL);
	gpio_set(port_CoastR,pin_CoastR);
	motorEnR = true;
	motorEnL = true;
	//esf = 1 (in case of fault, disable the driver)
	gpio_clear(port_esfL,pin_esfL);
	gpio_clear(port_esfR,pin_esfR);
	//setup AF for the speed control
	gpio_mode_setup(port_SpeedControlL, GPIO_MODE_AF, GPIO_PUPD_NONE, pin_SpeedControlL);
	gpio_mode_setup(port_SpeedControlR, GPIO_MODE_AF, GPIO_PUPD_NONE, pin_SpeedControlR);
	gpio_set_af(port_SpeedControlL, GPIO_AF1, pin_SpeedControlL);
	gpio_set_af(port_SpeedControlR, GPIO_AF1, pin_SpeedControlR);
	gpio_set_output_options(port_SpeedControlL, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, pin_SpeedControlL);
	gpio_set_output_options(port_SpeedControlR, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, pin_SpeedControlR);
}

void setuptimer(void){
	// Enable TIM1 clock.
	rcc_periph_clock_enable(RCC_TIM1);

	/* Reset TIM1 peripheral. */
	rcc_periph_reset_pulse(RST_TIM1);

	/* Timer global mode:
	 * - No divider
	 * - Alignment edge
	 * - Direction up
	 */
	timer_set_mode(TIM1, TIM_CR1_CKD_CK_INT,TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	/* Reset prescaler value. */
	timer_set_prescaler(TIM1, 0);

	/* Reset repetition counter value. */
	timer_set_repetition_counter(TIM1, 0);

	/* Enable preload. */
	timer_enable_preload(TIM1);

	/* Continuous mode. */
	timer_continuous_mode(TIM1);

	//depend of the value of the compnents on the PCB
	timer_set_period(TIM1, TIMERPERIOD);

	/* -- OC3 configuration -- */
	/* Disable outputs. */
	timer_disable_oc_output(TIM1, TIM_OC2N);
	timer_disable_oc_output(TIM1, TIM_OC1N);

	/* Configure global mode of line 3. */
	timer_disable_oc_clear(TIM1, TIM_OC2);
	timer_enable_oc_preload(TIM1, TIM_OC2);
	timer_set_oc_slow_mode(TIM1, TIM_OC2);
	timer_set_oc_mode(TIM1, TIM_OC2, TIM_OCM_PWM1);
	timer_disable_oc_clear(TIM1, TIM_OC1);
	timer_enable_oc_preload(TIM1, TIM_OC1);
	timer_set_oc_slow_mode(TIM1, TIM_OC1);
	timer_set_oc_mode(TIM1, TIM_OC1, TIM_OCM_PWM1);


	/* Configure OC3N. */
	timer_set_oc_polarity_high(TIM1, TIM_OC2N);
	timer_set_oc_polarity_high(TIM1, TIM_OC1N);

	/* Set the capture compare value to 0 when the robot booting */
	timer_set_oc_value(TIM1, TIM_OC2, 0);
	timer_set_oc_value(TIM1, TIM_OC1, 0);


	/* Reenable outputs. */
	timer_enable_oc_output(TIM1, TIM_OC2N);
	timer_enable_oc_output(TIM1, TIM_OC1N);

	/* ARR reload enable. */
	timer_enable_preload(TIM1);

	/* Enable outputs in the break subsystem. */
	timer_enable_break_main_output(TIM1);

	/* Counter enable. */
	timer_enable_counter(TIM1);
}




void adc_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_ADC1);

    gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1); // PA1 = ADC_Channel_2
    gpio_mode_setup(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO3); // PC3 = ADC_Channel_13


    adc_power_off(ADC1);
    adc_set_single_conversion_mode(ADC1);
    adc_disable_external_trigger_regular(ADC1);
    adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_480CYC);
    adc_enable_eoc_interrupt(ADC1);
    nvic_enable_irq(NVIC_ADC_IRQ);
	nvic_set_priority(NVIC_ADC_IRQ, 10);

    adc_power_on(ADC1);

    //delay_ms(1);

    // Démarrer la première conversion sur le canal 2
    adc_set_regular_sequence(ADC1, 1, &current_channel);
    adc_start_conversion_regular(ADC1);
}

// Fonction d'interruption pour l'ADC
void adc_isr(void) {
    if (adc_eoc(ADC1)) { // Vérifier si la conversion est terminée
        if (current_channel == adc_channelL) {
            adc_value_L = adc_read_regular(ADC1);
			if((adc_value_L > maxTorqueL) || motorEnL == false){
				gpio_clear(port_CoastL,pin_CoastL);
			}
			else{
				gpio_set(port_CoastL,pin_CoastL);
			}
            current_channel = adc_channelR;
        } else if (current_channel == adc_channelR) {
            adc_value_R = adc_read_regular(ADC1);
			if((adc_value_R > maxTorqueR) || motorEnR == false){
				gpio_clear(port_CoastR,pin_CoastR);
			}
			else{
				gpio_set(port_CoastR,pin_CoastR);
			}
            current_channel = adc_channelL;
        }
        // Configurer le prochain canal et relancer la conversion
        adc_set_regular_sequence(ADC1, 1, &current_channel);

        if(enableMaxTorque){
			adc_start_conversion_regular(ADC1);
		}
		else{
			gpio_set(port_CoastR,pin_CoastR);
			gpio_set(port_CoastL,pin_CoastL);
		}
    }
}

void setMaxSpeedR(int speed){
	maxSpeedR = speed;
}
void setMaxSpeedL(int speed){
	maxSpeedL = speed;
}

void setMaxTorqueR(int torque){
	maxTorqueR = (torque*4096)/100;
	if(maxTorqueR == 4096 && maxTorqueL == 4096){
		enableMaxTorque = false;
	}
	else{
		enableMaxTorque = true;
		adc_start_conversion_regular(ADC1);
	}
}

void setMaxTorqueL(int torque){
	maxTorqueL = (torque*4096)/100;
	if(maxTorqueR == 4096 && maxTorqueL == 4096){
		enableMaxTorque = false;
	}
	else{
		enableMaxTorque = true;
		adc_start_conversion_regular(ADC1);
	}
}

int getMotorCurrentL() {
    return adc_value_L;
}

int getMotorCurrentR() {
    return adc_value_R;
}

void prinAdcValue() {
    usartprintf(">courant_13 : %4d\n",adc_value_R);
	usartprintf(">courant_2 : %4d\n",adc_value_L);
}
