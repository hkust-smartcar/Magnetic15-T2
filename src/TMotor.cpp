/*
 * TMotor.cpp
 *
 *  Created on: 28 Feb, 2015
 *      Author: Travis
 */

#include <TMotor.h>

TMotor::TMotor(const Config &config)
	:libsc::k60::DirMotor::DirMotor(config)
{

}

#if ADVANCED_SPEED_MONITOR
void	TMotor::setSpeed(float speedMS)
{

}

void	TMotor::setSpeed(uint16_t encoderCountS)
{

}
#endif
