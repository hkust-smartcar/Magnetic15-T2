#include <libbase/k60/mcg.h>
#include <libbase/k60/ftm_pwm.h>
#include <libbase/k60/gpio.h>
#include <libsc/k60/led.h>
#include <libsc/k60/system.h>
#include <libsc/k60/alternate_motor.h>
#include <libsc/k60/tower_pro_mg995.h>
#include <libsc/k60/uart_device.h>
#include <libsc/k60/ftdi_ft232r.h>
#include <functional>
#include <string>
#include <libsc/k60/dir_motor.h>
#include <libsc/k60/encoder.h>
#include <Car.h>
using namespace libbase::k60;
using namespace libsc::k60;
libbase::k60::Mcg::Config libbase::k60::Mcg::GetMcgConfig(){
	libbase::k60::Mcg::Config config;
	config.external_oscillator_khz=50000;
	config.core_clock_khz=150000;
	return config;
}

int main(){
	System::Init();
	Car car;
	car.run();
	while(true);

	return 0;
}

