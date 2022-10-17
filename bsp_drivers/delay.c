#include "delay.h"
#include "stm32f4xx.h"

static __IO uint32_t uwTick;

void delay_init(void)
{
    /*开启时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    /*10 KHz*/
    TIM6->PSC = (FCK_FREQ / CKCNT_FREQ - 1);
    TIM6->ARR = ARR_CNT;
    /*产生事件更新ARR寄存器*/
    TIM6->EGR = TIM_EGR_UG;
    /*清除UIF标志位*/
    TIM6->SR &= ~TIM_SR_UIF;
}


void delay_ms(uint32_t ms)
{
#if 1
    uint32_t tickstart = HAL_GetTick();
    uint32_t wait = ms;

    /* Add a freq to guarantee minimum wait */
    if (wait < 0xFFFFFFFFU)
    {
        wait += (uint32_t)(1);
    }

    while ((HAL_GetTick() - tickstart) < wait)
    {
    }
#else
    TIM6->CNT = 0;
    TIM6->CR1 |= TIM_CR1_CEN;
    while (!(TIM6->SR & TIM_SR_UIF));
    TIM6->CR1 &= ~TIM_CR1_CEN;
    TIM6->SR &= ~TIM_SR_UIF;
#endif
}
/*将TIM6配置成100us中断*/
void TIM6_it_init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;//
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;      //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器

    
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);//开启相关中断
    TIM6->CR1 |= TIM_CR1_CEN;
}

__weak void HAL_IncTick(void)
{
    uwTick += 1;
}

__weak uint32_t HAL_GetTick(void)
{
    return uwTick;
}
