/**
 * @file
 * @brief USB Device Rom RNDIS Device Defines
 *
 *
 * @note
 * Copyright © 2019 Evgeniy Ivanov. Contacts: <strelok1290@gmail.com>
 * Copyright © 2015 Dean Camera (dean [at] fourwalledcubicle [dot] com)
 * All rights reserved.
 * @note
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 * @note
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @note
 * This file is a part of JB_Lib.
 */

#ifndef RNDIS_USB_DEVICE_ROM_DEFINES_H_
#define RNDIS_USB_DEVICE_ROM_DEFINES_H_

#include "jbopt.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Macros: */
/** Additional error code for RNDIS functions when a device returns a logical command failure. */
	#define RNDIS_ERROR_LOGICAL_CMD_FAILED        0x80

	/** Implemented RNDIS Version Major. */
	#define REMOTE_NDIS_VERSION_MAJOR             0x01

	/** Implemented RNDIS Version Minor. */
	#define REMOTE_NDIS_VERSION_MINOR             0x00

	/** \name RNDIS Message Values */
	//@{
	#define REMOTE_NDIS_PACKET_MSG                0x00000001UL
	#define REMOTE_NDIS_INITIALIZE_MSG            0x00000002UL
	#define REMOTE_NDIS_HALT_MSG                  0x00000003UL
	#define REMOTE_NDIS_QUERY_MSG                 0x00000004UL
	#define REMOTE_NDIS_SET_MSG                   0x00000005UL
	#define REMOTE_NDIS_RESET_MSG                 0x00000006UL
	#define REMOTE_NDIS_INDICATE_STATUS_MSG       0x00000007UL
	#define REMOTE_NDIS_KEEPALIVE_MSG             0x00000008UL
	//@}

	/** \name RNDIS Response Values */
	//@{
	#define REMOTE_NDIS_INITIALIZE_CMPLT          0x80000002UL
	#define REMOTE_NDIS_QUERY_CMPLT               0x80000004UL
	#define REMOTE_NDIS_SET_CMPLT                 0x80000005UL
	#define REMOTE_NDIS_RESET_CMPLT               0x80000006UL
	#define REMOTE_NDIS_KEEPALIVE_CMPLT           0x80000008UL
	//@}

	/** \name RNDIS Status Values */
	//@{
	#define REMOTE_NDIS_STATUS_SUCCESS            0x00000000UL
	#define REMOTE_NDIS_STATUS_FAILURE            0xC0000001UL
	#define REMOTE_NDIS_STATUS_INVALID_DATA       0xC0010015UL
	#define REMOTE_NDIS_STATUS_NOT_SUPPORTED      0xC00000BBUL
	#define REMOTE_NDIS_STATUS_MEDIA_CONNECT      0x4001000BUL
	#define REMOTE_NDIS_STATUS_MEDIA_DISCONNECT   0x4001000CUL
	//@}

	/** \name RNDIS Media States */
	//@{
	#define REMOTE_NDIS_MEDIA_STATE_CONNECTED     0x00000000UL
	#define REMOTE_NDIS_MEDIA_STATE_DISCONNECTED  0x00000001UL
	//@}

	/** \name RNDIS Media Types */
	//@{
	#define REMOTE_NDIS_MEDIUM_802_3              0x00000000UL
	//@}

	/** \name RNDIS Connection Types */
	//@{
	#define REMOTE_NDIS_DF_CONNECTIONLESS	      0x00000001UL
	#define REMOTE_NDIS_DF_CONNECTION_ORIENTED    0x00000002UL
	//@}

	/** \name RNDIS Packet Types */
	//@{
	#define REMOTE_NDIS_PACKET_DIRECTED           0x00000001UL
	#define REMOTE_NDIS_PACKET_MULTICAST          0x00000002UL
	#define REMOTE_NDIS_PACKET_ALL_MULTICAST      0x00000004UL
	#define REMOTE_NDIS_PACKET_BROADCAST          0x00000008UL
	#define REMOTE_NDIS_PACKET_SOURCE_ROUTING     0x00000010UL
	#define REMOTE_NDIS_PACKET_PROMISCUOUS        0x00000020UL
	#define REMOTE_NDIS_PACKET_SMT                0x00000040UL
	#define REMOTE_NDIS_PACKET_ALL_LOCAL          0x00000080UL
	#define REMOTE_NDIS_PACKET_GROUP              0x00001000UL
	#define REMOTE_NDIS_PACKET_ALL_FUNCTIONAL     0x00002000UL
	#define REMOTE_NDIS_PACKET_FUNCTIONAL         0x00004000UL
	#define REMOTE_NDIS_PACKET_MAC_FRAME          0x00008000UL
	//@}

	/** \name RNDIS OID Values */
	//@{
	#define OID_GEN_SUPPORTED_LIST                0x00010101UL
	#define OID_GEN_HARDWARE_STATUS               0x00010102UL
	#define OID_GEN_MEDIA_SUPPORTED               0x00010103UL
	#define OID_GEN_MEDIA_IN_USE                  0x00010104UL
	#define OID_GEN_MAXIMUM_FRAME_SIZE            0x00010106UL
	#define OID_GEN_MAXIMUM_TOTAL_SIZE            0x00010111UL
	#define OID_GEN_LINK_SPEED                    0x00010107UL
	#define OID_GEN_TRANSMIT_BLOCK_SIZE           0x0001010AUL
	#define OID_GEN_RECEIVE_BLOCK_SIZE            0x0001010BUL
	#define OID_GEN_VENDOR_ID                     0x0001010CUL
	#define OID_GEN_VENDOR_DESCRIPTION            0x0001010DUL
	#define OID_GEN_CURRENT_PACKET_FILTER         0x0001010EUL
	#define OID_GEN_MAXIMUM_TOTAL_SIZE            0x00010111UL
	#define OID_GEN_MEDIA_CONNECT_STATUS          0x00010114UL
	#define OID_GEN_PHYSICAL_MEDIUM               0x00010202UL
	#define OID_GEN_XMIT_OK                       0x00020101UL
	#define OID_GEN_RCV_OK                        0x00020102UL
	#define OID_GEN_XMIT_ERROR                    0x00020103UL
	#define OID_GEN_RCV_ERROR                     0x00020104UL
	#define OID_GEN_RCV_NO_BUFFER                 0x00020105UL
	#define OID_GEN_VENDOR_DRIVER_VERSION		  0x00010116UL
	#define OID_GEN_MAC_OPTIONS             	  0x00010113UL
	#define OID_GEN_PROTOCOL_OPTIONS         	  0x00010112UL
	#define OID_GEN_MAXIMUM_SEND_PACKETS      	  0x00010115UL
	#define OID_GEN_CURRENT_LOOKAHEAD    	      0x0001010FUL
	#define OID_802_3_PERMANENT_ADDRESS           0x01010101UL
	#define OID_802_3_CURRENT_ADDRESS             0x01010102UL
	#define OID_802_3_MULTICAST_LIST              0x01010103UL
	#define OID_802_3_MAXIMUM_LIST_SIZE           0x01010104UL
	#define OID_802_3_RCV_ERROR_ALIGNMENT         0x01020101UL
	#define OID_802_3_XMIT_ONE_COLLISION          0x01020102UL
	#define OID_802_3_XMIT_MORE_COLLISIONS        0x01020103UL
	#define OID_802_3_MAC_OPTIONS             	  0x01010105UL


	//@}

