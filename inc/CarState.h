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
#include <list>
#include <MagneticSensor.h>

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
			s_turnLeft,
			s_turnRight,
			s_rightAngleToLeft,
			s_rightAngleToRight,
			s_crossRoad,
			s_smallTurn,
			s_transition
	};
	class 		SituationScheduler
	{
		public:

			SituationScheduler(TimerInt interval=0);

			struct 		Event
			{
				public:
					Event(Situation s,float distance);
					Situation 		situation;
					float			distanceUponEventDetection;
			};
#if STATE_HANDLING_ROUTINE == 2 || STATE_HANDLING_ROUTINE == 3
			struct		Schedule
			{
				public:
					Schedule(Situation s,TimerInt interval)
					{
						situation=s;
						runAfter=interval+libsc::k60::System::Time();
					}
					Situation			situation;
					TimerInt			runAfter;
			};
			std::queue<Schedule>	situationQueue;
			TimerInt				processStateAfterTime;
#endif

#if STATE_HANDLING_ROUTINE == 1
			std::queue<Event>		situationQueue;
#endif
			TimerInt				pushInterval;
#if CALIBRATE_METHOD == 6
			/*
			 * the 6th algorithm requires the difference between any two sensors
			 * which has 4C2 = 6 combinations in total.
			 */
			float 					signalDifference[6];
			float					signalThreshold[6];

#endif
	};
#if CALIBRATE_METHOD == 6
	void					processSensorState(std::list<MagneticSensor::ReadingState> list);
#endif
	TimerInt						lastUpdateTime;
	void							completeTask();
#if !ADVANCE_SPEED_MONITOR
	void							addTask(Situation situation);
#endif
	void							addTask(Situation situation
											,float distance);
	void							updateSituation();
#if STATE_HANDLING_ROUTINE == 1
	SituationScheduler::Event		getTask();
#endif
#if STATE_HANDLING_ROUTINE >= 2
	SituationScheduler::Schedule	getTask();
#endif
	SituationScheduler	scheduler;
#endif
};

#endif /* SRC_CARSTATE_H_ */
