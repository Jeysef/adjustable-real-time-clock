#include "stm8s_clk.h"
#include "stm8s_tim4.h"
// #include "stm8s_gpio.h"
// #include "stm8s.h"
// #include "stm8s_it.h"
// #include "stm8s_it.h"

#ifndef DELAY_H
#define DELAY_H

// /** Global tick **/
extern volatile uint32_t tick;

/** Functions **/
void TIM4_UPD_OVF_IRQHandler(void) INTERRUPT(23);
void init_ms(void);

void delay_ms(uint16_t ms);
void delay_us(uint16_t us);

uint16_t millis(void);
uint32_t micros(void);

#endif //DELAY_H
