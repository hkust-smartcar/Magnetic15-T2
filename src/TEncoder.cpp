/*
 * TEncoder.cpp
 *
 *  Created on: 1 Mar, 2015
 *      Author: Travis
 */

#include <TEncoder.h>

TEncoder::TEncoder( const Config &config,
					uint16_t interval,
					float	wheelDiameter,
					int		countPerRevolution) :
	libsc::k60::AbEncoder::AbEncoder(config),
	wheelDiameter(wheelDiameter),
	countPerRevolution(countPerRevolution)
{
	updateInterval=interval;
}

float TEncoder::getCarSpeedByEncoderCount()
{
	Update();
	float distance=PI * wheelDiameter
					  * GetCount() / countPerRevolution;
	return distance/updateInterval*1000;
}

void TEncoder::setUpdateInterval(uint16_t interval)
{
	updateInterval=interval;
}