/* Enums: */
	/** Enum for the RNDIS class specific control requests that can be issued by the USB bus host. */
	enum RNDIS_ClassRequests_t
	{
		RNDIS_REQ_SendEncapsulatedCommand = 0x00, /**< RNDIS request to issue a host-to-device NDIS command. */
		RNDIS_REQ_GetEncapsulatedResponse = 0x01, /**< RNDIS request to issue a device-to-host NDIS response. */
	};

	/** Enum for the possible NDIS adapter states. */
	enum RNDIS_States_t
	{
		RNDIS_Uninitialized    = 0, /**< Adapter currently uninitialized. */
		RNDIS_Initialized      = 1, /**< Adapter currently initialized but not ready for data transfers. */
		RNDIS_Data_Initialized = 2, /**< Adapter currently initialized and ready for data transfers. */
	};

	/** Enum for the RNDIS class specific notification requests that can be issued by a RNDIS device to a host. */
	enum RNDIS_ClassNotifications_t
	{
		RNDIS_NOTIF_ResponseAvailable = 0x01, /**< Notification request value for a RNDIS Response Available notification. */
	};

	/** Enum for the NDIS hardware states. */
	enum NDIS_Hardware_Status_t
	{
		NDIS_HardwareStatus_Ready, /**< Hardware Ready to accept commands from the host. */
		NDIS_HardwareStatus_Initializing, /**< Hardware busy initializing. */
		NDIS_HardwareStatus_Reset, /**< Hardware reset. */
		NDIS_HardwareStatus_Closing, /**< Hardware currently closing. */
		NDIS_HardwareStatus_NotReady /**< Hardware not ready to accept commands from the host. */
	};

