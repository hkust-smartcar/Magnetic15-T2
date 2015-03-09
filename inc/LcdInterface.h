/*
 * LcdInterface.h
 *
 *  Created on: 18 Feb, 2015
 *      Author: Travis
 */

#ifndef LCDINTERFACE_H_
#define LCDINTERFACE_H_
#include <string>
#include <functional>
#include <libsc/k60/lcd_console.h>
#include <libsc/k60/lcd_typewriter.h>
#include <libsc/k60/lcd.h>
#include <libutil/misc.h>
class LcdInterface {
public:

	class Option{
		public:
			typedef std::function<void(*)(void)> Action;
			std::string text;
			Action *action;
	};
	class Menu{
		public:
		static const int MAX_OPTION=5;
			Option option[MAX_OPTION];
			Menu(int defaultIndex){
				currentIndex=libutil::ClampVal<int>(0,defaultIndex,MAX_OPTION-1);
				for(int i=0;i<MAX_OPTION;i++){
					option[i].text="Untitled";
					option[i].action=0;
				}
			}
		int getCurrentIndex(){
			return currentIndex;
		}
		void setCurrentIndex(int index){
			currentIndex=cyclicClamp(index);
		}
		private:
			int currentIndex;
			int cyclicClamp(int input){
				if(input<0){
					return MAX_OPTION;
				}
				if(input>MAX_OPTION-1)return 0;
				return input;
			}
	};
	LcdInterface(Menu* defaultMenu=0);
//	void highlightOption(int index);
	void highlightNextOption();
	void highlightPreviousOption();
	void selectOption();
	void useMenu(Menu *menu);
private:
	Menu* currentMenu;
	libsc::k60::St7735r lcd;
	libsc::k60::LcdTypewriter lcdWriter;
	libsc::k60::LcdConsole lcdConsole;
	void printLayout(int defaultId=0);
};

#endif /* LCDINTERFACE_H_ */
