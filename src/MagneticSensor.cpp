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

MagneticSensor::MagneticSensor(Pin::Name pin,KF filter)
		: adc(GetAdcConfig(pin)),isEnable(true),
		  filter(filter.getParam(KF::VAR::Q),filter.getParam(KF::VAR::R),1,1)
{
	//sensorCount++;
	min=0;
	max=100;
	referenceReading=getReading();
}

MagneticSensor::MagneticSensor(libbase::k60::Adc::Config config,KF filter)
							:adc(config),isEnable(true),
							 filter(filter.getParam(KF::VAR::Q),filter.getParam(KF::VAR::R),1,1)
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
#if CALIBRATE_METHOD == 1
	//naive calibrating method
	referenceReading=getReading();
#endif
#if CALIBRATE_METHOD == 2
	/*
	 * This calibrate method needs to be performed when the car is on a straight road.
	 * On calling this method 0.1 sec will be required to collect samples of read values.
	 * The average of the sample will be considered as the reference reading.
	 * The largest difference between the sample and the reference will be set as threshold,
	 * so that it is reasonable to guess that the state has changed when the difference between
	 * the reading and the reference is larger than the threshold.
	 */
	std::stack<float> valueStack;
	const uint16_t startTime	= libsc::k60::System::Time();
	uint16_t	   elapsedTime	= startTime;
	while(elapsedTime-startTime<100)		//1 sec fails. 0.1 sec ok
	{
		valueStack.push(getReading());
		elapsedTime=libsc::k60::System::Time();
	}
	int sampleSize=valueStack.size();
	float sum=0,max=0,min=0;
	while(!valueStack.empty())
	{
		float sample=valueStack.top();
		valueStack.pop();
		sum+=sample;
		if(max<sample)max=sample;
		if(min>sample)min=sample;
	}
	referenceReading=sum/sampleSize;
	if(max-referenceReading>referenceReading-min)
		 setTriggerThreshold(max-referenceReading);
	else setTriggerThreshold(referenceReading-min);
#endif
#if CALIBRATE_METHOD == 3
	/*
	 * This method is similar the the previous one. Except std::stack is not used as it is suspected that
	 * the data structure cannot store too many samples obtained in 1 sec.
	 */
	const uint16_t startTime	= libsc::k60::System::Time();
	uint16_t	   elapsedTime	= startTime;
	float sum=0,min=0,max=0;
	int count=0;
	while(elapsedTime-startTime<1000)		//1 sec
		{
			count++;
			float sample=getReading();
			sum+=sample;
			if(max<sample)max=sample;
			if(min>sample)min=sample;
			elapsedTime=libsc::k60::System::Time();
		}
	referenceReading=sum/count;
	if(max-referenceReading>referenceReading-min)
		 setTriggerThreshold(max-referenceReading);
	else setTriggerThreshold(referenceReading-min);
#endif
#if CALIBRATE_METHOD == 4
	/*
	 * This method aims to improve the previous calibration method. It is noted that the threshold set by the previous method
	 * is way too large. It is suspected that the VARIANCE may be used as the threshold.
	 */
	const uint16_t startTime	= libsc::k60::System::Time();
	uint16_t	   elapsedTime	= startTime;
	float sum=0,min=0,max=0,sumsq=0;
	int count=0;
	while(elapsedTime-startTime<1000)		//1 sec
		{
			count++;
			float sample=getReading();
			sum+=sample;
			sumsq+=(sample*sample);
			if(max<sample)max=sample;
			if(min>sample)min=sample;
			elapsedTime=libsc::k60::System::Time();
		}
	referenceReading=sum/count;
	setTriggerThreshold(sumsq/count-referenceReading*referenceReading);
#endif
#if CALIBRATE_METHOD == 5
	/*
	 * This method aims to improve the previous calibration method. It is noted that the threshold set by the previous method
	 * is way too large. It is suspected that the VARIANCE may be used as the threshold.
	 */
	const uint16_t startTime	= libsc::k60::System::Time();
	uint16_t	   elapsedTime	= startTime;
	float sum=0,min=0,max=0,sumsq=0;
	int count=0;
	while(elapsedTime-startTime<1000)		//1 sec
		{
			count++;
			float sample=getReading();
			sum+=sample;
			sumsq+=(sample*sample);
			if(max<sample)max=sample;
			if(min>sample)min=sample;
			elapsedTime=libsc::k60::System::Time();
		}
	referenceReading=sum/count;
	setTriggerThreshold(7);
#endif
	/*
	 * The 6th method for calibration is implemented in the general CALIBRATE_SENSORS method
	 */
}
void MagneticSensor::calibrate(KF filter)
{
#if CALIBRATE_METHOD == 1
	referenceReading=getFilteredReading(filter);
#endif
#if CALIBRATE_METHOD == 2

#endif
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
#if STATE_IDENTIFY_ALG != 3
MagneticSensor::ReadingState MagneticSensor::getState()
{
#if STATE_IDENTIFY_ALG ==1
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
#endif
#if STATE_IDENTIFY_ALG == 2
	/*
	 * 2nd algorithm for identifying sensor state.
	 */
		float value=getReading();
		if(value<=referenceReading+threshold
				&&value>=referenceReading-threshold)
		{
			return ReadingState::IN_REFERENCE_STATE;
		}
		if(value<referenceReading-threshold
				&&value>=referenceReading - 2*threshold)
		{
			return ReadingState::BELOW_REFERENCE_STATE;
		}
		if(value>referenceReading+threshold
				&&value>=referenceReading + 2*threshold)
		{
			return ReadingState::ABOVE_REFERENCE_STATE;
		}
		if(value>referenceReading+ 2* threshold)
		{
			return ReadingState::IS_MAXIMAL;
		}
		if(value<referenceReading - 2* threshold)
		{
			return ReadingState:: IS_MINIMAL;
		}
		return ReadingState::IN_REFERENCE_STATE;

#endif

}
#endif
void	MagneticSensor::setTriggerThreshold(float thr)
{
//	threshold=libutil::Clamp<float>((float)(getMax()-getMin())/50,threshold,(float)(getMax-getMin())/5);
#if SET_THRESHOLD_METHOD == 1
	CLAMP((getMax()-getMin())/50,threshold,(getMax()-getMin())/5);
#endif
#if SET_THRESHOLD_METHOD == 2
	threshold=thr;
#endif
}
float	MagneticSensor::getThreshold()
{
	return threshold;
}
float	MagneticSensor::getReferenceReading()
{
	return referenceReading;
}
float MagneticSensor::getFilteredReading()
{
	return getFilteredReading(filter);
}
void MagneticSensor::setFilterConfig(float q,float r)
{
	filter.setParam(KF::VAR::Q,q);
	filter.setParam(KF::VAR::R,r);
}
