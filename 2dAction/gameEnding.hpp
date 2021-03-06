#pragma once
#include "sequence.hpp"
#include "getInput.hpp"
#include <SDL.h>

class GameEnding : public Sequence {
private:
	GetInput m_eventInput;
	int m_cursorX, m_cursorY;
	int m_cusorWidth;
	int m_cursorHeight;
public:
	GameEnding(SDL_Renderer *renderer, const char *resultMsg);
	~GameEnding();

	bool Update(SDL_Renderer *renderer);
};