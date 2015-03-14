/*
 * CarConfig.h
 *
 *  Created on: 28 Feb, 2015
 *      Author: Travis
 */

#ifndef INC_CARCONFIG_H_
#define INC_CARCONFIG_H_
#include <stdint.h>
#include <list>
#include <TMotor.h>
#include <PID.h>
#include <Config.h>
using namespace std;
class CarConfig {
public:
	CarConfig();

	static const int	MAX_LED_COUNT			  = 4;
	static const int	MAX_MAGNETIC_SENSOR_COUNT = 4;

	bool	 			c_halt;
	volatile bool		c_mode;	//1 means auto. 0 means manual
	const	 int		c_ledCount;
	const	 uint16_t	c_processStateAfterTime;
#if VERSION > 2L
	const	 float		c_sensorCarDistance;		//in m
#endif

	const	 float		c_voltmeterVoltageRatio;

	volatile uint16_t	c_loopInterval;

	volatile uint16_t 	c_servoAngleMultiplier;
	volatile uint16_t 	c_servoAngle;
	const 	 uint16_t	c_servoAngleLowerBound;
	const	 uint16_t	c_servoAngleUpperBound;
	const	 uint16_t	c_servoCentralAngle;
	volatile uint16_t 	c_motorPower;
	volatile bool 	  	c_motorRotateClockwise;

	const 	 int 		c_magneticSensorCount;
	volatile int 	  	c_magneticSensorLowerBound;
	volatile int 	  	c_magneticSensorUpperBound;
	volatile float		c_magneticSensorTriggerThreshold;

	volatile bool 	  	c_motorPIDEnabled;
	volatile float 	  	c_motorPIDControlVariable[3];
	volatile float 	  	c_motorPIDSp;
	volatile bool		c_motorPIDUseAutomat;

	volatile uint8_t	c_bluetoothRXThreshold;
	volatile bool 		c_broadcastMotorPower;
	volatile bool		c_broadcastServoAngle;
	volatile bool		c_broadcastSensorReading;
	volatile bool		c_broadcastPIDControlVariable;
	volatile bool		c_broadcastPIDSp;
	volatile bool		c_broadcastEncoderReading;
	volatile bool		c_broadcastSpeed;
	volatile bool		c_broadcastPlainValue;

	volatile bool		c_useKalmanFilter;
	volatile float		c_kalmanFilterControlVariable[2];
	volatile float		c_signalTriggerThreshold;

	const	 float		c_wheelDiameter;	//in m
	const	 int		c_encoderCountPerRevolution;
	const	 float		c_gearRatio;		//Tire gear/motor gear
	volatile float		c_sensorSignalInvalidThreshold;
};

#endif /* INC_CARCONFIG_H_ */
