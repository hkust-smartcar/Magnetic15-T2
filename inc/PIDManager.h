/*
 * PIDManager.h
 *
 *  Created on: 26 Feb, 2015
 *      Author: Travis
 */

#ifndef PIDMANAGER_H_
#define PIDMANAGER_H_
#include <list>
#include <PID.h>
class PIDManager {
public:
	PIDManager();
	static std::list<GeneralPID> pidList;
};

#endif /* PIDMANAGER_H_ */
