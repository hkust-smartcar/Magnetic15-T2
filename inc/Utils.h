/*
 * Utils.h
 *
 *  Created on: 12 Mar, 2015
 *      Author: Travis
 */

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

class Utils {
public:
	Utils();
	static float convertValueFromInterval(float oldValue,float oldLowerBound,
				float oldUpperBound,float newLowerBound, float newUpperBound)
	{
		return (oldValue-oldLowerBound)/(oldUpperBound-oldLowerBound)*
				(newUpperBound-newLowerBound)+newLowerBound;
	}
};

#endif /* INC_UTILS_H_ */
