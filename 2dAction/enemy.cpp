#include "stdafx.h"
#include "enemy.hpp"

Enemy::Enemy(short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd) {
	Character::InitState(g_left, hp, power, defense, pos, action, time, yAdd);
	//m_inWindow = false;
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
/*
const bool Enemy::GetInWindow() {
	return m_inWindow;
}*/

//ウィンドウ内にいるかの状態更新
/*
void Enemy::SetInWindow(bool flag) {
	m_inWindow = flag;
}*/

vector<short> ReturnNextFrame(short hengFrame, short verticalFrame, vector<int> maxFrame, bool &endAnimation) {
	endAnimation = false;
	if (++hengFrame >= maxFrame[0]) {
		hengFrame = 0;
		if (++verticalFrame >= maxFrame[1]) {
			verticalFrame = 0;
			endAnimation = true;
		}
	}

	vector<short> nextFrame = { hengFrame, verticalFrame };
	return nextFrame;
}