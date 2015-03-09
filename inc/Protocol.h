/*
 * Protocol.h
 *
 *  Created on: 5 Mar, 2015
 *      Author: Travis
 */

#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

class Protocol {
public:
	Protocol();
 	const static int 		  SPACE_ASCII 		= 32;
    const static int 		  DASH_ASCII 		= 45;
    const static int 		  NEW_LINE_ASCII 	= 10;
    const static char		  SIGNAL			= 's';
    const static char		  DELIM				= 't';

};

#endif /* INC_PROTOCOL_H_ */
