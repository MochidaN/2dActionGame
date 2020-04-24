#include "stdafx.h"
#include "parameter.hpp"
#include "menu.hpp"
#include "font.hpp"
#include <iostream>

using namespace std;

const int g_msgInterval = WINDOW_HEIGHT * MAP_CHIPSIZE / 5;
const int g_msgNum = 3;

Menu::Menu(SDL_Renderer *renderer) {
	SDL_Color c = ConvertToRGB("white");
	SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
	SDL_RenderFillRect(renderer, NULL);

	Font *msg[g_msgNum];
	msg[0] = new Font(renderer, u8"ゲームスタート", 60, "black");
	msg[1] = new Font(renderer, u8"ボタン設定", 60, "black");
	msg[2] = new Font(renderer, u8"終了", 60, "black");
	msg[0]->QueryTexture(m_cusorWidth, m_cursorHeight);
	m_cursorX = WINDOW_WIDTH * MAP_CHIPSIZE / 2 - m_cusorWidth / 2;
	m_cursorY = 1;

	for (int i = 0; i < g_msgNum; i++) {
		int w, h;
		msg[i]->QueryTexture(w, h);
		SDL_Rect dstRect = { WINDOW_WIDTH * MAP_CHIPSIZE / 2 - w / 2, WINDOW_HEIGHT * MAP_CHIPSIZE / 5 * (i + 1), w, h };
		SDL_RenderCopy(renderer, msg[i]->ReturnTexture(), NULL, &dstRect);
		delete msg[i];
	}
	SDL_RenderPresent(renderer);
}

Menu::~Menu() {}

bool Menu::Update(SDL_Renderer *renderer) {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_Rect dstClear = { m_cursorX, g_msgInterval * m_cursorY, m_cusorWidth, m_cursorHeight };
	SDL_RenderDrawRect(renderer, &dstClear);

	EVENT event = m_eventInput.GetEvent();
	switch (event) {
	case EVENT::QUIT: {
		m_next = SEQ_ID::QUIT;
		return false;
	}
	case EVENT::JOY_HAT_UP: {
		if (--m_cursorY < 1) { m_cursorY = g_msgNum; }
		break;
	}
	case EVENT::JOY_HAT_DOWN: {
		if (++m_cursorY > g_msgNum) { m_cursorY = 1; }
		break;
	}
	case EVENT::JOY_CIRCLE: {
		switch (m_cursorY) {
		case 1: {
			m_next = SEQ_ID::GAME;
			break;
		}
		case 2: {
			m_next = SEQ_ID::BUTTON_SETTING;
			break;
		}
		case 3: {
			m_next = SEQ_ID::QUIT;
			break;
		}
		}
		return false;
	}
	}

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_Rect dstDraw = { m_cursorX, g_msgInterval * m_cursorY, m_cusorWidth, m_cursorHeight };
	SDL_RenderDrawRect(renderer, &dstDraw);
	SDL_RenderPresent(renderer);
	return true;
}