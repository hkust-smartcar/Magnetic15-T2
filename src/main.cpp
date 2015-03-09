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
//void AlternateMotor::OnSetClockwise(bool flag){
//	m_is_clockwise=flag;
//}
//void AlternateMotor::OnSetPower(const uint16_t power){
//	m_pos_width=power;
//}

//void ReceiveListener(const Byte* bytes,const size_t size){
//	switch(*bytes){
//		case 'a':
//			servo->SetDegree(450);
//			motor->SetPower(motor_power);
//			break;
//		case 'd':
//			servo->SetDegree(1350);
//			motor->SetPower(motor_power);
//			break;
//		case 'w':
//			servo->SetDegree(900);
//			motor->SetClockwise(true);
//			motor->SetPower(motor_power);
//			break;
//		case 's':
//			servo->SetDegree(900);
//			motor->SetClockwise(false);
//			motor->SetPower(motor_power);
//			break;
//		case 'f':
//			motor_power+50<=MAX_POWER?motor_power+=50:motor_power=MAX_POWER;
//			break;
//		case 'g':
//			motor_power-50>=MIN_POWER?motor_power-=50:motor_power=MIN_POWER;
//			break;
//		default:
//			servo->SetDegree(900);
//			motor->SetPower(0);
//	}
//	return_status();
//}
int main(){
//			config.id =0;
//			led= Led(config);
//			led->SetEnable(true);
//			motor_config.id=0;
//			encoderConfig.id = 0;
//			encoder=new Encoder(encoderConfig);
//			motor=new DirMotor(motor_config);
//			power=2;
//			float Kp=0.7;
//			ppower=&power;
//			servo_config.id=0;
//			//servo=new TowerProMg995(servo_config);
//			//motor->SetPower(500);
//			const int SP=0;
//			acc = new libbase::k60::MMA8451Q();
//			if(acc->isConnected()){
//				for(int i=0;i<10;i++){
//					led->Switch();
//					System::DelayMs(2000);
//				}
//			}else{
//
//			}
			System::Init();
//			Led led[4];
//			for(int i=0;i<4;i++)
//			{
//				Led::Config config;
//				config.id=i;
//
//				led[i](config);
//				led[i].SetEnable(true);
//			}

//			Gpo::Config gpio_conf;
//			gpio_conf.pin = Pin::Name::kPta10;
//			gpio_conf.is_high = true;
//			Gpo gpo(gpio_conf);
			Car car;
			car.run();
//			Probe probe=new Probe(ppower);
//			Probe::setBroadcaster<const char*>(FtdiFt232r::SendStr());
//			bt_config.id=0;
//			bt_config.baud_rate=libbase::k60::Uart::Config::BaudRate::k115200;
//			bt=new FtdiFt232r(bt_config);
//			//bt->EnableRx(&ReceiveListener);
//			int currentMotorPower=100;
//			motor->SetPower(100);
//			int encoderCount=encoder->GetCount();
//			int ratio=100/encoderCount;
//			while(true){
//				//led->Switch();
//					// Update the encoder value
//				encoder->Update();
//				// Get the encoder count between this Update() and
//				// the previous Update()
//				System::DelayMs(20);
//				encoder->Update();
//				encoderCount = encoder->GetCount();
//				int err=SP-encoderCount;
//				//Since encoderCount is negative, -=is needed
//				currentMotorPower-=(int)Kp*err*ratio;
//				motor->SetPower((int)currentMotorPower);
		while(true);
//}
	return 0;
}

