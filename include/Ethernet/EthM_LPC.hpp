/*
 * EthM_LPC.hpp
 *
 *  Created on: 13 сен. 2017 г.
 *      Author: Stalker1290
 */

#ifndef ETHM_LPC_HPP_
#define ETHM_LPC_HPP_

#include "chip.h"
#include "Defines.h"
#include "Void_Ethernet.hpp"
#include "IRQ_Controller.hpp"
#include "Common_interfaces.hpp"

#pragma pack(push, 4)

typedef struct Eth_phy_TX_Queue_struct
{
	EthernetFrame Frames[Eth_phy_Tx_Queue_Len];
	uint16_t frame_size[Eth_phy_Tx_Queue_Len];
	ENET_TXDESC_T* BdTxPtr[Eth_phy_Tx_Queue_Len];
	uint16_t bw;
	uint16_t br;
}Eth_phy_TX_Queue_t;
#pragma pack(pop)

typedef enum
{
	Undefined = 0,
	LAN8720 = 1,
	DP83848M = 2,
}Eth_PHY_type_t;

class Eth_phy_t:public Ethernet_t,Callback_Interface_t,protected IRQ_LISTENER_t
{
public:
	static Eth_phy_t* get_Ethernet_phy(void);
	virtual void Initialize(void);
	virtual void Start(void);
	virtual void ResetDevice(void);
	virtual void GetParameter(uint8_t ParamName, void* ParamValue);
	virtual void SetParameter(uint8_t ParamName, void* ParamValue);
	virtual uint8_t Check_if_TX_queue_not_full(void);
	virtual void Add_to_TX_queue(EthernetFrame* mes,uint16_t m_size);
#ifdef USE_LWIP
	virtual void Add_to_TX_queue(struct pbuf* p);
#endif
	virtual uint16_t Pull_out_RX_Frame(EthernetFrame* Frame_ptr);
private:
	Eth_phy_t(void);
	static Eth_phy_t* Eth_phy_ptr;
	virtual void IRQ(int8_t IRQ_num);
	void Check_link(void);
	void SendHandler(void);
	void RecvHandler(void);
	void ErrorHandler(void);
	uint16_t lpc_rmii_read(uint8_t reg);
	void Init_Descriptors(void);
	void Init_Tx_Descriptors(void);
	void Init_Rx_Descriptors(void);
	virtual void void_callback(void* Intf_ptr, void* parameters);

	uint32_t Eth_DMA_errors_counter;
	uint8_t Adapter_name[9];
	static uint8_t EmacPsMAC[6];
	int Initialized;
	Eth_phy_TX_Queue_t TxQueue __attribute__ ((aligned(4)));
	Ethernet_speed_t speed;
	uint8_t Tx_Unlocked;
	uint32_t PHY_Status;
	ENET_TXDESC_T BdTx[Eth_phy_Tx_Queue_Len]__attribute__ ((aligned(4)));
	Eth_PHY_type_t PHY_type;

	ENET_RXDESC_T BdRx[Eth_phy_Rx_Queue_Len]__attribute__ ((aligned(4)));
	uint8_t RxBd_buf[Eth_phy_Rx_Queue_Len][Eth_Rx_desc_buf_size]__attribute__ ((aligned(4)));
	uint16_t Rx_Bd_index;
	uint8_t Out_Frame[EMAC_ETH_MAX_FLEN];
	uint16_t Out_Frame_size;
};


#endif /* ETHM_LPC_HPP_ */
