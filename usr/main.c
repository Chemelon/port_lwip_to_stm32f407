#include "stm32f4xx.h"
#include "stm32f4x7_eth.h"
#include "delay.h"
#include "lan8720.h"
#include "usart.h"
#include "led.h"
#include <stdio.h>

int main(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    delay_init();
    TIM6_100us_it_init();
    usart3_init(115200);
    led_init();

    lan8720_gpio_init();
    lan8720_mac_init();
    for (int i = 0; i < 32; i++)
    {
        printf("%04x\r\n", ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, i));
    }
    printf("system inited!\r\n");
    while (1)
    {
        delay_ms(500);
        GPIO_ToggleBits(GPIOF, GPIO_Pin_8);
    };

}

uint32_t sys_now(void)
 {
     return HAL_GetTick();
 }


