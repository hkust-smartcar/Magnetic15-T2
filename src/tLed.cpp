/*
 * tLed.cpp
 *
 *  Created on: 28 Feb, 2015
 *      Author: Travis
 */

#include "tLed.h"
int TLed::s_count=0;
TLed::TLed(const Led::Config &config) :
	Led::Led(config),enabled(true),
	blinkInterval(20),
	ID(s_count),
	lastUpdateTime(libsc::k60::System::Time())
{
	s_count++;
}

void TLed::c_setEnable(int id,bool flag)
{
	if(ID==id)
	enabled=flag;
}

void TLed::c_setBlinkInterval(int id,uint16_t interval)
{
	if(ID==id)
	{
		blinkInterval=interval;
	}

}
