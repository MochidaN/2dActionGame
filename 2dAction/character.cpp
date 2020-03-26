#include "stdafx.h"
#include "character.hpp"

using namespace std;

Character::Character() {}

Character::~Character() {
}

void Character::InitState(bool dir, short hp, short power, short defense, short agility, SDL_Rect pos, short action, unsigned int time) {
	m_dir = dir;
	m_hp = hp;
	m_power = power;
	m_defense = defense;
	m_agility = agility;
	SetPos(pos);
	m_vertical = 0;
	m_heng = 0;
	m_action = action;
	m_prevTime = time;
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
	case CHARA_STATE::AGI: {
		return m_agility;
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
	case CHARA_STATE::TIME: {
		return m_prevTime;
	}
	default: {
		cout << "the state is invalid" << endl;
		return -1;
	}
	}
}

void Character::SetState(CHARA_STATE request, int state) {
	switch (request) {
	case CHARA_STATE::DIR: {
		m_dir = state;
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
	case CHARA_STATE::AGI: {
		m_agility = state;
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
		break;
	}
	case CHARA_STATE::TIME: {
		m_prevTime = state;
		break;
	}
	default: {
		cout << "the state is not exist" << endl;
		break;
	}
	}
}

void Character::SetPos(SDL_Rect nextPos) {
	m_pos.x = nextPos.x;
	m_pos.y = nextPos.y;
	m_pos.w = nextPos.w;
	m_pos.h = nextPos.h;
}

SDL_Rect Character::GetPos() {
	return m_pos;
}

Player::Player(short hp, short power, short defense, short agility, SDL_Rect pos, short action, unsigned int time) {
	Character::InitState(true, hp, power, defense, agility, pos, action, time);
}

const short Player::GetState(CHARA_STATE request) {
	return Character::GetState(request);
}

Enemy::Enemy(short hp, short power, short defense, short agility, SDL_Rect pos, short action, unsigned int time) {
	Character::InitState(false, hp, power, defense, agility, pos, action, time);
}

const short Enemy::GetState(CHARA_STATE request) {
	return Character::GetState(request);
}
