/*
 * Ethernet_router.cpp
 *
 *  Created on: 19.10.2017
 *      Author: Stalker1290
 */

#include "Ethernet_router.hpp"
#include "CONTROLLER.hpp"


Ethernet_router_t* Ethernet_router_t::Router_ptr = (Ethernet_router_t*)NULL;
Ethernet_t* Ethernet_router_t::Interface_ptrs[ETH_ROUTER_NUM_OF_NETWORK_INTF];
Arp_updater_t* Ethernet_router_t::Arp_updater_ptrs[ETH_ROUTER_NUM_OF_NETWORK_INTF];
uint8_t Ethernet_router_t::interfaces_count = 0;
EthernetFrame Ethernet_router_t::In_Frame;
uint16_t Ethernet_router_t::In_Frame_size = 0;
Ethernet_listener_t* Ethernet_router_t::listeners[ETH_ROUTER_NUM_OF_NETWORK_INTF][ETH_ROUTER_NUM_OF_LISTENERS];
struct netif Ethernet_router_t::LWIP_netif[ETH_ROUTER_NUM_OF_NETWORK_INTF];
C_void_callback_t* Ethernet_router_t::LWIP_ms_call_interface_ptr;


Ethernet_router_t* Ethernet_router_t::get_Ethernet_router(void)
{
	if(Ethernet_router_t::Router_ptr == (Ethernet_router_t*)NULL) Ethernet_router_t::Router_ptr = new Ethernet_router_t();
	return Ethernet_router_t::Router_ptr;
}

Ethernet_router_t::Ethernet_router_t(void)
{
	for(uint8_t i=0; i<ETH_ROUTER_NUM_OF_NETWORK_INTF; i++)
	{
		for(uint8_t j=0; j< ETH_ROUTER_NUM_OF_LISTENERS; j++) Ethernet_router_t::listeners[i][j] = (Ethernet_listener_t*)NULL;
	}
	CONTROLLER_t::get_CONTROLLER()->Add_main_procedure(this);
	LWIP_DEBUGF(LWIP_DBG_ON, ("Starting LWIP Lib\n\r"));
	lwip_init();
	Ethernet_router_t::LWIP_ms_call_interface_ptr = new C_void_callback_t(Ethernet_router_t::LWIP_ms_timer_Handler);
	CONTROLLER_t::get_Time_Engine()->NRT_setEvent(Lowest_priority_delay, 1000, Ethernet_router_t::LWIP_ms_call_interface_ptr, NULL);
}

struct netif* Ethernet_router_t::get_LWIP_netif(Ethernet_t* Eth_interface_ptr)
{
	for(uint8_t i=0; i<ETH_ROUTER_NUM_OF_NETWORK_INTF; i++)
	{
		if(Ethernet_router_t::Interface_ptrs[i] == Eth_interface_ptr) return &Ethernet_router_t::LWIP_netif[i];
	}
	return (struct netif*)NULL;
}

void Ethernet_router_t::LWIP_ms_timer_Handler(void)
{
	LWIPTimer_Handler();
	CONTROLLER_t::get_Time_Engine()->NRT_setEvent(Lowest_priority_delay, 1000, Ethernet_router_t::LWIP_ms_call_interface_ptr, NULL);
}

void Ethernet_router_t::Add_Ethernet_interface(Ethernet_t* Eth_interface_ptr, uint8_t* IP, uint8_t* GW, uint8_t* NETMASK)
{
	ip_addr_t ipaddr, netmask, gw;

	Ethernet_router_t::Interface_ptrs[Ethernet_router_t::interfaces_count] = Eth_interface_ptr;
	Eth_interface_ptr->Initialize();
	Eth_interface_ptr->Start();
	Ethernet_router_t::Arp_updater_ptrs[Ethernet_router_t::interfaces_count] = new Arp_updater_t(Eth_interface_ptr);
	Ethernet_router_t::Arp_updater_ptrs[Ethernet_router_t::interfaces_count]->Add_ip_in_ip_table_for_arp(IP);
	/* Static IP assignment */
	IP4_ADDR(&ipaddr, IP[0], IP[1], IP[2], IP[3]);
	IP4_ADDR(&gw, GW[0], GW[1], GW[2], GW[3]);
	IP4_ADDR(&netmask, NETMASK[0], NETMASK[1], NETMASK[2], NETMASK[3]);
	char* Adapter_name;
	Eth_interface_ptr->GetParameter(name_param, (void*)&Adapter_name);
	LWIP_DEBUGF(LWIP_DBG_ON,("%s IP addr = %d.%d.%d.%d\n\r", Adapter_name,ip4_addr1(&ipaddr),ip4_addr2(&ipaddr), ip4_addr3(&ipaddr), ip4_addr4(&ipaddr)));
	LWIP_DEBUGF(LWIP_DBG_ON,("%s Gateway = %d.%d.%d.%d\n\r", Adapter_name,ip4_addr1(&gw),ip4_addr2(&gw), ip4_addr3(&gw), ip4_addr4(&gw)));
	LWIP_DEBUGF(LWIP_DBG_ON,("%s Netmask = %d.%d.%d.%d\n\r\n\r", Adapter_name,ip4_addr1(&netmask),ip4_addr2(&netmask), ip4_addr3(&netmask), ip4_addr4(&netmask)));

	netif_add(&Ethernet_router_t::LWIP_netif[Ethernet_router_t::interfaces_count],
				&ipaddr, &netmask, &gw, (void*)Eth_interface_ptr, lwip_ethernetif_init, ethernet_input);
	if(Ethernet_router_t::interfaces_count == 0) netif_set_default(&Ethernet_router_t::LWIP_netif[Ethernet_router_t::interfaces_count]);
	netif_set_up(&Ethernet_router_t::LWIP_netif[Ethernet_router_t::interfaces_count]);

	Ethernet_router_t::interfaces_count++;
	this->Add_Ethernet_listener(Ethernet_router_t::Arp_updater_ptrs[Ethernet_router_t::interfaces_count - 1],Eth_interface_ptr);
}


