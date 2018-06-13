 /*
 * main.c
 *
 *  Created on: 9 ���. 2017 �.
 *      Author: Stalker1290
 */

#include "Platform.h"

IPC_proto_t* IPC_proto;
USB_Devices_module_t* USB0_Devices_buf[3];
RNDIS_device_module_t* RNDIS_dev_usb0 = (RNDIS_device_module_t*)NULL;

void Initialize(void);

int main(void)
{
	Initialize();

	while(1)
	{
		CONTROLLER_t::get_CONTROLLER()->Do_main();
	}
}


void heap_size(void);
	
C_void_callback_t* heap_call_ptr = new C_void_callback_t(heap_size);


static uint32_t last_hep_size = 0;

void heap_size(void)
{
		void* ptr = NULL;
	for(uint32_t i = 0; i < 0xffffffff; i = i+4)
	{
		ptr = malloc(i);
		if(ptr == NULL)
		{
			if(last_hep_size > i) printf("Heap ALLOC %i\n\r", last_hep_size - i);
			if(last_hep_size < i) printf("Heap FREE %i\n\r", i - last_hep_size);
			printf("Current Heap size: %i\n\r\n\r",i);
			last_hep_size = i;
			break;
		}
		free(ptr);
	}
	Time_Engine_t::Get_Time_Engine()->NRT_setEvent(Medium_priority_delay, 5000000,heap_call_ptr,NULL );
}
	


