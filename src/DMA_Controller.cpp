/*
 * DMA_handler.cpp
 *
 *  Created on: 01.09.2017
 *      Author: Stalker1290
 */
// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "DMA_Controller.hpp"


DMA_Controller_t* DMA_Controller_t::DMA_Controller_ptr = (DMA_Controller_t*)NULL;

DMA_Controller_t* DMA_Controller_t::get_DMA_Controller(void)
{
	if(DMA_Controller_t::DMA_Controller_ptr == (DMA_Controller_t*)NULL)
		DMA_Controller_t::DMA_Controller_ptr = new DMA_Controller_t();
	return DMA_Controller_t::DMA_Controller_ptr;
}

DMA_Controller_t::DMA_Controller_t(void)
{
	this->initialize = 0;
}


void DMA_Controller_t::Initialize(void)
{
	#ifdef CORE_M4
	uint32_t prioritygroup = 0x00;
	#endif
	if(this->initialize == 0)
	{
		Chip_GPDMA_Init(LPC_GPDMA);
		#ifdef CORE_M4
		prioritygroup = NVIC_GetPriorityGrouping();
		NVIC_SetPriority(DMA_IRQn, NVIC_EncodePriority(prioritygroup, DMA_interrupt_priority, 0));
		#endif
		#ifdef CORE_M0
		NVIC_SetPriority(DMA_IRQn, DMA_interrupt_priority);
		#endif

		NVIC_ClearPendingIRQ(DMA_IRQn);
		NVIC_EnableIRQ(DMA_IRQn);
		this->initialize = 1;
	}
}

void DMA_Controller_t::DEinitialize(void)
{
	if(this->initialize == 1)
	{
		NVIC_DisableIRQ(DMA_IRQn);
		Chip_RGU_TriggerReset(RGU_DMA_RST);
		while (Chip_RGU_InReset(RGU_DMA_RST)) {}
		Chip_GPDMA_DeInit(LPC_GPDMA);
		this->initialize = 0;
	}
}
