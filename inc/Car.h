/*
 * Car.h
 *
 *  Created on: 22 Jan, 2015
 *      Author: Travis
 */

#ifndef SRC_CAR_H_
#define SRC_CAR_H_
//#include <libsc/k60/mma8451q.h>
#include <libsc/k60/alternate_motor.h>
//#include <libsc/k60/tower_pro_mg995.h>
#include <libsc/k60/mpu6050.h>
#include <libsc/k60/ftdi_ft232r.h>
//#include <libsc/k60/ab_encoder.h>
#include <libsc/k60/system.h>
#include <libsc/k60/trs_d05.h>
#include <CarState.h>
#include <libsc/k60/jy_mcu_bt_106.h>
#include <libutil/looper.h>
#include <cstdio>
#include "MagneticSensor.h"
#include <functional>
#include <libsc/k60/st7735r.h>
#include <LcdInterface.h>
#include <libsc/k60/lcd_typewriter.h>
#include <libutil/kalman_filter.h>
#include <KF.h>
#include <CarConfig.h>
#include <assert.h>
#include <tLed.h>
#include <TMotor.h>
#include <TEncoder.h>
#include <TServo.h>
#include <libsc/k60/battery_meter.h>
#include <PID.h>
#include <MathUtil.h>
#include "Config.h"
#include "TBluetooth.h"
//#include <libsc/k60/joystick.h>
using namespace libsc::k60;
using namespace libbase::k60;

#if 	IS_FIRST_CAR==0
#define USE_CCD 1
#define USE_LCD 1
#endif

class Car {
public:
//	typedef 	JyMcuBt106	 						TBluetooth;
	typedef 	uint32_t 	 						TimerInt;
	typedef		BatteryMeter 						Meter;
	typedef		std::list<MagneticSensor>::iterator MgItr;
	typedef		std::list<TLed>::iterator			LedItr;
	CarConfig 	config;
	CarState	state;
				Car();
#if USE_BATTERY_METER !=0
	Meter		m_meter;
#endif
	static void btListener(const Byte* byte, const size_t size);

	void 		r_ledRoutine();
	void 		r_encoderRoutine();
	void 		r_accelerometerRoutine();
	void 		r_magneticSensorRoutine();
	void		r_servoRoutine();
	void		r_motorRoutine();
	void		r_bluetoothRoutine();
#if CAR_STATE_HANDLING
	void		r_stateHandlingRoutine();
#endif
	void 		run();

#if HIGH_LEVEL_INSTR
	void		BREAK();
	void		RESTORE();
	void		MOVE_FORWARD(uint16_t power, uint16_t angle,bool isLeft);
	void		MOVE_BACKWARD(uint16_t power,uint16_t andgle,bool isLeft);
	void		CALIBRATE_SENSORS();
	void		CALIBRATE_SENSORS(KF filter);
#endif

private:
//	Led 		 		led;
//	Led 		 		btLockInd;
//	Led 		 		m_btTxInd;
//	DirMotor 	 		m_dirMotor;
//	TrsD05 		 		servo;
//	Mpu6050 	 		m_accelerometer;
//	MagneticSensor 		mg[2];
#ifdef USE_LCD
	LcdInterface 				lcdInterface;
	LcdInterface::Menu 			menu;
#endif
	KF 							m_filter;
	std::list<TLed>				l_led;
	std::list<MagneticSensor> 	l_magneticSensor;
	TMotor						m_motor;
#if DEBUG_MODE!=1
	TServo						m_servo;
#endif
	TEncoder 	 				m_encoder;
	TBluetooth	 				m_bluetooth;
#if DEBUG_MODE==1
	Gpo gpo;
#endif
};


#endif /* SRC_CAR_H_ */