void Initialize(void)
{
	uint8_t IP[4] = Default_IP;
	uint8_t GW[4] = Default_GW;
	uint8_t NETMASK[4] = Default_NETMASK;

	uint8_t Eth_phy_MAC[6] = Eth_phy_Default_MAC;
	uint8_t Eth_RNDIS_MAC[6] = Eth_RNDIS_Default_MAC;
	uint8_t Eth_RNDIS_Adapter_MAC[6] = Eth_RNDIS_Default_Adapter_MAC;

#ifdef USE_CONSOLE
	uint8_t Console_M4_IP[4] = CONSOLE_M4_MCAST_IP;
	uint16_t Console_M4_SPORT = CONSOLE_M4_UDP_SPORT;
	uint16_t Console_M4_DPORT = CONSOLE_M4_UDP_DPORT;
#endif
	CONTROLLER_t::get_CONTROLLER()->Initialize();

/********************************Console***************************************/
#ifdef USE_CONSOLE
	Void_Console_t* Console = CONTROLLER_t::get_Console(Console_Tx_buf_size);
	Kernel_control_t::get_Kernel_Control();
#endif
/******************************************************************************/
	printf("Start Johnny Bravo ZingerBoot ver 2!\n\r\n\r");
	printf("Checking Signature... ");
	Crypto_checker_t::Initialize();
	if(Crypto_checker_t::Check()) printf("Hardware is Valid! \n\r");
	else printf("WARNING!!! Hardware is  Invalid! \n\r");

	Parameters_module_t::Initialize();

	if(Parameters_module_t::Get_param((char*)"ETH_IP", IP, 4, true) == 0) Parameters_module_t::Set_param((char*)"ETH_IP",IP, 4);
	if(Parameters_module_t::Get_param((char*)"ETH_GW", GW, 4, true) == 0) Parameters_module_t::Set_param((char*)"ETH_GW",GW, 4);
	if(Parameters_module_t::Get_param((char*)"ETH_NETMASK", NETMASK, 4, true) == 0) Parameters_module_t::Set_param((char*)"ETH_NETMASK",NETMASK, 4);

	if(Parameters_module_t::Get_param((char*)"ETH_PHY_MAC", Eth_phy_MAC, 6, true) == 0) Parameters_module_t::Set_param((char*)"ETH_PHY_MAC",Eth_phy_MAC, 6);
	if(Parameters_module_t::Get_param((char*)"ETH_RNDIS_MAC", Eth_RNDIS_MAC, 6, true) == 0) Parameters_module_t::Set_param((char*)"ETH_RNDIS_MAC",Eth_RNDIS_MAC, 6);
	if(Parameters_module_t::Get_param((char*)"ETH_RNDIS_ADAPTER_MAC",Eth_RNDIS_Adapter_MAC, 6, true) == 0) Parameters_module_t::Set_param((char*)"ETH_RNDIS_ADAPTER_MAC",Eth_RNDIS_Adapter_MAC, 6);

#ifdef USE_CONSOLE
	if(Parameters_module_t::Get_param((char*)"CONSOLE_MCAST_IP", Console_M4_IP, 4, true) == 0) Parameters_module_t::Set_param((char*)"CONSOLE_MCAST_IP",Console_M4_IP, 4);

	char Temp_str[32];
	int temp_int = 0;
	if(Parameters_module_t::Get_param((char*)"CONSOLE_M4_SPORT", (uint8_t*)Temp_str, 32, false) == 0)
	{
		sprintf(Temp_str, "%i",Console_M4_SPORT);
		Parameters_module_t::Set_param((char*)"CONSOLE_M4_SPORT",Temp_str);
	}
	else 
	{
		sscanf(Temp_str, "%i",&temp_int);
		Console_M4_SPORT = temp_int;
	}
	
	if(Parameters_module_t::Get_param((char*)"CONSOLE_M4_DPORT", (uint8_t*)Temp_str, 32, false) == 0)
	{
		sprintf(Temp_str, "%i",Console_M4_DPORT);
		Parameters_module_t::Set_param((char*)"CONSOLE_M4_DPORT",Temp_str);
	}
	else
	{
		sscanf(Temp_str, "%i",&temp_int);
		Console_M4_DPORT = temp_int;
	}

#endif

/********************************USB0******************************************/
	RNDIS_dev_usb0 = new RNDIS_device_module_t();
	RNDIS_dev_usb0->USB_SetParameter(RNDIS_Adapter_MAC_param,Eth_RNDIS_Adapter_MAC);

	USB0_Devices_buf[0] = (USB_Devices_module_t*)RNDIS_dev_usb0;
	VCOM_Device_module_t* VCOM_dev_usb0 = new VCOM_Device_module_t();
	USB0_Devices_buf[1] = (USB_Devices_module_t*)VCOM_dev_usb0;
	CONTROLLER_t::get_USB0()->Initialize(USB0_Devices_buf,2);
/******************************************************************************/

/********************************Ethernet router*******************************/
	CONTROLLER_t::get_Ethernet_phy()->SetParameter(MAC_param, Eth_phy_MAC);
	RNDIS_dev_usb0->SetParameter(MAC_param, Eth_RNDIS_MAC);

	CONTROLLER_t::get_Ethernet_router()->Add_Ethernet_interface(CONTROLLER_t::get_Ethernet_phy(),IP,GW,NETMASK);
	CONTROLLER_t::get_Ethernet_router()->Add_Ethernet_interface(RNDIS_dev_usb0,IP,GW,NETMASK);
/************************************************************************/

/*******************************UDP**********************************************/
#ifdef USE_CONSOLE
	CONTROLLER_t::get_CONTROLLER()->_delay_ms(2000);

	UDP_Channel_t* UDP_Console_M4_ptr = new UDP_Channel_t(Console_M4_SPORT,
			Console_M4_IP, Console_M4_DPORT, CONTROLLER_t::get_Ethernet_phy()/*RNDIS_dev_usb0*/);

	UDP_Console_M4_ptr->Initialize(NULL,0,Console);
	Console->Set_output_channel(UDP_Console_M4_ptr);

//	UART_int_t* U0 = CONTROLLER_t::get_UART0(430800);
//	U0->Initialize(malloc,512,Console);
//	Console->Set_output_channel(U0);
	
#endif
/********************************************************************************/

/*******************************DHCP**********************************************/
	uint8_t DHCP_start_IP_last_byte = 2;
	uint8_t DHCP_IP_count = 1;

	if(Parameters_module_t::Get_param((char*)"DHCP_START_IP_LAST_BYTE", &DHCP_start_IP_last_byte, 1, true) == 0)
		Parameters_module_t::Set_param((char*)"DHCP_START_IP_LAST_BYTE", &DHCP_start_IP_last_byte, 1);
	if(Parameters_module_t::Get_param((char*)"DHCP_IP_COUNT", &DHCP_IP_count, 1, true) == 0)
		Parameters_module_t::Set_param((char*)"DHCP_IP_COUNT", &DHCP_IP_count, 1);

	DHCP_Server_t* DHCP_serv_ptr = new DHCP_Server_t(RNDIS_dev_usb0,DHCP_start_IP_last_byte, DHCP_IP_count);
/********************************************************************************/

/*******************************DNS**********************************************/
	DNS_server_t* DNS_serv_ptr = new DNS_server_t(RNDIS_dev_usb0);
/********************************************************************************/

/********************************************************************************/
	TCP_server_t* TCP_boot_ptr = new TCP_server_t(7);
	
	uint8_t Message[5] = {0x55,0xff,0xff,0xff,0xAA};
	uint8_t Message_mask[5] = {1,0,1,0,1};

	Interface_checker_t* Interface_checker_ptr = new Interface_checker_t(Message, Message_mask, 5);
	Interface_checker_ptr->Add_interface(TCP_boot_ptr);
	Interface_checker_ptr->Add_interface(CONTROLLER_t::get_UART0(115200));

	Zinger_bootloader_t::Get_Zinger_boot()->Initialize(Interface_checker_ptr);

	Web_interface_t::Initialize();

//	Time_Engine_t::Get_Time_Engine()->NRT_setEvent(Medium_priority_delay, 2000000,heap_call_ptr,NULL );
}
