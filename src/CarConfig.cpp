/*
 * CarConfig.cpp
 *
 *  Created on: 28 Feb, 2015
 *      Author: Travis
 */

#include <CarConfig.h>

CarConfig::CarConfig():

	c_halt								(0),

	c_ledCount							(3),

	c_magneticSensorCount				(4),

	c_voltmeterVoltageRatio				(1),

	c_servoCentralAngle					(650),

	c_servoAngleLowerBound				(0),

	c_processStateAfterTime				(50),	//in ms

	c_servoAngleUpperBound				(1300),

	c_wheelDiameter						(0.0535),
	c_encoderCountPerRevolution			(500),
	c_gearRatio							(2.625) // 105/40
{
	c_mode							=	0;

	c_loopInterval					=	10;

	c_servoAngleMultiplier			=	50;
	c_servoAngle					=	800;


	c_motorPower					=	300;
	c_motorRotateClockwise			=	0;


	c_magneticSensorLowerBound		=	0;
	c_magneticSensorUpperBound		=	100;
//	c_magneticSensorTriggerThreshold= 	3;		//for [0,100]

	c_motorPIDEnabled				=	0;
	c_motorPIDControlVariable[0]	=	0;
	c_motorPIDControlVariable[1]	=	0;
	c_motorPIDControlVariable[2]	=	0;
	c_motorPIDSp					=	0;

	c_bluetoothRXThreshold			=	2;
	c_broadcastMotorPower			=	0;
	c_broadcastServoAngle			=	0;
	c_broadcastSensorReading		=	1;
	c_broadcastPIDControlVariable	=	0;
	c_broadcastPIDSp				=	0;
	c_broadcastEncoderReading		=	0;
	c_broadcastPlainValue			=	1;

	c_useKalmanFilter				=	1;
	c_kalmanFilterControlVariable[0]=	0.00001;
	c_kalmanFilterControlVariable[1]=	0.8;
	c_signalTriggerThreshold		=	0;
	c_sensorSignalInvalidThreshold	=	20;

}