/* Type Defines: */
	/** \brief MAC Address Structure.
	 *
	 *  Type define for a physical MAC address of a device on a network.
	 */
	#pragma pack(push, 1)
	typedef struct
	{
		uint8_t Octets[6]; /**< Individual bytes of a MAC address */
	} MAC_Address_t;
	#pragma pack(pop)
	/** \brief RNDIS Common Message Header Structure.
	 *
	 *  Type define for a RNDIS message header, sent before RNDIS messages.
	 *
	 *  \note Regardless of CPU architecture, these values should be stored as little endian.
	 */
	#pragma pack(push, 1)
	typedef struct
	{
		uint32_t MessageType; /**< RNDIS message type, a \c REMOTE_NDIS_*_MSG constant */
		uint32_t MessageLength; /**< Total length of the RNDIS message, in bytes */
	} RNDIS_Message_Header_t;
	#pragma pack(pop)
	/** \brief RNDIS Message Structure.
	 *
	 *  Type define for a RNDIS packet message, used to encapsulate Ethernet packets sent to and from the adapter.
	 *
	 *  \note Regardless of CPU architecture, these values should be stored as little endian.
	 */
	#pragma pack(push, 1)
	typedef struct
	{
		uint32_t MessageType;
		uint32_t MessageLength;
		uint32_t DataOffset;
		uint32_t DataLength;
		uint32_t OOBDataOffset;
		uint32_t OOBDataLength;
		uint32_t NumOOBDataElements;
		uint32_t PerPacketInfoOffset;
		uint32_t PerPacketInfoLength;
		uint32_t VcHandle;
		uint32_t Reserved;
	} RNDIS_Packet_Message_t;
	#pragma pack(pop)
	/** \brief RNDIS Initialization Message Structure.
	 *
	 *  Type define for a RNDIS Initialize command message.
	 *
	 *  \note Regardless of CPU architecture, these values should be stored as little endian.
	 */
	#pragma pack(push, 1)
	typedef struct
	{
		uint32_t MessageType;
		uint32_t MessageLength;
		uint32_t RequestId;

		uint32_t MajorVersion;
		uint32_t MinorVersion;
		uint32_t MaxTransferSize;
	} RNDIS_Initialize_Message_t;
	#pragma pack(pop)
	/** \brief RNDIS Initialize Complete Message Structure.
	 *
	 *  Type define for a RNDIS Initialize Complete response message.
	 *
	 *  \note Regardless of CPU architecture, these values should be stored as little endian.
	 */
	#pragma pack(push, 1)
	typedef struct
	{
		uint32_t MessageType;
		uint32_t MessageLength;
		uint32_t RequestId;
		uint32_t Status;

		uint32_t MajorVersion;
		uint32_t MinorVersion;
		uint32_t DeviceFlags;
		uint32_t Medium;
		uint32_t MaxPacketsPerTransfer;
		uint32_t MaxTransferSize;
		uint32_t PacketAlignmentFactor;
		uint32_t AFListOffset;
		uint32_t AFListSize;
	} RNDIS_Initialize_Complete_t;
	#pragma pack(pop)
	/** \brief RNDIS Keep Alive Message Structure.
	 *
	 *  Type define for a RNDIS Keep Alive command message.
	 *
	 *  \note Regardless of CPU architecture, these values should be stored as little endian.
	 */
	#pragma pack(push, 1)
	typedef struct
	{
		uint32_t MessageType;
		uint32_t MessageLength;
		uint32_t RequestId;
	} RNDIS_KeepAlive_Message_t;
	#pragma pack(pop)
	/** \brief RNDIS Keep Alive Complete Message Structure.
	 *
	 *  Type define for a RNDIS Keep Alive Complete response message.
	 *
	 *  \note Regardless of CPU architecture, these values should be stored as little endian.
	 */
	#pragma pack(push, 1)
	typedef struct
	{
		uint32_t MessageType;
		uint32_t MessageLength;
		uint32_t RequestId;
		uint32_t Status;
	} RNDIS_KeepAlive_Complete_t;
	#pragma pack(pop)
	/** \brief RNDIS Reset Complete Message Structure.
	 *
	 *  Type define for a RNDIS Reset Complete response message.
	 *
	 *  \note Regardless of CPU architecture, these values should be stored as little endian.
	 */
	#pragma pack(push, 1)
	typedef struct
	{
		uint32_t MessageType;
		uint32_t MessageLength;
		uint32_t Status;

		uint32_t AddressingReset;
	} RNDIS_Reset_Complete_t;
	#pragma pack(pop)
	/** \brief RNDIS OID Property Set Message Structure.
	 *
	 *  Type define for a RNDIS OID Property Set command message.
	 *
	 *  \note Regardless of CPU architecture, these values should be stored as little endian.
	 */
	#pragma pack(push, 1)
	typedef struct
	{
		uint32_t MessageType;
		uint32_t MessageLength;
		uint32_t RequestId;

		uint32_t Oid;
		uint32_t InformationBufferLength;
		uint32_t InformationBufferOffset;
		uint32_t DeviceVcHandle;
	} RNDIS_Set_Message_t;
	#pragma pack(pop)
	/** \brief RNDIS OID Property Set Complete Message Structure.
	 *
	 *  Type define for a RNDIS OID Property Set Complete response message.
	 *
	 *  \note Regardless of CPU architecture, these values should be stored as little endian.
	 */
	#pragma pack(push, 1)
	typedef struct
	{
		uint32_t MessageType;
		uint32_t MessageLength;
		uint32_t RequestId;
		uint32_t Status;
	} RNDIS_Set_Complete_t;
	#pragma pack(pop)
	/** \brief RNDIS OID Property Query Message Structure.
	 *
	 *  Type define for a RNDIS OID Property Query command message.
	 *
	 *  \note Regardless of CPU architecture, these values should be stored as little endian.
	 */
	#pragma pack(push, 1)
	typedef struct
	{
		uint32_t MessageType;
		uint32_t MessageLength;
		uint32_t RequestId;

		uint32_t Oid;
		uint32_t InformationBufferLength;
		uint32_t InformationBufferOffset;
		uint32_t DeviceVcHandle;
	} RNDIS_Query_Message_t;
	#pragma pack(pop)

	/** \brief RNDIS OID Property Query Complete Message Structure.
	 *
	 *  Type define for a RNDIS OID Property Query Complete response message.
	 *
	 *  \note Regardless of CPU architecture, these values should be stored as little endian.
	 */
	#pragma pack(push, 1)
	typedef struct
	{
		uint32_t MessageType;
		uint32_t MessageLength;
		uint32_t RequestId;
		uint32_t Status;

		uint32_t InformationBufferLength;
		uint32_t InformationBufferOffset;
	}RNDIS_Query_Complete_t;
	#pragma pack(pop)

