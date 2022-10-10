#ifndef __LED_H
#define __LED_H

#include "stm32f4xx.h"

#define LED1_PIN            GPIO_Pin_8
#define LED2_PIN            GPIO_Pin_9
#define LED3_PIN            GPIO_Pin_10

#define LED_ON(x)           GPIOF->BSRR = 1U << (x+8+16)    
#define LED_OFF(x)          GPIOF->BSRR = 1U << (x+8)

void led_init(void);

#endif


