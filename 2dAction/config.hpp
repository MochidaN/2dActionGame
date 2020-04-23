#pragma once
#include "sequence.hpp"
#include "getInput.hpp"
#include <SDL.h>

class Config : public Sequence {
private:
	GetInput m_eventInput;
	int m_cursorX, m_cursorY;
	int m_cusorWidth;
	int m_cursorHeight;
public:
	Config(SDL_Renderer *renderer);
	~Config();

	bool Update(SDL_Renderer *renderer);
};