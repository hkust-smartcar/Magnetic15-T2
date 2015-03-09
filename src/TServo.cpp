/*
 * TServo.cpp
 *
 *  Created on: 28 Feb, 2015
 *      Author: Travis
 */

#include <TServo.h>

TServo::TServo(const Config &config,uint16_t lowerBound,
		uint16_t upperBound,float multiplier)
:libsc::k60::TrsD05::TrsD05(config),
 lowerBound(lowerBound),upperBound(upperBound)
{
	angleMultiplier=multiplier;
}

void TServo::setAngleBySignalDifference(float dif)
{
	SetDegree(900-angleMultiplier*dif);
}

void TServo::SetDegree(uint16_t degree)
{
	uint16_t safeDegree=libutil::Clamp(lowerBound,degree,upperBound);
	Servo::SetDegree(safeDegree);
}
