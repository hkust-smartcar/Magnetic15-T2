/*
 * tLed.h
 *
 *  Created on: 28 Feb, 2015
 *      Author: Travis
 */

#ifndef TLED_H_
#define TLED_H_

#include <libsc/k60/led.h>
#include <libsc/k60/System.h>
class TLed: public libsc::k60::Led {
public:
	typedef uint32_t TimerInt;

	TLed(const Led::Config &config);

	static Led::Config getLedConfig(int id)
	{
		Led::Config config;
		config.id=id;
		return config;
	}
	static int		s_count;
	const  int		ID;
	bool			enabled;
	TimerInt		blinkInterval;
	TimerInt		lastUpdateTime;

	void			c_setEnable(int id,bool flag);
	void			c_setBlinkInterval(int id, uint16_t interval);

};

#endif /* TLED_H_ */
