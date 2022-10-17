#include "stm32f4x7_eth.h"
#include "lan8720.h"
#include "lwipopts.h"

/* ETH_DMA使用的描述符*/
extern ETH_DMADESCTypeDef  DMARxDscrTab[ETH_RXBUFNB];/* Ethernet Rx MA Descriptor */
extern ETH_DMADESCTypeDef  DMATxDscrTab[ETH_TXBUFNB];/* Ethernet Tx DMA Descriptor */
extern uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE]; /* Ethernet Receive Buffer */
extern uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE]; /* Ethernet Transmit Buffer */
/* 在lan8720.c中定义 */
extern ETH_InitTypeDef ETH_InitStructure;

#ifdef ETH_link_callback
/**
  * @brief  Link callback function, this function is called on change of link status.
  * @param  The network interface
  * @retval None
  */
void ETH_link_callback(struct netif *netif)
{
    
    __IO uint32_t timeout = 0;
    uint32_t tmpreg, RegValue;
    struct ip4_addr_t ipaddr;
    struct ip4_addr_t netmask;
    struct ip4_addr_t gw;

    if (netif_is_link_up(netif))
    {
        /* Restart the autonegotiation */
        if (ETH_InitStructure.ETH_AutoNegotiation != ETH_AutoNegotiation_Disable)
        {
            /* Reset Timeout counter */
            timeout = 0;

            /* Enable auto-negotiation */
            ETH_WritePHYRegister(ETHERNET_PHY_ADDRESS, PHY_BCR, PHY_AutoNegotiation);

            /* Wait until the auto-negotiation will be completed */
            do
            {
                timeout++;
            }
            while (!(ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, PHY_BSR) & PHY_AutoNego_Complete) && (timeout < (uint32_t)PHY_READ_TO));

            /* Reset Timeout counter */
            timeout = 0;

            /* Read the result of the auto-negotiation */
            RegValue = ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, PHY_SR);

            /* Configure the MAC with the Duplex Mode fixed by the auto-negotiation process */
            if ((RegValue & PHY_DUPLEX_STATUS) != (uint32_t)RESET)
            {
                /* Set Ethernet duplex mode to Full-duplex following the auto-negotiation */
                ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;
            }
            else
            {
                /* Set Ethernet duplex mode to Half-duplex following the auto-negotiation */
                ETH_InitStructure.ETH_Mode = ETH_Mode_HalfDuplex;
            }
            /* Configure the MAC with the speed fixed by the auto-negotiation process */
            if (RegValue & PHY_SPEED_STATUS)
            {
                /* Set Ethernet speed to 10M following the auto-negotiation */
                ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
            }
            else
            {
                /* Set Ethernet speed to 100M following the auto-negotiation */
                ETH_InitStructure.ETH_Speed = ETH_Speed_100M;
            }

//      /* This is different for every PHY */
//          ETH_EXTERN_GetSpeedAndDuplex(ETHERNET_PHY_ADDRESS, &ETH_InitStructure);

            /*------------------------ ETHERNET MACCR Re-Configuration --------------------*/
            /* Get the ETHERNET MACCR value */
            tmpreg = ETH->MACCR;

            /* Set the FES bit according to ETH_Speed value */
            /* Set the DM bit according to ETH_Mode value */
            tmpreg |= (uint32_t)(ETH_InitStructure.ETH_Speed | ETH_InitStructure.ETH_Mode);

            /* Write to ETHERNET MACCR */
            ETH->MACCR = (uint32_t)tmpreg;

            _eth_delay_(ETH_REG_WRITE_DELAY);
            tmpreg = ETH->MACCR;
            ETH->MACCR = tmpreg;
        }

        /* Restart MAC interface */
        ETH_Start();

#ifdef USE_DHCP
        ipaddr.addr = 0;
        netmask.addr = 0;
        gw.addr = 0;
        DHCP_state = DHCP_START;
#else
        IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
        IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
        IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif /* USE_DHCP */

        netif_set_addr(&gnetif, &ipaddr, &netmask, &gw);

        /* When the netif is fully configured this function must be called.*/
        netif_set_up(&gnetif);

        EthLinkStatus = 0;
    }
    else
    {
        ETH_Stop();
#ifdef USE_DHCP
        DHCP_state = DHCP_LINK_DOWN;
        dhcp_stop(netif);
#endif /* USE_DHCP */

        /*  When the netif link is down this function must be called.*/
        netif_set_down(&gnetif);
    }
}
#endif

