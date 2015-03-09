/*
 * PID.h
 *
 *  Created on: 17 Feb, 2015
 *      Author: Travis
 */
#ifndef INC_PID_H_
#define INC_PID_H_

#include <libsc/k60/timer.h>
#include <libsc/k60/system.h>
#include <functional>
#include <list>
class GeneralPID{

};
template<typename T>
class PID :GeneralPID{
public:
	enum Controller{
		P=0,
		I,
		D
	};
	typedef uint32_t TimerInt;
	/*
	 * The Operator checks if the specified condition is specified
	 */
	typedef std::function<bool(void)> Operator;
	/*
	 * The automat sets the value of particular control variables when Operator returns true
	 */
	typedef std::function<void(Controller,Operator,float)> Automat;
	typedef std::list<Automat> AutomatList;
	enum Precision
	{
					/*
					 * LOW precision=Riemann sum
					 * MEDIUM precision=Trapezoidal rule
					 * HIGH precision= Simpson's rule
					 *
					 * Note that:
					 * 1. Higher precision implies lower efficiency
					 * 2. Applying Simpson's Rule requires the error between the sampling time,
					 *    which is often difficult to find. Therefore such precision mode is disabled
					 *    until further fixes.
					 */
					LOW,
					MEDIUM,
					HIGH
	};

	PID(float Kp, float Ki, float Kd,T sp,Precision precision=LOW):
		m_kp(Kp),m_ki(Ki),m_kd(Kd),m_sp(sp),m_last_error(0),
		m_integral(0),m_differential(0),
		useKp(true),useKi(false),useKd(false),
		m_initTime(libsc::k60::System::Time()),
		m_lastTime(m_initTime),
		m_precision(precision),
		m_refreshInterval(0),
		useAutomat(false),
		m_lastUpdate(libsc::k60::System::Time())
		{};
	PID():PID(0,0,0,(T)0){};
	T 				getTunedValue(T input)
					{
						if(!useKp&&!useKi&&!useKd) return input; //this case is equivalent to disabling the controller
						/*
						 * fetch dt, error and initialize tuning procedure
						 */
						libsc::k60::Timer::TimerInt dt=getDt();
						T error=m_sp-getLastError(); //when observed value is lower than the set point,
											//the error will be positive. so that the proportional
											//controller will compensate the error by adding the differences
						T result=input;
						/*
						 * main procedure: calculate tuned result
						 */
						if(useKp){
							result+=m_kp*error;
						}
						if(useKi){
							updateIntegral(error,dt);
							result+=m_ki*m_integral;
						}
						if(useKd){
							updateDifferential(error,dt);
							result+=m_kd*m_differential;
						}
						/*
						 * update variables and return the result
						 */
						m_lastTime=libsc::k60::System::Time();
						return result;
					};
	T 				getLastError()
					{
						return m_last_error;
					};
	void 			setControlValue(Controller controller,float value)
					{
						switch(controller){
							case P:
								m_kp=value;
								break;
							case I:
								m_ki=value;
								break;
							case D:
								m_kd=value;
								break;
						}
					};
	void 			setSp(T sp)
					{
						m_sp=sp;
					};
	void 			toggleController(Controller controller, bool flag)
					{
						switch(controller){
							case P:
								useKp=flag;
								break;
							case I:
								useKi=flag;
								break;
							case D:
								useKd=flag;
								break;
						}
					};
	bool 			getControllerState(Controller controller)
					{
						switch(controller){
							case P:
								return useKp;
								break;
							case I:
								return useKi;
								break;
							case D:
								return useKd;
								break;
						}
					}
	void 			setPrecision(Precision precision)
					{
						m_precision=precision;
					}
	void 			addAutomat(Automat automat)
					{
						m_automatList.push_back(automat);
					};
	void 			removeAutomat(Automat automat)
					{
						m_automatList.remove(automat);
					};
	void 			setRefreshInterval(TimerInt interval)
					{
						m_refreshInterval=interval;
					};
	TimerInt 		getRefreshInterval();
	void 			checkAutomat()
					{
						if(m_refreshInterval>=(libsc::k60::System::Time()-m_lastUpdate)
								&&!m_automatList.empty()){
							for(typename AutomatList::iterator it=m_automatList.begin();
									it!=m_automatList.end();it++){
								*it;
							}
							m_lastUpdate=libsc::k60::System::Time();
						}
					};
	void			setAutomatEnable(bool flag)
					{
						useAutomat=flag;
					};
private:
	float 			m_kp,
					m_ki,
	 				m_kd,
					m_sp;

	bool 			useKp,
					useKi,
					useKd,
					useAutomat;
	Precision 		m_precision;

	libsc::k60::Timer::TimerInt getDt()
	{
		return libsc::k60::System::Time()-m_lastTime;
	};
	void 			updateIntegral(T error, libsc::k60::Timer::TimerInt dt)
	{
						switch(m_precision){
							case LOW:									//Riemann sum
								m_integral=error*dt;
								break;
							case MEDIUM:								//Trapezoidal rule
							case HIGH:									//Simpson's Rule(deprecated)
								m_integral+=(m_last_error+error)*dt/2;
								break;
						}

					};
	void 			updateDifferential(T error, libsc::k60::Timer::TimerInt dt)
					{
						m_differential=(error-m_last_error)/dt;
					};
	/*
	 * Automat configuration: Tune params under specified conditions
	 */
	const libsc::k60::Timer::TimerInt m_initTime;
	libsc::k60::Timer::TimerInt m_lastTime;
	TimerInt 					m_refreshInterval;
	TimerInt 					m_lastUpdate;
	AutomatList 				m_automatList;
	T 							m_integral;
	T 							m_differential;
	T 							m_last_error;

};

#endif /* INC_PID_H_ */
