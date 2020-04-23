#include "stdafx.h"
#include "getInput.hpp"
#include "readFile.hpp"
#include <iostream>
#include <string>

using namespace std;
const short g_circle = 0;
const short g_cross = 1;
const short g_r1 = 2;
const short g_l1 = 3;
const short g_hatDown = 4;
const short g_hatUp = 5;
const short g_hatRight = 6;
const short g_hatLeft = 7;

GetInput::GetInput() {
	m_joystick = SDL_JoystickOpen(0);//ジョイスティックを構造体に割り当てて有効化
	if (!SDL_JoystickGetAttached(m_joystick)) { cout << "failed to open joystick" << endl; }

#ifdef _DEBUG
	string filePath("../txt/joyevent/input.txt");
#else
	string filePath("../../txt/joyevent/input.txt");
#endif
	m_eventValue = ReadFileSplit(filePath, ' ')[0];
}

GetInput::~GetInput() {
	if (SDL_JoystickGetAttached(m_joystick)) { SDL_JoystickClose(m_joystick); }
}

EVENT GetInput::GetEvent() {
	//まだ開いてないとき
	if (!SDL_JoystickGetAttached(m_joystick)) { m_joystick = SDL_JoystickOpen(0); }

	SDL_Event event;
	if (SDL_PollEvent(&event)) {//キューにイベントがあったとき
		switch (event.type) {
		case SDL_JOYBUTTONDOWN:
			if (SDL_JoystickGetButton(m_joystick, m_eventValue[g_circle])) {//〇ボタン
				return EVENT::JOY_CIRCLE;
			}
			else if (SDL_JoystickGetButton(m_joystick, m_eventValue[g_cross])) {//×ボタン
				return EVENT::JOY_CROSS;
			}
			else if (SDL_JoystickGetButton(m_joystick, m_eventValue[g_l1])) {//L1
				return EVENT::JOY_L1_DOWN;
			}
			else if (SDL_JoystickGetButton(m_joystick, m_eventValue[g_r1])) {//R1
				return EVENT::JOY_R1;
			}
			else if (SDL_JoystickGetButton(m_joystick, m_eventValue[g_hatUp])) {//上
				return EVENT::JOY_HAT_UP;
			}
			else if (SDL_JoystickGetButton(m_joystick, m_eventValue[g_hatDown])) {//下
				return EVENT::JOY_HAT_DOWN;
			}
			else if (SDL_JoystickGetButton(m_joystick, m_eventValue[g_hatLeft])) {//左
				return EVENT::JOY_HAT_LEFT;
			}
			else if (SDL_JoystickGetButton(m_joystick, m_eventValue[g_hatRight])) {//右
				return EVENT::JOY_HAT_RIGHT;
			}
			break;
		case SDL_JOYBUTTONUP:
			if ((int)event.jbutton.button == m_eventValue[g_l1]) {//L1
				return EVENT::JOY_L1_UP;
			}
			else if ((int)event.jbutton.button == m_eventValue[g_hatUp] || 
				(int)event.jbutton.button == m_eventValue[g_hatDown] || 
				(int)event.jbutton.button == m_eventValue[g_hatRight] || 
				(int)event.jbutton.button == m_eventValue[g_hatLeft]) {
				return EVENT::JOY_HAT_CENTERED;
			}
			break;
		case SDL_JOYHATMOTION: {//十字キーが押されたとき
			cout << "joy" << endl;
			switch (SDL_JoystickGetHat(m_joystick, 0)) {
			case SDL_HAT_UP:
				cout << "up" << endl;
				return EVENT::JOY_HAT_UP;
			case SDL_HAT_DOWN:
				return EVENT::JOY_HAT_DOWN;
			case SDL_HAT_RIGHT:
				return EVENT::JOY_HAT_RIGHT;
			case SDL_HAT_LEFT:
				return EVENT::JOY_HAT_LEFT;
			default:
				return EVENT::JOY_HAT_CENTERED;
			}
			break;
		}
		case SDL_QUIT: {//ウィンドウの×ボタン
			return EVENT::QUIT;
		}
		}
	}
	return EVENT::NONE;
}