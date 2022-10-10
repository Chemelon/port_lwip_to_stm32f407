#ifndef     __LAN8720_H
#define     __LAN8720_H

//#include "lwip/ip_addr.h"

/*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0                    192
#define IP_ADDR1                    168
#define IP_ADDR2                    137
#define IP_ADDR3                    200

/*NETMASK*/
#define NETMASK_ADDR0               255
#define NETMASK_ADDR1               255
#define NETMASK_ADDR2               255
#define NETMASK_ADDR3                 0

/*Gateway Address*/
#define GW_ADDR0                    192
#define GW_ADDR1                    168
#define GW_ADDR2                    137
#define GW_ADDR3                      1
/* USER CODE END 0 */



#define ETHERNET_PHY_ADDRESS       0x00 /* Relative to STM324xG-EVAL Board */

/*
	ETH_MDIO -------------------------> PA2
	ETH_MDC --------------------------> PC1
	ETH_MII_RX_CLK/ETH_RMII_REF_CLK---> PA1
	ETH_MII_RX_DV/ETH_RMII_CRS_DV ----> PA7
	ETH_MII_RXD0/ETH_RMII_RXD0 -------> PC4
	ETH_MII_RXD1/ETH_RMII_RXD1 -------> PC5
	ETH_MII_TX_EN/ETH_RMII_TX_EN -----> PG11
	ETH_MII_TXD0/ETH_RMII_TXD0 -------> PG13
	ETH_MII_TXD1/ETH_RMII_TXD1 -------> PG14
																						*/
/* ETH_MDIO */
#define ETH_MDIO_GPIO_CLK               RCC_AHB1Periph_GPIOA
#define ETH_MDIO_PORT                   GPIOA
#define ETH_MDIO_PIN                    GPIO_Pin_2
#define ETH_MDIO_AF                     GPIO_AF_ETH
#define ETH_MDIO_SOURCE                 GPIO_PinSource2

/* ETH_MDC */
#define ETH_MDC_GPIO_CLK                RCC_AHB1Periph_GPIOC
#define ETH_MDC_PORT                    GPIOC
#define ETH_MDC_PIN                     GPIO_Pin_1
#define ETH_MDC_AF                      GPIO_AF_ETH
#define ETH_MDC_SOURCE                  GPIO_PinSource1

/* ETH_RMII_REF_CLK */
#define ETH_RMII_REF_CLK_GPIO_CLK       RCC_AHB1Periph_GPIOA
#define ETH_RMII_REF_CLK_PORT           GPIOA
#define ETH_RMII_REF_CLK_PIN            GPIO_Pin_1
#define ETH_RMII_REF_CLK_AF             GPIO_AF_ETH
#define ETH_RMII_REF_CLK_SOURCE         GPIO_PinSource1

/* ETH_RMII_CRS_DV */
#define ETH_RMII_CRS_DV_GPIO_CLK        RCC_AHB1Periph_GPIOA
#define ETH_RMII_CRS_DV_PORT            GPIOA
#define ETH_RMII_CRS_DV_PIN             GPIO_Pin_7
#define ETH_RMII_CRS_DV_AF              GPIO_AF_ETH
#define ETH_RMII_CRS_DV_SOURCE          GPIO_PinSource7

/* ETH_RMII_RXD0 */
#define ETH_RMII_RXD0_GPIO_CLK          RCC_AHB1Periph_GPIOC
#define ETH_RMII_RXD0_PORT              GPIOC
#define ETH_RMII_RXD0_PIN               GPIO_Pin_4
#define ETH_RMII_RXD0_AF                GPIO_AF_ETH
#define ETH_RMII_RXD0_SOURCE            GPIO_PinSource4

/* ETH_RMII_RXD1 */
#define ETH_RMII_RXD1_GPIO_CLK          RCC_AHB1Periph_GPIOC
#define ETH_RMII_RXD1_PORT              GPIOC
#define ETH_RMII_RXD1_PIN               GPIO_Pin_5
#define ETH_RMII_RXD1_AF                GPIO_AF_ETH
#define ETH_RMII_RXD1_SOURCE            GPIO_PinSource5

/* ETH_RMII_TX_EN */
#define ETH_RMII_TX_EN_GPIO_CLK         RCC_AHB1Periph_GPIOG
#define ETH_RMII_TX_EN_PORT             GPIOG
#define ETH_RMII_TX_EN_PIN              GPIO_Pin_11
#define ETH_RMII_TX_EN_AF               GPIO_AF_ETH
#define ETH_RMII_TX_EN_SOURCE           GPIO_PinSource11

/* ETH_RMII_TXD0 */
#define ETH_RMII_TXD0_GPIO_CLK          RCC_AHB1Periph_GPIOG
#define ETH_RMII_TXD0_PORT              GPIOG
#define ETH_RMII_TXD0_PIN               GPIO_Pin_13
#define ETH_RMII_TXD0_AF                GPIO_AF_ETH
#define ETH_RMII_TXD0_SOURCE            GPIO_PinSource13

/* ETH_RMII_TXD1 */
#define ETH_RMII_TXD1_GPIO_CLK          RCC_AHB1Periph_GPIOG
#define ETH_RMII_TXD1_PORT              GPIOG
#define ETH_RMII_TXD1_PIN               GPIO_Pin_14
#define ETH_RMII_TXD1_AF                GPIO_AF_ETH
#define ETH_RMII_TXD1_SOURCE            GPIO_PinSource14

void lan8720_gpio_init(void);
void lan8720_mac_init(void);
//void ETH_link_callback(struct netif *netif);

#endif
