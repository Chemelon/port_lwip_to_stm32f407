#ifndef __DELAY_H
#define __DELAY_H
#include "stm32f4xx.h"
#define SYS_CLOCK_FREQ          150000000
#define FCK_FREQ                (SYS_CLOCK_FREQ / 2)
#define CKCNT_FREQ              10000
#define ARR_CNT                 (CKCNT_FREQ/1000)  

void delay_init(void);
void delay_ms(uint32_t ms);
void HAL_IncTick(void);
uint32_t HAL_GetTick(void);
void TIM6_100us_it_init(void);
    
#endif
