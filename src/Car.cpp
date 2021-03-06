/*
 * Car.cpp
 *
 *  Created on: 22 Jan, 2015
 *      Author: Travis
 */

#include "Car.h"
#include "PID.h"


//AlternateMotor::Config getMotorConfig(uint8_t id)
//{
//	AlternateMotor::Config config;
//	config.id = id;
//	return config;
//}
#if DEBUG_MODE==1
	Gpo::Config getGpoConfig(Pin::Name pin
						,bool pullHigh=true)
	{
		Gpo::Config config;
		config.pin=pin;
		config.is_high=pullHigh;
		return config;
	}
#endif

DirMotor::Config getDirMotorConfig(uint8_t id)
{
	DirMotor::Config config;
	config.id=id;
	return config;
}
TrsD05::Config getServoConfig(uint8_t id)
{
	TrsD05::Config config;
	config.id = id;
	return config;
}

JyMcuBt106::Config getBtConfig(uint8_t id,uint8_t rxThreshold=2)
{
	JyMcuBt106::Config config;
	config.id = id;
	config.baud_rate = Uart::Config::BaudRate::k115200;
	config.rx_irq_threshold = rxThreshold;
	config.tx_buf_size=1;
	return config;
}

AbEncoder::Config getEncoderConfig(uint8_t id)
{
	AbEncoder::Config config;
	config.id = id;
	return config;
}


Mpu6050::Config getAccelConfig()
{
	Mpu6050::Config config;

	config.accel_range=Mpu6050::Config::Range::kSmall;
	config.gyro_range=Mpu6050::Config::Range::kSmall;
	return config;
}

#if USE_BATTERY_METER
		BatteryMeter::Config bconfig;
		config.voltage_ratio=this->config.c_voltmeterVoltageRatio;

#endif
Car* m_instance;
Car::Car()
//:
//		/*
//		 * Component initialization
//		 */
//		led({0}),
//		btLockInd({1}),
//		btTxInd({2}),
////		motor(getMotorConfig(0)),
//		servo(getServoConfig(0)),
//		bt(getBtCOnfig(0)),
//		//acc(getAccelConfig(0)),
////		encoder(getEncoderConfig(0)),
//		acc(getAccelConfig()),
//		lcdInterface(0),
//		mg{MagneticSensor(libbase::k60::Pin::Name::kPtc9),
////				MagneticSensor(libbase::k60::Pin::Name::kPtc10),
////				MagneticSensor(libbase::k60::Pin::Name::kPtc11),
//				MagneticSensor(libbase::k60::Pin::Name::kPtd1)},
//		menu(0),
//		dirMotor(getDirMotorConfig(0)),
//		filter(0.5,0.5,1,1)
:		m_motor(getDirMotorConfig(0)),
		m_bluetooth(getBtConfig(0,config.c_bluetoothRXThreshold)),
		m_encoder(getEncoderConfig(0),
					config.c_loopInterval,
					config.c_wheelDiameter,
					config.c_encoderCountPerRevolution),
#if DEBUG_MODE!=1
		m_servo(getServoConfig(0),
				config.c_servoAngleLowerBound,
				config.c_servoAngleUpperBound,
				config.c_servoAngleMultiplier),
#endif
		m_filter(config.c_kalmanFilterControlVariable[0],
			   config.c_kalmanFilterControlVariable[1],
			   1,1),
		currentTime(System::Time())
#if USE_BATTERY_METER
		,m_meter(config)
#endif
#if DEBUG_MODE==1
		,gpo(getGpoConfig(Pin::Name::kPta10))
