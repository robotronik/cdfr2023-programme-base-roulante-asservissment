#include "motor.h"

//local variables
int modeL;
int modeR;

//local fonctions
void setuptimer(void);
void setupGPIO(void);
void motorSetSpeedL(int speed);
void motorSetSpeedR(int speed);


void motorSetup(void){
	setupGPIO();
	setuptimer();
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
	if(modeL==0){
		speed =	speed/2 + 50;
	}
	timer_set_oc_value(TIM1, TIM_OC2, speed*COEFMULT);
}
void motorSetSpeedR(int speed){
	{
	if(speed<0){
		speed = 0;
	}
	else if(speed>100){
		speed = 100;
	}
	if(modeR==0){
		speed = speed/2 + 50;
	}
	timer_set_oc_value(TIM1, TIM_OC1, speed*COEFMULT);
}
}

void setupGPIO(void){
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	
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