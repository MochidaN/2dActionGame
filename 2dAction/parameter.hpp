#pragma once

const short MAP_CHIPSIZE = 64;

const short WINDOW_WIDTH = 15;
const short WINDOW_HEIGHT = 12;
const short WORLD_WIDTH = 100;
const short WORLD_HEIGHT = 12;

const short PLAYER_SIZE = 350;
const short ENEMY_SIZE = 256;

const short g_yAdd_ground = -1;

enum class EVENT {
	//JOY_TRIANGLE,
	JOY_CIRCLE,
	JOY_CROSS,
	//JOY_SQUARE,
	JOY_R1,
	JOY_L1_UP,
	JOY_L1_DOWN,
	//JOY_HAT_UP,
	//JOY_HAT_DOWN,
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