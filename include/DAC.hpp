/*
 * DAC.h
 *
 *  Created on: 14.02.2017
 *      Author: Stalker1290
 */

#ifndef DAC_HPP_
#define DAC_HPP_

#include "chip.h"

class DAC_t
{
public:
	static DAC_t& getDAC(void);
	void Initialize(void);
	void Start(void);
	void SetOut(uint32_t value);
	void DeInitialize(void);
private:
	DAC_t(void);
	DAC_t(DAC_t const&);
	DAC_t& operator=(DAC_t const&);
};


#endif /* DAC_HPP_ */
