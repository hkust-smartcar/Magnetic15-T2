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
			   1,1)
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
			l_magneticSensor.push_back(MagneticSensor(pin));
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
		System::DelayMs(config.c_loopInterval);
		r_encoderRoutine();
		r_ledRoutine();
		r_accelerometerRoutine();
		r_magneticSensorRoutine();

#if !CAR_STATE_HANDLING
#if DEBUG_MODE!=1
		r_servoRoutine();
#endif
		r_motorRoutine();
#else
		r_stateHandlingRoutine();
#endif
		r_bluetoothRoutine();
	}
}


void Car::btListener(const Byte* byte, const size_t size){

	switch(*byte){
//	case 'r':
//		m_instance->btLock=!m_instance->btLock;
//		m_instance->bt.SendStr("BT: I get something!");
//		break;
//	case 's':
//		m_instance->lcdInterface.highlightNextOption();
//		break;
//	case 't':
//		m_instance->filter.setParam(KF::VAR::Q,libutil::Clamp<float>(0,
//				m_instance->filter.getParam(KF::VAR::Q)+0.125,1));
//		break;
//	case 'g':
//		m_instance->filter.setParam(KF::VAR::Q,libutil::Clamp<float>(0,
//						m_instance->filter.getParam(KF::VAR::Q)-0.125,1));
//		break;
//	case 'y':
//		m_instance->filter.setParam(KF::VAR::R,libutil::Clamp<float>(0,
//						m_instance->filter.getParam(KF::VAR::R)+0.125,1));
//		break;
//	case 'h':
//		m_instance->filter.setParam(KF::VAR::R,libutil::Clamp<float>(0,
//						m_instance->filter.getParam(KF::VAR::R)-0.125,1));
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
	for(MgItr it=l_magneticSensor.begin();
			it!=l_magneticSensor.end();it++)
	{
		it->updateReading();
		if(config.c_useKalmanFilter)
		{
			it->getFilteredReading(m_filter);
		}
//		else
//		{
//			it->filteredReading=it->rawReading;
//		}
	}

}
void Car::r_stateHandlingRoutine()
{
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
}
//TODO get it done as well
#if DEBUG_MODE!=1
void Car::r_servoRoutine()
{
	if(config.c_halt)return;
	if(l_magneticSensor.size()%2)return;
	if(l_magneticSensor.size()==0)return;
#if USE_SERVO_ALG==0
		/*
		 * This is almost equivalent to manually control the servo.
		 */
		m_servo.SetDegree(config.c_servoAngle);


#elif USE_SERVO_ALG==1
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
#elif USE_SERVO_ALG==2
#ifndef ADVANCED_SPEED_MONITOR
#define ADVANCED_SPEED_MONITOR 1
#endif
	//TODO get this done
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
}
//TODO try to finish this algorithm after plotting of the sensors in different situation.

#elif USE_SERVO_ALG==3
/*
 * Third algorithm: simple PID that uses the difference between the readings as set points
 */

#endif

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
#define SEND_FORMAT 6
	//TODO Enable this for testing readings in different situation first.
	if(config.c_broadcastSensorReading)
	{
		std::string buf="";
		int i=0;
		float reading[4];
		float reference[4];
		float threshold[4];
		for(MgItr it=l_magneticSensor.begin();
				it!=l_magneticSensor.end();it++)
		{
			float value=it->getReading();
			reference[i]=it->getReferenceReading();
			reading[i]=value;
			threshold[i]=it->getThreshold();
			//it works:)
			//TODO for testing signals
			char buffer[100];
#if SEND_FORMAT == 1
			int len=sprintf(buffer,"%f",value);
			m_bluetooth.SendBuffer((Byte*)buffer,len);
			m_bluetooth.SendStrLiteral("\n");
			System::DelayMs(20);
#endif
//			buf+=m_bluetooth.composeMessage(i,value,config.c_broadcastPlainValue);
//			buf+='\n';

//			m_bluetooth.SendStr(m_bluetooth.composeMessage(i,value));
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
			//TODO hard fault?!
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
	for(MgItr i=l_magneticSensor.begin();i!=l_magneticSensor.end();i++)
	{
		i->calibrate();
	}
}
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
	}
}
