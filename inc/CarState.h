/*
 * CarState.h
 *
 *  Created on: 22 Jan, 2015
 *      Author: Travis
 */

#ifndef SRC_CARSTATE_H_
#define SRC_CARSTATE_H_
#include <functional>
#include <queue>
#include <Config.h>
#include <libsc/k60/System.h>
//#define CAR_STATE_HANDLING 1
class CarState {
public:
	typedef 	uint16_t		TimerInt;
	CarState();

#ifdef USE_BATTERY_METER
	float		s_batteryLevel;
#endif
	float				s_velocity;
	float				s_servoAngle;
#ifdef	CAR_STATE_HANDLING
	enum		Situation
	{
			s_straightRoad=0,
			s_turn,
			s_rightAngle,
			s_crossRoad
	};
	class 		SituationScheduler
	{
		public:
			struct 		Event
			{
				public:
					Event(Situation s,float distance);
					Situation 		situation;
					float			distanceUponEventDetection;
			};
			SituationScheduler(TimerInt interval=0);
			std::queue<Event>		situationQueue;
			TimerInt				pushInterval;
	};

	TimerInt						lastUpdateTime;
	void							completeTask();
#if !ADVANCE_SPEED_MONITOR
	void							addTask(Situation situation);
#endif
	void							addTask(Situation situation
											,float distance);
	void							updateSituation();
	SituationScheduler::Event		getTask();

	SituationScheduler	scheduler;
#endif
};

#endif /* SRC_CARSTATE_H_ */
