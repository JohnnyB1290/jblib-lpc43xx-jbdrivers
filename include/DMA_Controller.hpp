/*
 * DMA_handler.hpp
 *
 *  Created on: 01.09.2017
 *      Author: Stalker1290
 */

#ifndef DMA_CONTROLLER_HPP_
#define DMA_CONTROLLER_HPP_


#include "chip.h"
#include "IRQ_Controller.hpp"
#include "Defines.h"

class DMA_Controller_t
{
public:
	static DMA_Controller_t* get_DMA_Controller(void);
	void Initialize(void);
	void DEinitialize(void);
private:
	uint8_t initialize;
	DMA_Controller_t(void);
	static DMA_Controller_t* DMA_Controller_ptr;
};


#endif /* DMA_CONTROLLER_HPP_ */
