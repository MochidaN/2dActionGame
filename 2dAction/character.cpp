#include "stdafx.h"
#include "character.hpp"

using namespace std;

Character::Character() {}

Character::~Character() {
}

void Character::InitState(short dir, short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd) {
	m_dir = dir;
	m_hp = hp;
	m_power = power;
	m_defense = defense;
	SetPos(pos);
	m_vertical = 0;
	m_heng = 0;
	m_action = action;
	m_xAdd = 0;
	m_yAdd = yAdd;
	m_prevTime = time;
	m_hurtActive = 0;
	m_attackActive = 0;
}

const short Character::GetState(CHARA_STATE request) {
	switch (request) {
	case CHARA_STATE::DIR: {
		return m_dir;
	}
	case CHARA_STATE::HP: {
		return m_hp;
	}
	case CHARA_STATE::POW: {
		return m_power;
	}
	case CHARA_STATE::DEF: {
		return m_defense;
	}
	case CHARA_STATE::FRAME_V: {
		return m_vertical;
	}
	case CHARA_STATE::FRAME_H: {
		return m_heng;
	}
	case CHARA_STATE::ACTION: {
		return m_action;
	}
	case CHARA_STATE::X_ADD: {
		return m_xAdd;
	}
	case CHARA_STATE::Y_ADD: {
		return m_yAdd;
	}
	default: {
		cout << "the state is invalid" << endl;
		return -1;
	}
	}
}

void Character::SetState(CHARA_STATE request, short state) {
	switch (request) {
	case CHARA_STATE::DIR: {
		if (state == g_right || state == g_left) {
			m_dir = state;
		}
		break;
	}
	case CHARA_STATE::HP: {
		m_hp = state;
		break;
	}
	case CHARA_STATE::POW: {
		m_power = state;
		break;
	}
	case CHARA_STATE::DEF: {
		m_defense = state;
		break;
	}
	case CHARA_STATE::FRAME_V: {
		m_vertical = state;
		break;
	}
	case CHARA_STATE::FRAME_H: {
		m_heng = state;
		break;
	}
	case CHARA_STATE::ACTION: {
		m_action = state;
		m_vertical = 0;//コマ数リセット
		m_heng = 0;
		break;
	}
	case CHARA_STATE::X_ADD: {
		m_xAdd = state;
		break;;
	}
	case CHARA_STATE::Y_ADD: {
		m_yAdd = state;
		break;
	}
	default: {
		cout << "the state is not exist" << endl;
		break;
	}
	}
}

const SDL_Rect Character::GetPos() {
	return m_pos;
}

void Character::SetPos(SDL_Rect nextPos) {
	m_pos.x = nextPos.x;
	m_pos.y = nextPos.y;
	m_pos.w = nextPos.w;
	m_pos.h = nextPos.h;
}

const unsigned int Character::GetTime() {
	return m_prevTime;
}

void Character::SetTime(unsigned int time) {
	m_prevTime = time;
}

const unsigned int Character::GetActiveBit(CHARA_STATE request, int frame) {
	if (request == CHARA_STATE::HURT_ACTIVE) {
		return m_hurtActive & (1 << frame);
	}
	else if(request == CHARA_STATE::ATTACK_ACTIVE) {
		return m_attackActive & (1 << frame);
	}
	else {
		cout << "the state is invalid" << endl;
		return 0;
	}
}

void Character::SetActiveBit(CHARA_STATE request, unsigned int bit) {
	if (request == CHARA_STATE::HURT_ACTIVE) {
		m_hurtActive = bit;
	}
	else if (request == CHARA_STATE::ATTACK_ACTIVE) {
		m_attackActive = bit;
	}
	else {
		cout << "the state is not exist" << endl;
	}
}



Player::Player(short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd) {
	Character::InitState(g_right, hp, power, defense, pos, action, time, yAdd);
}
/*
const short Player::GetState(CHARA_STATE request) {
	return Character::GetState(request);
}

void Player::SetState(CHARA_STATE request, int state) {
	Character::SetState(request, state);
}*/



Enemy::Enemy(short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd) {
	Character::InitState(g_left, hp, power, defense, pos, action, time, yAdd);
	m_inWindow = false;
}
/*
const short Enemy::GetState(CHARA_STATE request) {
	return Character::GetState(request);
}

void Enemy::SetState(CHARA_STATE request, int state) {
	Character::SetState(request, state);
}
*/

//このキャラクターがウィンドウ内にいるか返す
const bool Enemy::GetInWindow() {
	return m_inWindow;
}

//ウィンドウ内にいるかの状態更新
void Enemy::SetInWindow(bool flag) {
	m_inWindow = flag;
}