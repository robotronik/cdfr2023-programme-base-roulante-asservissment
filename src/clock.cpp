#include "clock.h"
#include "uart.h"

void clock_setup() {
  rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_84MHZ]);

  systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
  // Interrupts each millisec
  systick_set_reload(SYSTICK_PERIOD - 1);
  // clear counter so it starts right away
  systick_clear();
  systick_counter_enable();
  systick_interrupt_enable();

}

volatile uint32_t systicks = 0;

void sys_tick_handler() {
  systicks++;
}


uint32_t get_uptime_ms() {
  return systicks;
}

uint32_t get_uptime_us() {
  return systicks*1000 + systick_get_value()/MICROS_SYSTICK_RATIO;
}

void delay_ms(uint32_t ms) {
  uint32_t count_max = systicks + ms;
  while(systicks < count_max) {}
}

// const uint32_t millisec = 1600;
// const uint32_t second = millisec*1000;
