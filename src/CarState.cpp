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
	//TODO The parameters should depend on the config.
#if STATE_HANDLING_ROUTINE == 2 || STATE_HANDLING_ROUTINE == 3
	processStateAfterTime=0;
#endif
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
#if STATE_HANDLING_ROUTINE == 1
	scheduler.situationQueue.emplace(situation,-1);
#endif
#if STATE_HANDLING_ROUTINE == 2
	scheduler.situationQueue.emplace(situation,
			scheduler.processStateAfterTime);
#endif
}
#if STATE_HANDLING_ROUTINE == 1
//TODO Why declaration not found when the above case is true?
CarState::SituationScheduler::Event CarState::getTask()
{
	return scheduler.situationQueue.front();
}
#endif
#if STATE_HANDLING_ROUTINE == 2
CarState::SituationScheduler::Schedule	CarState::getTask()
{
	return scheduler.situationQueue.front();
}
void CarState::SituationScheduler::
	processSensorState(std::list
			<MagneticSensor::ReadingState> list)
{

}
#endif
#if STATE_HANDLING_ROUTINE == 3
void CarState::SituationScheduler::processSensorState(
		std::list<MagneticSensor::ReadingState> list)
{
	int i=0;
	for(std::list<MagneticSensor::ReadingState>::iterator it
			=list.begin();it!=list.end();it++)
	{
		/* TODO complete this algorithm
		 * Here is the core of the 6th servo algorithm.
		 * Only several elements in the list will be used to determine the state.
		 * The selected state will then be pushed into the queue for further processing.
		 * (and so far the processing algorithm need not to be changed.)
		 */
	}
}

#endif
#endif
#endif


