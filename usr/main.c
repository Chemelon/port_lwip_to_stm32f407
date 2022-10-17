#include "stm32f4xx.h"
#include "stm32f4x7_eth.h"
#include "delay.h"
#include "lan8720.h"
#include "usart.h"
#include "led.h"
#include "eth.h"
#include <stdio.h>

#include <lwip/opt.h>
#include <lwip/arch.h>
#include "lwip/tcpip.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "ethernetif.h"
#include "netif/ethernet.h"
#include "lwip/def.h"
#include "lwip/stats.h"
#include "lwip/etharp.h"
#include "lwip/ip.h"
#include "lwip/snmp.h"
#include "lwip/timeouts.h"
#include "lwip/dhcp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cm_backtrace.h"
//#include "tcpclient.h"

#define HARDWARE_VERSION               "V1.0.0"
#define SOFTWARE_VERSION               "V0.1.0"


struct netif gnetif;
ip4_addr_t ipaddr;
ip4_addr_t netmask;
ip4_addr_t gw;
uint8_t IP_ADDRESS[4];
uint8_t NETMASK_ADDRESS[4];
uint8_t GATEWAY_ADDRESS[4];


void LwIP_Init(void)
{
    /* IP addresses initialization */

    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

    /* USER CODE END 0 */

    /* Initilialize the LwIP stack without RTOS */
    lwip_init();

    /* add the network interface (IPv4/IPv6) without RTOS */
    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

    /* Registers the default network interface */
    netif_set_default(&gnetif);

    if (netif_is_link_up(&gnetif))
    {
        /* When the netif is fully configured this function must be called */
        netif_set_up(&gnetif);
        printf("netif set linkup\r\n");
    }
    else
    {
        /* When the netif link is down this function must be called */
        netif_set_down(&gnetif);
        printf("netif set linkdown\r\n");
    }
    printf("本地IP地址是:%ld.%ld.%ld.%ld\r\n",  \
           ((gnetif.ip_addr.addr) & 0x000000ff),       \
           (((gnetif.ip_addr.addr) & 0x0000ff00) >> 8),  \
           (((gnetif.ip_addr.addr) & 0x00ff0000) >> 16), \
           ((gnetif.ip_addr.addr) & 0xff000000) >> 24);

}

void lwip_rx_task(void *parameter)
{
    for (;;)
    {
        //调用网卡接收函数
        if (ETH_CheckFrameReceived())
            ethernetif_input(&gnetif);
        //处理LwIP中定时事件
        sys_check_timeouts();
        vTaskDelay(5);
    }
}


void led_toggle_task(void *parameter)
{
    for (;;)
    {
        GPIO_ToggleBits(GPIOF, GPIO_Pin_8);
        vTaskDelay(500);
    }
}

int main(void)
{
    BaseType_t task_status;
    TaskHandle_t lwip_rx_task_handle, led_toggle_task_handle;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    cm_backtrace_init("CmBacktrace", HARDWARE_VERSION, SOFTWARE_VERSION);
    delay_init();
    TIM6_it_init();
    usart3_init(115200);
    led_init();

    /* 配置GPIO */
    lan8720_gpio_init();
    /* 配置ETH外设的mac 和 dma 并且设置mac_addr */
    lan8720_macdma_init();
    /* 配置ETH_DMA的描述符 接下来应该调用ETH_Start启动传输就可以了 */

    eth_dma_desc_init();

    LwIP_Init();

    for (int i = 0; i < 32; i++)
    {
        printf("%04x\r\n", ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, i));
    }
    printf("system inited!\r\n");

    task_status = xTaskCreate(lwip_rx_task, "lwip_rx_task", 100, NULL, 1, &lwip_rx_task_handle);
    task_status = xTaskCreate(led_toggle_task, "led_toggle_task", 100, NULL, 1, &led_toggle_task_handle);
    if (pdPASS == task_status)
        vTaskStartScheduler();
    while (1)
    {

    };

}

/*
 * 无操作系统时为LWIP提供定时器时基
 *
 */
u32_t sys_now(void)
{
    return HAL_GetTick();
}