#endif
{
		for(int i=0;i<config.c_ledCount;i++){
			if(config.c_ledCount>config.MAX_LED_COUNT
				||config.c_ledCount<=0)
			{
				assert(false);
				break;
			}
			l_led.push_back(TLed({i}));
			l_led.back().SetEnable(true);
		}

		for(int i=0;i<config.c_magneticSensorCount;i++){
			if(config.c_magneticSensorCount
					>config.MAX_MAGNETIC_SENSOR_COUNT)
			{
				assert(false);
				break;
			}
			Pin::Name pin;
			switch(i)
			{
				case 0:
					pin=Pin::Name::kPtc9;
					break;
				case 1:
					pin=Pin::Name::kPtc10;
					break;
				case 2:
					pin=Pin::Name::kPtc11;
					break;
				case 3:
					pin=Pin::Name::kPtd1;
					break;
			}
			l_magneticSensor.push_back(MagneticSensor(pin,m_filter));
			l_magneticSensor.back().setMin(
							config.c_magneticSensorLowerBound);
			l_magneticSensor.back().setMax(
							config.c_magneticSensorUpperBound);
			l_magneticSensor.back().setTriggerThreshold(
							config.c_magneticSensorTriggerThreshold);
		}

		m_motor.SetPower(config.c_motorPower);
		m_motor.SetClockwise(config.c_motorRotateClockwise);
		for(int i=0;i<3;i++)
		{
			PID<uint16_t>::Controller c=(PID<uint16_t>::Controller)i;
			m_motor.pid.setControlValue(
					c,
					config.c_motorPIDControlVariable[i]);
		}
		m_motor.pid.setAutomatEnable(
				config.c_motorPIDUseAutomat);


		m_bluetooth.EnableRx(&btListener);
#if DEBUG_MODE!=1
		m_servo.SetDegree(config.c_servoAngle);
#endif

		if(config.c_useKalmanFilter)
		{
			m_filter=KF(config.c_kalmanFilterControlVariable[0],
					 config.c_kalmanFilterControlVariable[1],
					 1,1);
		}else
		{
			m_filter=KF(0,0,1,1);
		}

		m_instance=this;
#ifdef USE_LCD
		for(int i=0;i<LcdInterface::Menu::MAX_OPTION;i++){
			menu.option[i].text="Option";
			menu.option[i].text+=i;
			menu.option[i].action=0;
		}
#endif
	}

void Car::run()
{
	/*
	 * initial setup and declare variables used throughout the loop;
	 */
#ifdef USE_LCD
	lcdInterface.useMenu(&menu);
#endif
#ifdef DEBUG_MODE
	//	And sensors calibration
#if HIGH_LEVEL_INSTR
		CALIBRATE_SENSORS();
#endif

#endif
	/*
	 * Do not modify the following code in any circumstances.
	 */
	while(true){
		if(System::Time()-currentTime<config.c_loopInterval)
		{
			continue;
		}
		if(config.c_halt)
		{
			r_restoreBlock();
			continue;
		}
#if VERSION == 1L
		r_encoderRoutine();
		r_ledRoutine();
		r_accelerometerRoutine();
		r_magneticSensorRoutine();
		r_bluetoothRoutine();
#if !CAR_STATE_HANDLING

		r_servoRoutine();
		r_motorRoutine();
#else
		r_stateHandlingRoutine();
#endif
#endif
#if VERSION >= 2L
		for(std::list<Module>::iterator i=l_modules.begin();
				i!=l_modules.end();i++)
		{
			i->run();
		}
#endif
		currentTime=System::Time();
	}
}


void Car::btListener(const Byte* byte, const size_t size){

	switch(*byte){
//	case 'r':
//		m_instance->btLock=!m_instance->btLock;
//		m_instance->bt.SendStr("BT: I get something!");
//		break;
//	case 'u':
//		if(m_instance->dirMotor.GetPower()==0){
//			m_instance->dirMotor.SetPower(400);
//		}else{
//			m_instance->dirMotor.SetPower(0);
//		}
//		break;
//	case 'z':
//			m_instance->servo.SetDegree(450);
//			break;
//	case 'x':
//			m_instance->servo.SetDegree(1350);
//			break;
	switch(*byte)
	{
	case 'a':
		if(m_instance->config.c_kalmanFilterControlVariable[0]<0.95)
			m_instance->config.c_kalmanFilterControlVariable[0]+=0.05;
		break;
	case 'z':
		if(m_instance->config.c_kalmanFilterControlVariable[0]>0.05)
			m_instance->config.c_kalmanFilterControlVariable[0]-=0.05;
		break;
	case 's':
		if(m_instance->config.c_kalmanFilterControlVariable[1]<0.95)
				m_instance->config.c_kalmanFilterControlVariable[1]+=0.05;
				break;
	case 'x':
		if(m_instance->config.c_kalmanFilterControlVariable[1]>0.05)
			m_instance->config.c_kalmanFilterControlVariable[1]-=0.05;
				break;
	case 'q':
		if(m_instance->m_motor.GetPower()!=0)
		{
			m_instance->m_motor.SetPower(0);
		}else
		{
			m_instance->m_motor.SetPower(m_instance->config.c_motorPower);
		}
		break;
	}
	}
}
void Car::r_ledRoutine()
{
	if(config.c_halt)return;
	for(std::list<TLed>::iterator i=l_led.begin();i!=l_led.end();i++)
	{
		if(i->blinkInterval>=System::Time()-i->lastUpdateTime
				&&i->enabled)
		{
			i->Switch();
			i->lastUpdateTime=System::Time();
		}
#if	USE_BATTERY_METER
		if(m_meter.GetVoltage()<5.0&&i==l_led.end())
		{
			i->SetEnable(true);
		}

#endif
	}
}

