#pragma once
#include "sequence.hpp"
#include <SDL.h>


class GameEnding : public Sequence {
private:
	SDL_Joystick * m_joystick;
	int m_cursorX, m_cursorY;
	int m_cusorWidth;
	int m_cursorHeight;
public:
	GameEnding(SDL_Renderer *renderer, const char *resultMsg);
	~GameEnding();

	bool Update(SDL_Renderer *renderer);
};