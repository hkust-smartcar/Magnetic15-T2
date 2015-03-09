/*
 * TServo.h
 *
 *  Created on: 28 Feb, 2015
 *      Author: Travis
 */

#ifndef INC_TSERVO_H_
#define INC_TSERVO_H_

#include <libsc/k60/trs_d05.h>
#include <libutil/misc.h>
#include <PID.h>
class TServo: public libsc::k60::TrsD05 {
public:
	TServo(const Config &config,uint16_t lowerBound,
			uint16_t upperBound,float multiplier=9);
	void			setAngleBySignalDifference(float dif);
	void			SetDegree(uint16_t degree);
	const uint16_t	lowerBound;
	const uint16_t	upperBound;
private:
	float			angleMultiplier;
	PID<uint16_t>	pid;
};

#endif /* INC_TSERVO_H_ */
