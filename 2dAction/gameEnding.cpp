#include "stdafx.h"
#include "parameter.hpp"
#include "gameEnding.hpp"
#include "font.hpp"
#include <iostream>

using namespace std;

const int g_msgInterval = WINDOW_HEIGHT * MAP_CHIPSIZE / 5;

GameEnding::GameEnding(SDL_Renderer *renderer, const char *resultMsg) {
	const int msgNum = 4;
	Font *msg[msgNum];
	msg[0] = new Font(renderer, resultMsg, 100, "red");
	msg[1] = new Font(renderer, u8"メニューに戻る", 60, "black");
	msg[2] = new Font(renderer, u8"もう一度遊ぶ", 60, "black");
	msg[3] = new Font(renderer, u8"終了", 60, "black");
	msg[1]->QueryTexture(m_cusorWidth, m_cursorHeight);
	m_cursorX = WINDOW_WIDTH * MAP_CHIPSIZE / 2 - m_cusorWidth / 2;
	m_cursorY = 2;

	for (int i = 0; i < msgNum; i++) {
		int w, h;
		msg[i]->QueryTexture(w, h);
		SDL_Rect dstRect = { WINDOW_WIDTH * MAP_CHIPSIZE / 2 - w / 2, WINDOW_HEIGHT * MAP_CHIPSIZE / 5 * (i + 1), w, h };
		SDL_RenderCopy(renderer, msg[i]->ReturnTexture(), NULL, &dstRect);
		delete msg[i];
	}
	SDL_RenderPresent(renderer);
}

GameEnding::~GameEnding() {}

bool GameEnding::Update(SDL_Renderer *renderer) {
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
		if (--m_cursorY < 2) { m_cursorY = 4; }
		break;
	}
	case EVENT::JOY_HAT_DOWN: {
		if (++m_cursorY > 4) { m_cursorY = 2; }
		break;
	}
	case EVENT::JOY_CIRCLE: {
		switch (m_cursorY) {
		case 2: {
			m_next = SEQ_ID::MENU;
			break;
		}
		case 3: {
			m_next = SEQ_ID::GAME;
			break;
		}
		case 4: {
			m_next = SEQ_ID::QUIT;
			break;
		}
		}
		return false;
	}
	}

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_Rect dstDraw = {m_cursorX, g_msgInterval * m_cursorY, m_cusorWidth, m_cursorHeight};
	SDL_RenderDrawRect(renderer, &dstDraw);
	SDL_RenderPresent(renderer);
	return true;
}