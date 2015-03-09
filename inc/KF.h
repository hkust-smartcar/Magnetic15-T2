/*
 * KF.h
 *
 *  Created on: 26 Feb, 2015
 *      Author: Travis
 */

#ifndef INC_KF_H_
#define INC_KF_H_

#include <libutil/kalman_filter.h>

class KF: public libutil::KalmanFilter {
public:
	explicit KF(const float q, const float r,
				const float x, const float p)
			:KalmanFilter(q,r,x,p){};

	enum VAR{
		Q=0,
		R
	};
	void setParam(VAR var, float value);
	float getParam(VAR var);
};

#endif /* INC_KF_H_ */
