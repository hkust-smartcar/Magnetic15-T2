/*
 * Module.cpp
 *
 *  Created on: 14 Mar, 2015
 *      Author: Travis
 */

#include <Module.h>

Module::Module(Car* car) {
	p_car=car;
	disable=false;
}

Module::~Module()
{
}

void Module::suspend()
{
	disable=true;
}

void Module::resume()
{
	disable=false;
}
void Module::run()
{
	if(disable)
	{
		loopWhileSuspension();
		return;
	}
}