void Car::r_encoderRoutine()
{
	if(config.c_halt)return;
	state.s_velocity=m_encoder.getCarSpeedByEncoderCount();
}

void Car::r_accelerometerRoutine()
{
	if(config.c_halt)return;
}

void Car::r_magneticSensorRoutine()
{
	if(config.c_halt)return;
	MagneticSensor::ReadingState state[4];
	int i=0;
	for(MgItr it=l_magneticSensor.begin();
			it!=l_magneticSensor.end();it++)
	{
		it->updateReading();
		if(config.c_useKalmanFilter)
		{
			it->getFilteredReading(m_filter);
		}
		else
		{
			it->getReading();
//			state[i]=it->getState();
		}
		i++;
	}
#if SENSOR_PUSH_STATE_ALG == 1
	if(state[0]==MagneticSensor::ReadingState::IN_REFERENCE_STATE &&
	   state[1]==MagneticSensor::ReadingState::IN_REFERENCE_STATE &&
	   state[2]==MagneticSensor::ReadingState::IN_REFERENCE_STATE &&
	   state[3]==MagneticSensor::ReadingState::IN_REFERENCE_STATE)
	{
		this->state.addTask(CarState::Situation::s_straightRoad);
		return;
	}
	if(state[0]==MagneticSensor::ReadingState::IN_REFERENCE_STATE &&
	   state[1]==MagneticSensor::ReadingState::IN_REFERENCE_STATE &&
	   state[2]<=MagneticSensor::ReadingState::BELOW_REFERENCE_STATE &&
	   state[3]==MagneticSensor::ReadingState::IN_REFERENCE_STATE)
	{
		this->state.addTask(CarState::Situation::s_turnRight);
		return;
	}
	if(state[0]==MagneticSensor::ReadingState::IN_REFERENCE_STATE &&
	   state[1]<=MagneticSensor::ReadingState::BELOW_REFERENCE_STATE &&
	   state[2]==MagneticSensor::ReadingState::IN_REFERENCE_STATE &&
	   state[3]==MagneticSensor::ReadingState::IN_REFERENCE_STATE)
	{
		this->state.addTask(CarState::Situation::s_turnLeft);
		return;
	}
	this->state.addTask(CarState::Situation::s_straightRoad);

#endif
#if		SENSOR_PUSH_STATE_ALG == 2

#endif
}
#if CAR_STATE_HANDLING
void Car::r_stateHandlingRoutine()
{
#if STATE_HANDLING_ROUTINE ==1
	if(state.scheduler.situationQueue.size()!=0)
	{
		CarState::SituationScheduler::Event event
				=state.getTask();

		if(event.distanceUponEventDetection==1)		//The encoder is not available
		{
			if(System::Time()>state.lastUpdateTime)
			{

			}
		}
		state.completeTask();
	}
#endif
#if STATE_HANDLING_ROUTINE == 2
	if(state.scheduler.situationQueue.size()==0)return;
	while(state.getTask().runAfter<=System::Time())
	{
		CarState::Situation situation=state.getTask().situation;
		switch(situation)
		{
		/*
		 * TODO the following may not represent all the cases.
		 * 		e.g. when the car comes to a right-angled turn,
		 * 		the 4 readings change rapidly and this may lead
		 * 		to other cases.
		 * TODO try to apply fuzzy logic here
		 */
			case CarState::Situation::s_straightRoad:
			case CarState::Situation::s_crossRoad:
				//optimal case: works like straight line
			case CarState::Situation::s_smallTurn:
				config.c_servoAngle=900;
				break;
			case CarState::Situation::s_turnLeft:
				config.c_servoAngle=650;
				break;
			case CarState::Situation::s_turnRight:
				config.c_servoAngle=1250;
				break;
			case CarState::Situation::s_rightAngleToLeft:
				config.c_servoAngle=450;
				break;
			case CarState::Situation::s_rightAngleToRight:
				config.c_servoAngle=1350;
				break;
				//remain previous state
			case CarState::Situation::s_transition:
				break;
		}
		conformProtocol();
		state.completeTask();
	}
#endif
#if STATE_HANDLING_ROUTINE == 3
	state.processSensorState(getDifferenceState());
			CarState::Situation situation=state.getTask().situation;
			switch(situation)
			{
			/*
			 * TODO the following may not represent all the cases.
			 * 		e.g. when the car comes to a right-angled turn,
			 * 		the 4 readings change rapidly and this may lead
			 * 		to other cases.
			 * TODO try to apply fuzzy logic here
			 */
				case CarState::Situation::s_straightRoad:
				case CarState::Situation::s_crossRoad:
					//optimal case: works like straight line
				case CarState::Situation::s_smallTurn:
					config.c_servoAngle=900;
					break;
				case CarState::Situation::s_turnLeft:
					config.c_servoAngle=650;
					break;
				case CarState::Situation::s_turnRight:
					config.c_servoAngle=1250;
					break;
				case CarState::Situation::s_rightAngleToLeft:
					config.c_servoAngle=450;
					break;
				case CarState::Situation::s_rightAngleToRight:
					config.c_servoAngle=1350;
					break;
					//remain previous state
				case CarState::Situation::s_transition:
					break;
			}
			conformProtocol();
			state.completeTask();
#endif
}
#endif
#if DEBUG_MODE!=1
void Car::r_servoRoutine()
{
	if(config.c_halt)return;
	if(l_magneticSensor.size()%2)return;
	if(l_magneticSensor.size()==0)return;
#endif
#if USE_SERVO_ALG==0
		/*
		 * This is almost equivalent to manually control the servo.
		 */
		m_servo.SetDegree(config.c_servoAngle);

#endif
#if USE_SERVO_ALG==1
		/*
		 * Simple algorithm to determine servo angle by comapring
		 * the difference between the reading of left and right sensors
		 */
	float LR=0,RR=0,LRF=0,RRF=0;
	MgItr it=l_magneticSensor.begin();
	for(int i=0;i<l_magneticSensor.size();i++)
	{
		if(i<l_magneticSensor.size()/2)
		{
			LR+=it->filteredReading;
			LRF+=it->referenceReading;
		}else
		{
			RR+=it->filteredReading;
			RRF+=it->referenceReading;
		}
		std::advance(it,1);
	}
	LRF/=l_magneticSensor.size()/2;
	RRF/=l_magneticSensor.size()/2;
	if(MathUtil::inEqualInterval(LR,LRF
			,config.c_signalTriggerThreshold)
	   &&MathUtil::inEqualInterval(RR,RRF,
			 config.c_signalTriggerThreshold))
	{
		m_servo.SetDegree(900);
	}else{
		m_servo.setAngleBySignalDifference(LRF-RRF);
	}
#endif
#if USE_SERVO_ALG==2
#ifndef ADVANCED_SPEED_MONITOR
#define ADVANCED_SPEED_MONITOR 1
		/*
		 *	This algorithm is designed to deal with 4 magnetic sensors with the leftmost and rightmost perpendicular
		 *	To the car and the two in the middle lies 45 degrees to the car.
		 *	The principle is as follows:
		 *
		 *		1.	When the car is in a straight road, the sensors on two sides should return a maximum reading.
		 *			Whereas the middle two should give a stable reading. (note that the reading of them may not be the same and thus
		 *			reference reading is important)
		 *
		 *		2.  When the car is in a turn, sensors on the side should give a significant rise in reading. If the car is turning
		 *			left, the sensor in the middle left should get a reduced reading and the middle right one should get an
		 *			increased reading, which should have raised rapidly. The sharper the turn is, the higher the gains are.
		 *
		 *		3.	When the car is in a right-angled turn or in a cross, all sensors should return high readings.
		 *			The difference between the two cases is that, if the situation the car is going to encounter is a
		 *			right-angled turn, the reading should drop rapidly after some time interval. But in the cross-road case
		 *			the reading should behave like what they would in a straight road.
		 *
		 *		4.	To figure out whether the car should turn left or right in a right-angled turn, the car may turn in either
		 *			direction first. If the left reading drops to a minimal value the car should turn RIGHT. Otherwise the car
		 *			should turn LEFT.
		 *
		 *		p.s. The referenceReading of the sensors on both sides should be very high.
		 *			 Whereas the references of the sensors in the middle should be at around the midpoint of the interval.
		 *
		 *		p.s. It is assumed that the reading of the sensor is PROPORTIONAL to the distance between the wire and
		 *			 the sensor. If this is not true, the algorithm may need to be justified and depends on the difference
		 *			 between readings from different sensors.
		 *
		 *		p.s. Do not forget to calibrate the sensors before using this algorithm.
		 */

	//Stage 1: get state of each sensor. Suppress cars that are not equipped with 4 magnetic sensors.
	if(l_magneticSensor.size()!=4)return;
	MagneticSensor::ReadingState state[4];
	int index=0;
	for(MgItr i=l_magneticSensor.begin();i!=l_magneticSensor.end();i++)
	{
		state[index]=i->getState();
		index++;
	}
	//Stage 2: identify states
	if(state[0]==state[3]&&state[0]==MagneticSensor::ReadingState::IN_REFERENCE_STATE)
	{
		//either straight road,right-angled-turn or right turn.
		if(state[1]==state[2]&&state[1]==MagneticSensor::ReadingState::IN_REFERENCE_STATE)
		{
#if		VERSION > 2L
			this->state.addTask(CarState::Situation::s_straightRoad,config.c_sensorCarDistance);
#else
			this->state.addTask(CarState::Situation::s_straightRoad);
#endif
		}else
		{
			//Straight road and crosses
		}
	}else
	{
		//turn
	}
#endif
#endif
#if USE_SERVO_ALG == 3
/*
 * Third algorithm: identify differences between the readings of sensors on both sides
 */
	int j=0;
	float reading[4];
	for(MgItr it=l_magneticSensor.begin();it!=l_magneticSensor.end();it++)
	{
		if(config.c_useKalmanFilter)
		{
			reading[j]=it->getFilteredReading();
		}else
		{
			reading[j]=it->getReading();
		}
		j++;
	}
	float dif = (reading[0]-reading[3])/(reading[3]+reading[0]);
			//TODO calibrate the values
//	config.c_servoAngle=config.c_servoCentralAngle*(config.c_servoAngleMultiplier*dif+1);
	if( dif>1||dif<-1) return;
	//Is the sensor out of range?
	float thr=config.c_sensorSignalInvalidThreshold;
	if(reading[0]<thr&&reading[3]<thr) return;
	config.c_servoAngle=config.c_servoCentralAngle*(config.c_servoAngleMultiplier*dif+1);
	m_servo.SetDegree(config.c_servoAngle);
#endif
#if USE_SERVO_ALG == 4
	int j=0;
		float reading[4];
		for(MgItr it=l_magneticSensor.begin();it!=l_magneticSensor.end();it++)
		{
			if(config.c_useKalmanFilter)
			{
				reading[j]=it->getFilteredReading();
			}else
			{
				reading[j]=it->getReading();
			}
			j++;
		}
		float dif = (reading[0]-reading[3])/(reading[3]+reading[0]);
		//TODO is that work?
		if( dif>1||dif<-1) return;
		config.c_servoAngle=config.c_servoCentralAngle*(config.c_servoAngleMultiplier*dif+1);
		m_servo.SetDegree(config.c_servoAngle);
#endif
}



