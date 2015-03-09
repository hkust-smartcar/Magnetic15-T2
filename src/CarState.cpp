/*
 * CarState.cpp
 *
 *  Created on: 22 Jan, 2015
 *      Author: Travis
 */

#include "CarState.h"


CarState::CarState(){
	s_velocity=0;
	s_servoAngle=900;
#ifdef USE_BATTERY_METER
	s_batteryLevel=7;
#endif
#ifdef CAR_STATE_HANDLING
	lastUpdateTime=libsc::k60::System::Time();
	scheduler=SituationScheduler();
#endif
}
#ifdef CAR_STATE_HANDLING
CarState::SituationScheduler::
SituationScheduler(TimerInt interval)
{
	pushInterval=interval;
}
CarState::SituationScheduler::
Event::Event(Situation s,float distance)
{
	situation=s;
	distanceUponEventDetection=distance;
}

void CarState::completeTask()
{
	scheduler.situationQueue.pop();
}

void CarState::addTask(
		Situation situation,float distance)
{
	scheduler.situationQueue.emplace(situation,distance);
}

#if !ANDVANCED_SPEED_MONITOR
void CarState::addTask(Situation situation)
{
	scheduler.situationQueue.emplace(situation,-1);
}
CarState::SituationScheduler::Event CarState::getTask()
{
	return scheduler.situationQueue.front();
}
#endif

#endif


