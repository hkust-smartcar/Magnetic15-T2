/*
 * Module.h
 *
 *  Created on: 14 Mar, 2015
 *      Author: Travis
 */

#ifndef INC_MODULE_H_
#define INC_MODULE_H_
#include <Car.h>
class Module {
public:
	Module(Car* car);
	virtual ~Module();
	virtual void run();
	virtual void loopWhileSuspension();
	void	suspend();
	void	resume();
private:
	Car* p_car;
	bool disable;
};

#endif /* INC_MODULE_H_ */
