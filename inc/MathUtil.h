/*
 * MathUtil.h
 *
 *  Created on: 2 Mar, 2015
 *      Author: Travis
 */

#ifndef MATHUTIL_H_
#define MATHUTIL_H_

class MathUtil {
public:
	MathUtil();
	template<typename T>
	static bool 		inEqualInterval(T val, T midPoint,T radius)
						{
							if(val<midPoint-radius || val>midPoint+radius)
									return false;
							return true;
						}
};

#endif /* MATHUTIL_H_ */
