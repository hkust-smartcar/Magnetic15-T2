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
using namespace libbase::k60;
class MagneticSensor {
public:
	typedef 			std::function<void(Adc *adc, const uint16_t result)>
						OnConversionCompleteListener;

	explicit 			MagneticSensor(Pin::Name pin);
	explicit 			MagneticSensor(libbase::k60::Adc::Config config);

	static 	Adc::Config	getAdcConfig(libbase::k60::Pin::Name pin){
				Adc::Config config;
					config.pin=pin;
					config.is_continuous_mode=false;
					config.is_diff_mode=false;
					config.speed=Adc::Config::SpeedMode::kTypical;
					config.avg_pass=Adc::Config::AveragePass::k4;
					config.resolution=Adc::Config::Resolution::k8Bit;
				return config;
	}
	enum				ReadingState
	{
			IN_REFERENCE_STATE,
			BELOW_REFERENCE_STATE,
			ABOVE_REFERENCE_STATE,
			IS_MINIMAL,
			IS_MAXIMAL
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
	float				getFilteredReading(KF filter);
	void 				setEnable(bool flag);
	void				updateReading();
	void				updateFilteredReading(KF filter);
	void				calibrate();
	void				calibrate(KF filter);
	bool				isNotInReferenceState();
	ReadingState		getState();
private:
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
