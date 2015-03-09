/*
 * MagneticSensor.cpp
 *
 *  Created on: 10 Feb, 2015
 *      Author: Travis
 */

#include <MagneticSensor.h>

namespace
{

Adc::Config GetAdcConfig(Pin::Name pin)
{
	Adc::Config config;
	config.pin=pin;
	config.is_continuous_mode=false;
	config.is_diff_mode=false;
	config.speed=Adc::Config::SpeedMode::kTypical;
	config.avg_pass=Adc::Config::AveragePass::k4;
	config.resolution=Adc::Config::Resolution::k8Bit;
//	config.conversion_isr=listener;
	return config;
}

}

MagneticSensor::MagneticSensor(Pin::Name pin)
		: adc(GetAdcConfig(pin)),isEnable(true)
{
	//sensorCount++;
	min=0;
	max=100;
	referenceReading=getReading();
}

MagneticSensor::MagneticSensor(libbase::k60::Adc::Config config)
							:adc(config),isEnable(true)
{
	//MagneticSensor::sensorCount++;
	min=0;
	max=100;
	referenceReading=getReading();
}

float MagneticSensor::getReading(){
	if(isEnable)
	return adc.GetResultF()*(max-min)*10/33+min;
	else return 0;
}
void MagneticSensor::setMax(int max){
	this->max=max;
}

void MagneticSensor::setMin(int min){
	this->min=min;
}
int MagneticSensor::getMin()
{
	return min;
}
int	MagneticSensor::getMax()
{
	return max;
}
void MagneticSensor::setRange(int min,int max){
	if(min<max){
		setMin(min);
		setMax(max);
	}
}
void MagneticSensor::setEnable(bool flag)
{
	isEnable=flag;
}
void MagneticSensor::updateReading()
{
	rawReading=getReading();

}
void MagneticSensor::updateFilteredReading(KF filter)
{
	filteredReading=filter.Filter(getReading());
}
void MagneticSensor::calibrate()
{
	referenceReading=getReading();
}
void MagneticSensor::calibrate(KF filter)
{
	referenceReading=getFilteredReading(filter);
}
float MagneticSensor::getFilteredReading(KF filter)
{
	return filter.Filter(getReading());
}
bool MagneticSensor::isNotInReferenceState()
{
	if(filteredReading<referenceReading-threshold
			||filteredReading>referenceReading+threshold)
	{
		return true;
	}
	return false;
}
/*
 * In reference state does not imply the reading is at the middle of the interval. If the reference is in the upper quartile
 * and the threshold is large enough, the IS_MAXIMAL case may never be returned.
 */
MagneticSensor::ReadingState MagneticSensor::getState()
{
	if(filteredReading<referenceReading-threshold)
	{
		if(filteredReading<getMin()+threshold)
		{
			return ReadingState::IS_MINIMAL;
		}else
		{
			return ReadingState::BELOW_REFERENCE_STATE;
		}
	}else if(filteredReading>referenceReading+threshold)
	{
		if(filteredReading>getMax()-threshold)
		{
			return ReadingState::IS_MAXIMAL;
		}else
		{
			return ReadingState::ABOVE_REFERENCE_STATE;
		}
	}else
	{
		return ReadingState::IN_REFERENCE_STATE;
	}
}
void	MagneticSensor::setTriggerThreshold(float threshold)
{
//	threshold=libutil::Clamp<float>((float)(getMax()-getMin())/50,threshold,(float)(getMax-getMin())/5);
	CLAMP((getMax()-getMin())/50,threshold,(getMax()-getMin())/5);

}
