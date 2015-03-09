/*
 * LcdInterface.cpp
 *
 *  Created on: 18 Feb, 2015
 *      Author: Travis
 */

#include "LcdInterface.h"
using namespace libsc::k60;
St7735r::Config getLcdConfig(){
	libsc::k60::St7735r::Config config;
	return config;
}

LcdTypewriter::Config getLcdWriterConfig(libsc::k60::St7735r *lcd){
	LcdTypewriter::Config config;
	config.lcd=lcd;
	config.is_text_wrap=false;
	return config;
}
LcdConsole::Config getLcdConsole(libsc::k60::St7735r *lcd){
	LcdConsole::Config config;
	config.lcd=lcd;
	return config;
}
LcdInterface::LcdInterface(Menu* defaultMenu)
	:lcd(getLcdConfig()),lcdWriter(getLcdWriterConfig(&lcd)),
	 lcdConsole(getLcdConsole(&lcd)){
	currentMenu=defaultMenu;
	printLayout(0);
}
/*
 * TODO fix the problem here
 */
void LcdInterface::printLayout(int defaultId){
//	Uint id=libutil::Clamp<Uint>(0,defaultId,Menu::MAX_OPTION-1);
	if(currentMenu==0)return;
//	lcd.Clear();
	uint16_t bgcolor=lcdWriter.GetBgColor();
	for(int i=0;i<Menu::MAX_OPTION;i++){
		lcdConsole.SetCursorRow(2*i);
		if(i==defaultId){
			lcdConsole.SetBgColor(0xFF00);
			lcdConsole.WriteString(currentMenu->option[i].text.c_str());
		}else
		lcdConsole.SetBgColor(bgcolor);
		lcdConsole.WriteString(currentMenu->option[i].text.c_str());
	}

}
void LcdInterface::useMenu(Menu* menu){
	currentMenu=menu;
	printLayout(0);
}
//void LcdInterface::highlightOption(int index){
//	if(index>=Menu::MAX_OPTION){
//		currentMenu->resetIndex();
//	}
//	if(index<=0)currentMenu->toLastIndex();
//	lcdConsole.SetCursorRow(index);
//	lcdConsole.SetBgColor(0xFF00);
//}
void LcdInterface::highlightNextOption(){
	int newIndex=currentMenu->getCurrentIndex()+1;
	printLayout(newIndex);
	currentMenu->setCurrentIndex(newIndex);

}