void Car::r_motorRoutine()
{
	m_motor.SetClockwise(config.c_motorRotateClockwise);
	if(config.c_mode==0)
	{
		if(config.c_motorPIDEnabled)
		{
			m_motor.SetPower(m_motor.pid.getTunedValue(
					(uint16_t)config.c_motorPIDSp));
		}else
		{
			m_motor.SetPower(config.c_motorPower);
		}

	}else
	{}

}
void Car::r_bluetoothRoutine()
{
	if(config.c_halt)return;
	int len=0;
	Byte buffer[256];
	if(config.c_broadcastMotorPower)
	{
		len=sprintf((char*)buffer, "%d\n",m_motor.GetPower());
		m_bluetooth.SendBuffer(buffer,len);
	}
#if DEBUG_MODE!=1
	if(config.c_broadcastServoAngle)
	{
		len=sprintf((char*)buffer, "%d\n",m_servo.GetDegree());
		m_bluetooth.SendBuffer(buffer,len);
	}
#endif

	//TODO Enable this for testing readings in different situation first.
	if(config.c_broadcastSensorReading)
	{
		std::string buf="";
		int i=0;
		float reading[4];
		float reference[4];
		float threshold[4];
		int state[4];
		for(MgItr it=l_magneticSensor.begin();
				it!=l_magneticSensor.end();it++)
		{
#if CALIBRATE_METHOD < 6
			float value=it->getReading();
			reference[i]=it->getReferenceReading();
			reading[i]=value;
			threshold[i]=it->getThreshold();
			state[i]=(int)(it->getState());
#endif
			//it works:)
			char buffer[100];
#if SEND_FORMAT == 1
			int j=0;
			for(MgItr it=l_magneticSensor.begin();it!=l_magneticSensor.end();it++)
			{
				reading[j]=it->getReading();
				j++;
			}
			int len=sprintf((char*)buffer,"%f,%f,%f,%f\n",reading[0],reading[1],reading[2],reading[3]);
			m_bluetooth.SendBuffer((Byte*)buffer,len);

#endif
			i++;
		}
#if SEND_FORMAT == 2
			int len=sprintf((char*)buffer,"%f,%f,%f,%f,%f,%f\n",m_filter.Filter(reading[0]),
					m_filter.Filter(reading[1]),m_filter.Filter(reading[2]),m_filter.Filter(reading[3]),
					config.c_kalmanFilterControlVariable[0],config.c_kalmanFilterControlVariable[1]);
			m_bluetooth.SendBuffer((Byte*)buffer,len);
#endif
#if SEND_FORMAT == 3
			int len=sprintf((char*)buffer,"%f,%f,%f,%f\n",reading[0],reading[1],reading[2],reading[3]);
			m_bluetooth.SendBuffer((Byte*)buffer,len);
#endif
#if SEND_FORMAT == 4
			int len=sprintf((char*)buffer,"%f,%f,%f,%f,%f,%f\n",reading[0],reading[1],reading[2],reading[3],
									l_magneticSensor.front().getReferenceReading(),
									l_magneticSensor.front().getThreshold());
			m_bluetooth.SendBuffer((Byte*)buffer,len);
#endif
#if SEND_FORMAT == 5
			int len=sprintf((char*)buffer,"%f,%f,%f,%f,%f,%f,%f,%f\n",reading[0],reading[1],reading[2],reading[3],
												reference[0],reference[1],reference[2],reference[3]);
						m_bluetooth.SendBuffer((Byte*)buffer,len);
#endif
#if SEND_FORMAT == 6
			int len=sprintf((char*)buffer,"%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",reading[0],reading[1],reading[2],reading[3],
												reference[0],reference[1],reference[2],reference[3],
												threshold[0],threshold[1],threshold[2],threshold[3]
												);
			m_bluetooth.SendBuffer((Byte*)buffer,len);
#endif
#if SEND_FORMAT == 7
			int len=sprintf((char*)buffer,"%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d\n",reading[0],reading[1],reading[2],reading[3],
															threshold[0],threshold[1],threshold[2],threshold[3],
															state[0],state[1],state[2],state[3]
															);
			m_bluetooth.SendBuffer((Byte*)buffer,len);
#endif
#if SEND_FORMAT == 8
			int j=0;
			int readingState[6];
			std::list<MagneticSensor::ReadingState> list=getDifferenceState();
			for(std::list<MagneticSensor::ReadingState>::iterator it
					=list.begin();it!=list.end();it++)
			{
				readingState[j]=(int)(*it);
				j++;
			}
			int len=sprintf((char*)buffer,"%d,%d,%d,%d,%d,%d\n",readingState[0],
							readingState[1],readingState[2]
							,readingState[3],readingState[4],readingState[5]);
			m_bluetooth.SendBuffer((Byte*)buffer,len);
#endif
#if SEND_FORMAT == 9
			int j=0;
			int readingState[6];
			std::list<MagneticSensor::ReadingState> list=getDifferenceState();
			for(std::list<MagneticSensor::ReadingState>::iterator it
					=list.begin();it!=list.end();it++)
			{
				readingState[j]=(int)(*it);
				j++;
			}
			j=0;
			for(MgItr it=l_magneticSensor.begin();it!=l_magneticSensor.end();it++)
			{
				reading[j]=it->getReading();
				j++;
			}
			int len=sprintf((char*)buffer,"%f,%f,%f,%f,%d,%d,%d,%d,%d,%d\n",
					reading[0],reading[1],reading[2],reading[3],
					readingState[0],readingState[1],readingState[2]
							,readingState[3],readingState[4],readingState[5]);
			m_bluetooth.SendBuffer((Byte*)buffer,len);
#endif
#if SEND_FORMAT == 10
			int j=0;
			for(MgItr it=l_magneticSensor.begin();it!=l_magneticSensor.end();it++)
			{
				reading[j]=it->getReading();
				j++;
			}
			int len=sprintf((char*)buffer,"%f,%f,%f,%d\n",reading[3],
					reading[0],(reading[3]-reading[0])/(reading[3]+reading[0])
					*(config.c_servoAngleMultiplier+1),m_servo.GetDegree());
			m_bluetooth.SendBuffer((Byte*)buffer,len);
#endif
	}
	if(config.c_broadcastPIDControlVariable)
	{
		for(int i=0;i<3;i++)
		{
			len=sprintf((char*)buffer, "%f\n"
					,config.c_motorPIDControlVariable[i]);
			m_bluetooth.SendBuffer(buffer,len);
		}
	}
	if(config.c_broadcastPIDSp)
	{
		len=sprintf((char*)buffer, "%f\n",config.c_motorPIDSp);
		m_bluetooth.SendBuffer(buffer,len);
	}
	if(config.c_broadcastEncoderReading)
	{
		len=sprintf((char*)buffer, "%d\n",m_encoder.GetCount());
		m_bluetooth.SendBuffer(buffer,len);
	}
}
#if HIGH_LEVEL_INSTR
void	Car::CALIBRATE_SENSORS()
{
#if CALIBRATE_METHOD <=5
	for(MgItr i=l_magneticSensor.begin();i!=l_magneticSensor.end();i++)
	{
		i->calibrate();
	}
#endif
#if CALIBRATE_METHOD == 6
	/*
	 * This method uses the difference among readings of sensors to determine state.
	 */
	float reading[4];
	int i=0,count=0;
	TimerInt initTime=System::Time(),elapsedTime=initTime;
			float difference[6]={0,0,0,0,0,0},sqdifference[6]={0,0,0,0,0,0};
	while(elapsedTime-initTime<=1000)	//calibrate for 1 second
	{
		i=0;
		for(MgItr it=l_magneticSensor.begin();it!=l_magneticSensor.end();it++)
		{
			reading[i]=it->getReading();
			i++;
		}
		difference[0]+=reading[1]-reading[0];
		difference[1]+=reading[2]-reading[0];
		difference[2]+=reading[3]-reading[0];
		difference[3]+=reading[2]-reading[1];
		difference[4]+=reading[3]-reading[1];
		difference[5]+=reading[3]-reading[2];
		sqdifference[0]+=(reading[1]-reading[0])*(reading[1]-reading[0]);
		sqdifference[1]+=(reading[2]-reading[0])*(reading[2]-reading[0]);
		sqdifference[2]+=(reading[3]-reading[0])*(reading[3]-reading[0]);
		sqdifference[3]+=(reading[2]-reading[1])*(reading[2]-reading[1]);
		sqdifference[4]+=(reading[3]-reading[1])*(reading[3]-reading[1]);
		sqdifference[5]+=(reading[3]-reading[2])*(reading[3]-reading[2]);
		count++;
		elapsedTime=System::Time();
	}
	for(int i=0;i<6;i++)
	{
		//TODO threshold too high?
		state.scheduler.signalDifference[i]=difference[i]/count;
		state.scheduler.signalThreshold[i]=1.2*(sqdifference[i]/count
						-difference[i]*difference[i]/(count*count));	//double the variance may work better
	}
#endif
}
#if STATE_IDENTIFY_ALG == 3
std::list<MagneticSensor::ReadingState> Car::getDifferenceState()
{
		typedef std::list<MagneticSensor>::iterator MgItr;
		float reading[4],difference[6]={0,0,0,0,0,0};
		int i=0;
		for(MgItr it=l_magneticSensor.begin();it!=l_magneticSensor.end();it++)
		{
			reading[i]=it->getReading();
			i++;
		}
		difference[0]+=reading[1]-reading[0];
		difference[1]+=reading[2]-reading[0];
		difference[2]+=reading[3]-reading[0];
		difference[3]+=reading[2]-reading[1];
		difference[4]+=reading[3]-reading[1];
		difference[5]+=reading[3]-reading[2];
		std::list<MagneticSensor::ReadingState> result;
		for(int i=0;i<6;i++)
		{
			float dif=difference[i],
				  ref=state.scheduler.signalDifference[i],
				  thr=state.scheduler.signalThreshold[i];
			if(dif<=ref+thr&&dif>=ref-thr)
			{
				result.push_back(MagneticSensor::ReadingState::IN_REFERENCE_STATE);
				continue;
			}
			if(dif<ref-thr && dif>=ref-2*thr)
			{
				result.push_back(MagneticSensor::ReadingState::BELOW_REFERENCE_STATE);
				continue;
			}
			if(dif<ref - 2*thr)
			{
				result.push_back(MagneticSensor::ReadingState::IS_MINIMAL);
				continue;
			}
			if(dif>ref+thr && dif<=ref+2*thr)
			{
				result.push_back(MagneticSensor::ReadingState::ABOVE_REFERENCE_STATE);
				continue;
			}
			if(dif>ref+2*thr)
			{
				result.push_back(MagneticSensor::ReadingState::IS_MAXIMAL);
				continue;
			}
		}
		return result;
}
#endif
void	Car::CALIBRATE_SENSORS(KF filter)
{
	for(MgItr i=l_magneticSensor.begin();i!=l_magneticSensor.end();i++)
		{
			i->calibrate(filter);
		}
}
void	Car::BREAK()
{
	m_motor.SetPower(0);
	config.c_halt=true;
}
void	Car::RESTORE()
{
	m_motor.SetPower(config.c_motorPower);
	config.c_halt=false;
}
void	Car::MOVE_FORWARD(uint16_t power, uint16_t angle,bool isLeft)
{
	config.c_motorRotateClockwise=false;	//for 2 gears
	config.c_motorPower=libutil::Clamp<uint16_t>(0,power,1800);
	m_motor.SetPower(config.c_motorPower);
	uint16_t finalAngle=900;
	if(isLeft)
	{
		config.c_servoAngle=libutil::Clamp<uint16_t>(0,finalAngle-angle,1800);
	}else
	{
		config.c_servoAngle=libutil::Clamp<uint16_t>(0,finalAngle+angle,1800);
	}
}
void Car::MOVE_BACKWARD(uint16_t power, uint16_t angle,bool isLeft)
{
	config.c_motorRotateClockwise=true;	//for 2 gears
	config.c_motorPower=libutil::Clamp<uint16_t>(0,power,1800);
	m_motor.SetPower(config.c_motorPower);
	uint16_t finalAngle=900;
	if(isLeft)
	{
		config.c_servoAngle=libutil::Clamp<uint16_t>(0,finalAngle-angle,1800);
	}else
	{
		config.c_servoAngle=libutil::Clamp<uint16_t>(0,finalAngle+angle,1800);
	}
}
#endif

void Car::r_restoreBlock()
{
	if(config.c_halt)
	{
		/*
		 * Insert codes that the system needs to do when the car is halted.
		 */
		r_bluetoothRoutine();
	}
}

void Car::conformProtocol()
{
	/*
	 * Set all parameters of the car to the current configuration
	 */
	m_servo.SetDegree(config.c_servoAngle);
	m_motor.SetPower(config.c_motorPower);
	m_motor.SetClockwise(config.c_motorRotateClockwise);
	for(MgItr i=l_magneticSensor.begin();
			i!=l_magneticSensor.end();i++)
	{
		i->setMax(config.c_magneticSensorUpperBound);
		i->setMin(config.c_magneticSensorLowerBound);
	}
	//TODO complete the rest
}
#if IS_DEBUG
void Car::d_servoDebug()
{
	for(uint16_t i=0;i<1800; i+=100)
	{
		m_servo.SetDegree(i);
		System::DelayMs(200);
	}
}
#endif
