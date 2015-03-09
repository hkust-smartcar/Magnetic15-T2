/*
 * TBluetooth.h
 *
 *  Created on: 5 Mar, 2015
 *      Author: Travis
 */

#ifndef TBLUETOOTH_H_
#define TBLUETOOTH_H_

#include <libsc/k60/jy_mcu_bt_106.h>
#include "Protocol.h"
class TBluetooth: public libsc::k60::JyMcuBt106 {
public:
	TBluetooth(const Config &config);
	std::string	composeMessage(int index,float value,bool plain=false);
	Protocol protocol;
};

#endif /* TBLUETOOTH_H_ */