/*
 * 初始化STM32 ETH外设专用DMA的描述符
 *
 * Initialization for the MAC is as follows:
 *  1. Write to ETH_DMABMR to set STM32F4xx bus access parameters.（这个在lan8720_macdma_init函数中完成）
 *  2. Write to the ETH_DMAIER register to mask unnecessary interrupt causes.（复位之后默认关闭所有中断，所以可以先不配置）
 *  3. The software driver creates the transmit and receive descriptor lists. Then it writes to
 * both the ETH_DMARDLAR and ETH_DMATDLAR registers, providing the DMA with
 * the start address of each list.
 *  4. Write to MAC Registers 1, 2, and 3 to choose the desired filtering options.（这个在lan8720_macdma_init函数中完成）
 *  5. Write to the MAC ETH_MACCR register to configure and enable the transmit and
 * receive operating modes. The PS and DM bits are set based on the auto-negotiation
 * result (read from the PHY).
 *  6. Write to the ETH_DMAOMR register to set bits 13 and 1 and start transmission and(这一步在lwip的lowlevel_init中实现)
 * reception.
 *  7. The transmit and receive engines enter the running state and attempt to acquire
 * descriptors from the respective descriptor lists. The receive and transmit engines then
 * begin processing receive and transmit operations. The transmit and receive processes
 * are independent of each other and can be started or stopped separately.
 */
void eth_dma_desc_init(void)
{
    uint32_t RegValue = 0,timeout = 0,tmpreg = 0,i = 0;
    /* 完成上述步骤3 */
    /* Initialize Tx Descriptors list: Chain Mode */
    ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
    /* Initialize Rx Descriptors list: Chain Mode  */
    ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);
#ifdef CHECKSUM_BY_HARDWARE
    /* Enable the TCP, UDP and ICMP checksum insertion for the Tx frames */
    for (i = 0; i < ETH_TXBUFNB; i++)
    {
        ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i], ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
    }
#endif
    /* 步骤3结束 */
    /* 完成上述步骤5 */
    /* Restart the autonegotiation */
    if (ETH_InitStructure.ETH_AutoNegotiation != ETH_AutoNegotiation_Disable)
    {
        /* Reset Timeout counter */
        timeout = 0;

        /* Enable auto-negotiation */
        ETH_WritePHYRegister(ETHERNET_PHY_ADDRESS, PHY_BCR, PHY_AutoNegotiation);

        /* Wait until the auto-negotiation will be completed */
        do
        {
            timeout++;
        }
        while (!(ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, PHY_BSR) & PHY_AutoNego_Complete) && (timeout < (uint32_t)PHY_READ_TO));

        /* Reset Timeout counter */
        timeout = 0;

        /* Read the result of the auto-negotiation */
        RegValue = ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, PHY_SR);

        /* Configure the MAC with the Duplex Mode fixed by the auto-negotiation process */
        if ((RegValue & PHY_DUPLEX_STATUS) != (uint32_t)RESET)
        {
            /* Set Ethernet duplex mode to Full-duplex following the auto-negotiation */
            ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;
        }
        else
        {
            /* Set Ethernet duplex mode to Half-duplex following the auto-negotiation */
            ETH_InitStructure.ETH_Mode = ETH_Mode_HalfDuplex;
        }
        /* Configure the MAC with the speed fixed by the auto-negotiation process */
        if (RegValue & PHY_SPEED_STATUS)
        {
            /* Set Ethernet speed to 10M following the auto-negotiation */
            ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
        }
        else
        {
            /* Set Ethernet speed to 100M following the auto-negotiation */
            ETH_InitStructure.ETH_Speed = ETH_Speed_100M;
        }

        /* This is different for every PHY */
        //ETH_EXTERN_GetSpeedAndDuplex(ETHERNET_PHY_ADDRESS, &ETH_InitStructure);

        /*------------------------ ETHERNET MACCR Re-Configuration --------------------*/
        /* Get the ETHERNET MACCR value */
        tmpreg = ETH->MACCR;

        /* Set the FES bit according to ETH_Speed value */
        /* Set the DM bit according to ETH_Mode value */
        tmpreg |= (uint32_t)(ETH_InitStructure.ETH_Speed | ETH_InitStructure.ETH_Mode);

        /* Write to ETHERNET MACCR */
        ETH->MACCR = (uint32_t)tmpreg;

        _eth_delay_(ETH_REG_WRITE_DELAY);
        tmpreg = ETH->MACCR;
        ETH->MACCR = tmpreg;
    }
    /* 步骤5结束 */
}