#pragma pack(push, 1)
	typedef struct
	{
		uint8_t  bmRequestType; /**< Type of the request. */
		uint8_t  bRequest; /**< Request command code. */
		uint16_t wValue; /**< wValue parameter of the request. */
		uint16_t wIndex; /**< wIndex parameter of the request. */
		uint16_t wLength; /**< Length of the data to transfer in bytes. */
	} MyUsbRequestHeader_t;
#pragma pack(pop)

#pragma pack(push, 4)
	typedef struct RNDIS_RX_Queue_struct
	{
		uint8_t packets[RNDIS_DEVICE_ROM_RX_QUEUE_SIZE][EMAC_ETH_MAX_FLEN +
												sizeof(RNDIS_Packet_Message_t)];
		uint8_t ownFlags[RNDIS_DEVICE_ROM_RX_QUEUE_SIZE]; //if 1 still hadn't read by main
		uint16_t bw = 0;
		uint16_t br = 0;
		uint32_t isBufferQueued = 0;
	}RndisRxQueue_t;

	typedef struct RNDIS_TX_Queue_struct
	{
		uint8_t packets[RNDIS_DEVICE_ROM_TX_QUEUE_SIZE][EMAC_ETH_MAX_FLEN +
												sizeof(RNDIS_Packet_Message_t)];
		uint8_t ownFlags[RNDIS_DEVICE_ROM_TX_QUEUE_SIZE];  //if 1 OWN by USB Stack
		uint16_t bw = 0;
		uint16_t br = 0;
		uint32_t isTxbusy = 0;
	}RndisTxQueue_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif /* RNDIS_USB_DEVICE_ROM_DEFINES_H_ */
