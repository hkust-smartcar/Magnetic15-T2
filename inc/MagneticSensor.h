/*
 * MagneticSensor.h
 *
 *  Created on: 10 Feb, 2015
 *      Author: Travis
 */

#ifndef INC_MAGNETICSENSOR_H_
#define INC_MAGNETICSENSOR_H_
#include <libbase/k60/adc.h>
#include <libsc/k60/System.h>
#include <libutil/misc.h>
#include <functional>
#include <KF.h>
#include <Config.h>
#include <stack>
#include <list>
using namespace libbase::k60;
class MagneticSensor {
public:
	typedef 			std::function<void(Adc *adc, const uint16_t result)>
						OnConversionCompleteListener;

	explicit 			MagneticSensor(Pin::Name pin,KF filter);
	explicit 			MagneticSensor(libbase::k60::Adc::Config config,KF filter);

	static 	Adc::Config	getAdcConfig(libbase::k60::Pin::Name pin){
				Adc::Config config;
					config.pin=pin;
//					config.is_continuous_mode=false;
//					config.is_diff_mode=false;
					config.speed=Adc::Config::SpeedMode::kTypical;
					config.avg_pass=Adc::Config::AveragePass::k4;
					config.resolution=Adc::Config::Resolution::k8Bit;
				return config;
	}
	enum				ReadingState
	{
			IN_REFERENCE_STATE		=	0,
			BELOW_REFERENCE_STATE	=	-1,
			ABOVE_REFERENCE_STATE	=	1,
			IS_MINIMAL				=	-2,
			IS_MAXIMAL				=	2
	};
	void 				setMin(int min);
	void 				setMax(int max);
	int					getMin();
	int					getMax();
	void 				setRange(int min,int max);
	void				setTriggerThreshold(float threshold);
	float 				getReading();
	float				getReferenceReading();
	float				getThreshold();
	float				getFilteredReading();
	float				getFilteredReading(KF filter);
	void 				setEnable(bool flag);
	void				updateReading();
	void				updateFilteredReading(KF filter);
	void				calibrate();
	void				calibrate(KF filter);
	bool				isNotInReferenceState();
	ReadingState		getState();
	void				setFilterConfig(float q,float r);
private:
	KF					filter;
	float				threshold;
	float				referenceReading;
	float				rawReading;
	float				filteredReading;
	bool 				isEnable;
	Adc 				adc;
	int 				min;
	int 				max;
};

#endif /* INC_MAGNETICSENSOR_H_ */
