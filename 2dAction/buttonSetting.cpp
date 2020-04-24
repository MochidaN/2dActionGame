#include "stdafx.h"
#include "parameter.hpp"
#include "buttonSetting.hpp"
#include "font.hpp"
#include "readFile.hpp"
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

const int g_msgNum = 11;
const int g_msgInterval = WINDOW_HEIGHT * MAP_CHIPSIZE / g_msgNum;

void writeFile(string fileName, vector<int> data);

ButtonSetting::ButtonSetting(SDL_Renderer *renderer) {
	m_joystick = SDL_JoystickOpen(0);//ジョイスティックを構造体に割り当てて有効化
	if (!SDL_JoystickGetAttached(m_joystick)) { cout << "failed to open joystick" << endl; }
#ifdef _DEBUG
	filePath = "../txt/joyevent/input.txt";
#else
	filePath = "../../txt/joyevent/input.txt";
#endif
	m_buttonValue = ReadFileSplit(filePath, ' ')[0];

	SDL_Color c = ConvertToRGB("white");
	SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
	SDL_RenderFillRect(renderer, NULL);

	Font *msg[g_msgNum];
	int i = 0;
	msg[i++] = new Font(renderer, u8"各キーに対応するボタンを押してください", 30, "red");
	msg[i++] = new Font(renderer, u8"〇ボタン", 30, "black");
	msg[i++] = new Font(renderer, u8"Xボタン", 30, "black");
	msg[i++] = new Font(renderer, u8"R１", 30, "black");
	msg[i++] = new Font(renderer, u8"L１", 30, "black");
	msg[i++] = new Font(renderer, u8"方向キー↓", 30, "black");
	msg[i++] = new Font(renderer, u8"方向キー↑", 30, "black");
	msg[i++] = new Font(renderer, u8"方向キー→", 30, "black");
	msg[i++] = new Font(renderer, u8"方向キー←", 30, "black");
	msg[i++] = new Font(renderer, u8"決定", 30, "blue");
	msg[i++] = new Font(renderer, u8"変更せず戻る", 30, "blue");
	msg[10]->QueryTexture(m_cusorWidth, m_cursorHeight);
	m_cursorX = 0;
	m_cursorY = 1;

	for (int i = 0; i < g_msgNum; i++) {
		int w, h;
		msg[i]->QueryTexture(w, h);
		SDL_Rect dstRect = { 0, g_msgInterval * i, w, h };
		SDL_RenderCopy(renderer, msg[i]->ReturnTexture(), NULL, &dstRect);
		delete msg[i];

		if (1 <= i && i <= 8) {
			ostringstream oss;
			oss << m_buttonValue[i - 1];
			string str = oss.str();
			msg[i] = new Font(renderer, str.c_str(), 30, "green");
			int width, height;
			msg[i]->QueryTexture(width, height);
			SDL_Rect dst = { m_cusorWidth, g_msgInterval * i, width, height };
			SDL_RenderCopy(renderer, msg[i]->ReturnTexture(), NULL, &dst);
			delete msg[i];
		}
	}
	SDL_RenderPresent(renderer);
}

ButtonSetting::~ButtonSetting() {
	if (SDL_JoystickGetAttached(m_joystick)) { SDL_JoystickClose(m_joystick); }
}

bool ButtonSetting::Update(SDL_Renderer *renderer) {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_Rect dstClear = { m_cursorX, g_msgInterval * m_cursorY, m_cusorWidth, m_cursorHeight };
	SDL_RenderDrawRect(renderer, &dstClear);

	SDL_Event event;
	if (SDL_PollEvent(&event)) {//キューにイベントがあったとき
		switch (event.type) {
		case SDL_JOYBUTTONDOWN: {
			m_buttonValue[m_cursorY - 1] = (int)event.jbutton.button;
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_Rect dst = { m_cusorWidth, g_msgInterval * m_cursorY, WINDOW_WIDTH * MAP_CHIPSIZE, m_cursorHeight };
			SDL_RenderFillRect(renderer, &dst);

			ostringstream oss;
			oss << m_buttonValue[m_cursorY - 1];
			string str = oss.str();
			Font msg(renderer, str.c_str(), 30, "green");
			int width, height;
			msg.QueryTexture(width, height);
			SDL_Rect dstRect = { m_cusorWidth, g_msgInterval * m_cursorY, width, height };
			SDL_RenderCopy(renderer, msg.ReturnTexture(), NULL, &dstRect);
			break;
		}
		case SDL_QUIT: {//ウィンドウの×ボタン
			m_next = SEQ_ID::QUIT;
			return false;
		}
		case SDL_KEYDOWN: {
			if (event.key.keysym.sym == SDLK_DOWN) {
				if (++m_cursorY > g_msgNum - 1) { m_cursorY = 1; }
			}
			else if (event.key.keysym.sym == SDLK_UP) {
				if (--m_cursorY < 1) { m_cursorY = g_msgNum - 1; }
			}
			else if (event.key.keysym.sym == SDLK_KP_ENTER || event.key.keysym.sym == SDLK_RETURN) {
				if (m_cursorY == g_msgNum - 2) {
					writeFile(filePath, m_buttonValue);
					m_next = SEQ_ID::MENU;
					return false;
				}
				else if (m_cursorY == g_msgNum - 1) {
					m_next = SEQ_ID::MENU;
					return false;
				}
			}
			break;
		}
		}
	}

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_Rect dstDraw = { m_cursorX, g_msgInterval * m_cursorY, m_cusorWidth, m_cursorHeight };
	SDL_RenderDrawRect(renderer, &dstDraw);
	SDL_RenderPresent(renderer);
	return true;
}

void writeFile(string fileName, vector<int> data) {
	ofstream outputFile;
	outputFile.open(fileName, ios::trunc);

	for (int i = 0, end = data.size(); i < end; i++) {
		outputFile << data[i] << ' ';
	}
	outputFile << endl;
}