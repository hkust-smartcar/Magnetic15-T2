/*
 * KF.cpp
 *
 *  Created on: 26 Feb, 2015
 *      Author: Travis
 */

#include <KF.h>


void KF::setParam(VAR var, float value)
{
	if(value>1||value<0)return;
	switch(var)
	{
		case Q:
			m_q=value;
			break;
		case R:
			m_r=value;
			break;
	}
}
float KF::getParam(VAR var)
{
	switch(var)
	{
		case Q:
			return m_q;
		case R:
			return m_r;
	}
}
