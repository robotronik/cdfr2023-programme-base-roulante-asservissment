#include "Motor.h"
#include "uart.h"
#include "clock.h"
#include "math.h"

// BLDC motor driver
// https://datasheet.datasheetarchive.com/originals/distributors/Datasheets-DGA5/483784.pdf

//local fonctions
static uint8_t current_channel_indx = 0;
static uint8_t adc_channels[] = {adc_channelA, adc_channelB, adc_channelC};
static uint16_t * adc_values_registers[] = {NULL, NULL, NULL};
void setuptimer(void);
void adc_setup(void);

Motor::Motor(int motorID)
{
	id = motorID;
	name = 'A' + motorID;
	adc_values_registers[id] = &adc_value;
}

void Motor::Setup(){
	setupGPIO();
	setuptimer();
	adc_setup();
}

void Motor::SetSpeedSigned(int speed){
	SetSpeedUnsigned(abs(speed), speed<0);
}

void Motor::SetSpeedUnsigned(int speed, bool reverse){
	SetDirection(reverse);
	SetSpeed(speed);
}

void Motor::SetDirection(bool reverse){
	if (reverse)
		gpio_set(port_Direction,pin_Direction);
	else
		gpio_clear(port_Direction,pin_Direction);
}

void Motor::Brake(bool brake){
	if (brake)
		gpio_clear(port_Brake,pin_Brake);	
	else
		gpio_set(port_Brake,pin_Brake);
}

/*
Sets the current-decay method. Referring to table 3, when
in slow-decay mode, MODE = 1, only the high-side MOSFET
is switched off during a PWM-off cycle. In the fast-decay mode,
MODE = 0, the device switches both the high-side and low-side
MOSFETs. 
*/
void Motor::SetMode(int motorMode){
	if (motorMode == 1) {
		gpio_set(port_Mode,pin_Mode);
		mode = 1;
	}
	else {
		gpio_clear(port_Mode,pin_Mode);
		mode = 0;
	}
}

void Motor::SetSpeed(int speed){
	speed = CLAMP(speed,0,maxSpeed);

	int pwm;
	if (mode == 0)
		pwm = speed/2 + 50;
	else
		pwm = speed;
		
	int pwmVal = pwm * COEFMULT;

	timer_set_oc_value(TIM1, TIM_OC2, pwmVal);
}
/*
void motorSetSpeed(int speed){
	if(speed<0){
		speed = 0;
	}
	else if(speed>100){
		speed = 100;
	}
	speed = CLAMP(speed,(maxSpeedR/2)-50,(maxSpeedR/2)+50);
	if(modeR==0){
		speed = speed/2 + 50;
	}
	timer_set_oc_value(TIM1, TIM_OC1, speed*COEFMULT);
}
*/

void Motor::Disable(){
	gpio_clear(port_Coast,pin_Coast);
	motorEn = false;
}
void Motor::Enable(){
	gpio_set(port_Coast,pin_Coast);
	motorEn = true;
}
void Motor::SetMaxSpeed(int max){
	maxSpeed = CLAMP(max,0,100);
}

void Motor::SetMaxTorque(int torque){
	maxTorque = (torque*4096)/100;
	if (maxTorque > 4095) {
		doesLimitTorque = false;
	}
	else {
		doesLimitTorque = true;
		adc_start_conversion_regular(ADC1);
	}
}

// Returns the current in A
double Motor::GetCurrent() {
	// ADC value between 0 and 4095
	// Resistor value = 0.018 Ohm
	// Vref = 3.3V

	// Convert ADC value to voltage
	double voltage = (double)(adc_value) / 4095.0 * 3.3;
	double CSOutVoltage = voltage * (4.7+2.2) / 2.2; // Voltage divider

	// VCSOUT ≈ (ILOAD × AV × RSENSE) + VOOS
	double current = (CSOutVoltage - 0.320) / (19 * 0.018);

    return current;
}

void Motor::PrintValues() {
    usartprintf(">ADC of %c: %4d /4095\n", adc_value);
    usartprintf(">Current of %c: %lf A\n", GetCurrent());
}

