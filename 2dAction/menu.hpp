#pragma once
#include "sequence.hpp"
#include "getInput.hpp"
#include <SDL.h>

class Menu : public Sequence {
private:
	GetInput m_eventInput;
	int m_cursorX, m_cursorY;
	int m_cusorWidth;
	int m_cursorHeight;
public:
	Menu(SDL_Renderer *renderer);
	~Menu();

	bool Update(SDL_Renderer *renderer);
};
