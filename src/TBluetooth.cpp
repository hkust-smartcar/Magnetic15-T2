/*
 * TBluetooth.cpp
 *
 *  Created on: 5 Mar, 2015
 *      Author: Travis
 */

#include "TBluetooth.h"
#include <cstdio>
TBluetooth::TBluetooth(const Config &config)
:JyMcuBt106(config){}

std::string TBluetooth::composeMessage(int index,float value,bool plain)
{
	char message[125];
	if(plain)
	{
		sprintf(message,"%f\n",value);
	}else
	{
		sprintf(message,"%c%i%c%f\n",protocol.SIGNAL
					,index,protocol.DELIM,value);
	}

	return message;
}
