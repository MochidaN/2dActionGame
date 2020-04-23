#pragma once
#include <SDL.h>
#include <vector>

enum class EVENT {
	//JOY_TRIANGLE,
	JOY_CIRCLE,
	JOY_CROSS,
	//JOY_SQUARE,
	JOY_R1,
	JOY_L1_UP,
	JOY_L1_DOWN,
	JOY_HAT_UP,
	JOY_HAT_DOWN,
	JOY_HAT_RIGHT,
	//JOY_HAT_RIGHTUP,
	//JOY_HAT_RIGHTDOWN,
	JOY_HAT_LEFT,
	//JOY_HAT_LEFTUP,
	//JOY_HAT_LEFTDOWN,
	JOY_HAT_CENTERED,
	QUIT,
	NONE
};

class GetInput {
private:
	SDL_Joystick *m_joystick;
	std::vector<int> m_eventValue;
public:
	GetInput();
	~GetInput();
	EVENT GetEvent();
};