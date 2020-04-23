#pragma once
#include "sequence.hpp"
#include <SDL.h>

class ButtonSetting : public Sequence {
private:
	SDL_Joystick *m_joystick;
	int m_cursorX, m_cursorY;
	int m_cusorWidth;
	int m_cursorHeight;
	vector<int> m_buttonValue;
	string filePath;
public:
	ButtonSetting(SDL_Renderer *renderer);
	~ButtonSetting();

	bool Update(SDL_Renderer *renderer);
};