void Motor::setupGPIO(void){
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_ADC1);


	gpio_mode_setup(port_Reset, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_Reset);
	gpio_mode_setup(port_Coast, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_Coast);
	gpio_mode_setup(port_Mode, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_Mode);
	gpio_mode_setup(port_Direction, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_Direction);
	gpio_mode_setup(port_Brake, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_Brake);
	gpio_mode_setup(port_ESF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_ESF);

	gpio_mode_setup(port_Err1, GPIO_MODE_INPUT, GPIO_PUPD_NONE, pin_Err1);
	gpio_mode_setup(port_Err2, GPIO_MODE_INPUT, GPIO_PUPD_NONE, pin_Err2);

    /* Configure PA5 en entrée analogique */
    gpio_mode_setup(port_Tacho, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, pin_Tacho);

	SetMode(0);
	Brake(true);
	SetDirection(false);

	// disble reset = 1 (When RESET is low, all internal circuitry is disabled)
	gpio_set(port_Reset, pin_Reset);

	// coast disable = 1 (coast : reset only the gate)
	gpio_set(port_Coast, pin_Coast);
	Enable();

	// Disable ESF
	gpio_clear(port_ESF, pin_ESF);

	// setup PWM AF for the speed control
	gpio_mode_setup(port_SpeedControl, GPIO_MODE_AF, GPIO_PUPD_NONE, pin_SpeedControl);
	gpio_set_af(port_SpeedControl, GPIO_AF1, pin_SpeedControl);
	gpio_set_output_options(port_SpeedControl, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, pin_SpeedControl);
	SetSpeed(0);
}

void setuptimer(void){
	// Make sure this function is only called once.
	static bool first = false;
	if (first) return;
	first = true;

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
	timer_disable_oc_output(TIM1, TIM_OC3N);

	/* Configure global mode of line 3. */
	timer_disable_oc_clear(TIM1, TIM_OC2);
	timer_enable_oc_preload(TIM1, TIM_OC2);
	timer_set_oc_slow_mode(TIM1, TIM_OC2);
	timer_set_oc_mode(TIM1, TIM_OC2, TIM_OCM_PWM1);

	timer_disable_oc_clear(TIM1, TIM_OC1);
	timer_enable_oc_preload(TIM1, TIM_OC1);
	timer_set_oc_slow_mode(TIM1, TIM_OC1);
	timer_set_oc_mode(TIM1, TIM_OC1, TIM_OCM_PWM1);

	timer_disable_oc_clear(TIM1, TIM_OC3);
	timer_enable_oc_preload(TIM1, TIM_OC3);
	timer_set_oc_slow_mode(TIM1, TIM_OC3);
	timer_set_oc_mode(TIM1, TIM_OC3, TIM_OCM_PWM1);


	/* Configure OC3N. */
	timer_set_oc_polarity_high(TIM1, TIM_OC2N);
	timer_set_oc_polarity_high(TIM1, TIM_OC1N);
	timer_set_oc_polarity_high(TIM1, TIM_OC3N);

	/* Set the capture compare value to 0 when the robot booting */
	timer_set_oc_value(TIM1, TIM_OC2, 0);
	timer_set_oc_value(TIM1, TIM_OC1, 0);
	timer_set_oc_value(TIM1, TIM_OC3, 0);


	/* Reenable outputs. */
	timer_enable_oc_output(TIM1, TIM_OC2N);
	timer_enable_oc_output(TIM1, TIM_OC1N);
	timer_enable_oc_output(TIM1, TIM_OC3N);

	/* ARR reload enable. */
	timer_enable_preload(TIM1);

	/* Enable outputs in the break subsystem. */
	timer_enable_break_main_output(TIM1);

	/* Counter enable. */
	timer_enable_counter(TIM1);
}

void adc_setup(void) {
	// Make sure this function is only called once.
	static bool first = false;
	if (first) return;
	first = true;

    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_ADC1);

    gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1); // A - PA1 = ADC_Channel_1
    gpio_mode_setup(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO3); // B - PC3 = ADC_Channel_11
    gpio_mode_setup(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0); // C - PC0 = ADC_Channel_10


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
    adc_set_regular_sequence(ADC1, 1, adc_channels);
    adc_start_conversion_regular(ADC1);
}

// Fonction d'interruption pour l'ADC
void adc_isr(void) {
    if (adc_eoc(ADC1)) { // Vérifier si la conversion est terminée
		if (adc_values_registers[current_channel_indx] != NULL) {
			*adc_values_registers[current_channel_indx] = adc_read_regular(ADC1);
		}
        // Configurer le prochain canal et relancer la conversion
		current_channel_indx++;
		if (current_channel_indx >= sizeof(adc_channels)/sizeof(adc_channels[0])) 
			current_channel_indx = 0;
		
        adc_set_regular_sequence(ADC1, 1, adc_channels + current_channel_indx);
		adc_start_conversion_regular(ADC1);
    }
}