void Ethernet_router_t::void_callback(void* Intf_ptr, void* parameters)
{
	uint8_t link;
	if((CONTROLLER_t*)Intf_ptr == CONTROLLER_t::get_CONTROLLER())
	{
		if(Ethernet_router_t::interfaces_count == 0) return;
		for(uint8_t i = 0; i<Ethernet_router_t::interfaces_count; i++)
		{
			Ethernet_router_t::Interface_ptrs[i]->GetParameter(LINK_param,&link);
			if(link == 0) netif_set_link_down(&Ethernet_router_t::LWIP_netif[i]);
			else netif_set_link_up(&Ethernet_router_t::LWIP_netif[i]);

			Ethernet_router_t::In_Frame_size = Ethernet_router_t::Interface_ptrs[i]->Pull_out_RX_Frame(&Ethernet_router_t::In_Frame);
			if(Ethernet_router_t::In_Frame_size != 0)
			{
			//	if(i == 0) Ethernet_router_t::Interface_ptrs[1]->Add_to_TX_queue(&Ethernet_router_t::In_Frame,Ethernet_router_t::In_Frame_size);
			//	if(i == 1) Ethernet_router_t::Interface_ptrs[0]->Add_to_TX_queue(&Ethernet_router_t::In_Frame,Ethernet_router_t::In_Frame_size);
				
				lwip_ethernetif_input(&Ethernet_router_t::LWIP_netif[i],&Ethernet_router_t::In_Frame,Ethernet_router_t::In_Frame_size);
				for(uint8_t j=0; j<ETH_ROUTER_NUM_OF_LISTENERS; j++ )
				{
					if(Ethernet_router_t::listeners[i][j] == (Ethernet_listener_t*)NULL) break;
					else
					{
						Ethernet_router_t::listeners[i][j]->Parse_frame(&Ethernet_router_t::In_Frame,Ethernet_router_t::In_Frame_size,
								Ethernet_router_t::Interface_ptrs[i],NULL);
					}
				}
			}
		}
		/* LWIP timers - ARP, DHCP, TCP, etc. */
		sys_check_timeouts();
	}
}

Arp_updater_t* Ethernet_router_t::Get_ARP_Updater(Ethernet_t* Eth_interface_ptr)
{
	if(Ethernet_router_t::interfaces_count == 0) return (Arp_updater_t*)NULL;
	for(uint8_t i = 0; i<Ethernet_router_t::interfaces_count; i++)
	{
		if(Ethernet_router_t::Interface_ptrs[i] == (Ethernet_t*)Eth_interface_ptr) return Ethernet_router_t::Arp_updater_ptrs[i];
	}
	return NULL;
}

void Ethernet_router_t::Add_Ethernet_listener(Ethernet_listener_t* listener, Ethernet_t* Eth_interface_ptr)
{
	if(Ethernet_router_t::interfaces_count == 0) return;
	for(uint8_t i = 0; i<Ethernet_router_t::interfaces_count; i++)
	{
		if(Ethernet_router_t::Interface_ptrs[i] == (Ethernet_t*)Eth_interface_ptr)
		{
			for(uint8_t j = 0; j < ETH_ROUTER_NUM_OF_LISTENERS; j++)
			{
				if(Ethernet_router_t::listeners[i][j] == listener) break;
				if(Ethernet_router_t::listeners[i][j] == (Ethernet_listener_t*)NULL)
				{
					Ethernet_router_t::listeners[i][j] = listener;
					break;
				}
			}
			break;
		}
	}
}

void Ethernet_router_t::Add_Ethernet_listener(Ethernet_listener_t* listener)
{
	for(uint8_t i = 0; i<Ethernet_router_t::interfaces_count; i++)
	{
		this->Add_Ethernet_listener(listener,Ethernet_router_t::Interface_ptrs[i]);
	}
}

void Ethernet_router_t::Delete_Ethernet_listener(Ethernet_listener_t* listener, Ethernet_t* Eth_interface_ptr)
{
	uint32_t index = 0;

	if(Ethernet_router_t::interfaces_count == 0) return;

	for(uint8_t i = 0; i<Ethernet_router_t::interfaces_count; i++)
	{
		if(Ethernet_router_t::Interface_ptrs[i] == (Ethernet_t*)Eth_interface_ptr)
		{
			for(uint8_t j = 0; j < ETH_ROUTER_NUM_OF_LISTENERS; j++)
			{
				if(Ethernet_router_t::listeners[i][j] == listener) break;
				else index++;
			}
			if(index == (ETH_ROUTER_NUM_OF_LISTENERS-1))
			{
				if(Ethernet_router_t::listeners[i][index] == listener) Ethernet_router_t::listeners[i][index] = (Ethernet_listener_t*)NULL;
			}
			else
			{
				for(uint8_t j = index; j < (ETH_ROUTER_NUM_OF_LISTENERS-1); j++)
				{
					Ethernet_router_t::listeners[i][j] = Ethernet_router_t::listeners[i][j+1];
					if(Ethernet_router_t::listeners[i][j+1] == (Ethernet_listener_t*)NULL) break;
				}
			}
			break;
		}
	}
}

void Ethernet_router_t::Delete_Ethernet_listener(Ethernet_listener_t* listener)
{
	for(uint8_t i = 0; i<Ethernet_router_t::interfaces_count; i++)
	{
		this->Delete_Ethernet_listener(listener,Ethernet_router_t::Interface_ptrs[i]);
	}
}

