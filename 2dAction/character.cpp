#include "stdafx.h"
#include "parameter.hpp"
#include "character.hpp"
#include <algorithm>

using namespace std;
const short ANIME_INTERVAL = 1000 / 10;//アニメーション間隔
const short X = 0, Y = 1, W = 2, H = 3;

Character::Character() {}
Character::~Character() {}

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

void Character::Update() {
	cout << "please override this function" << endl;
	exit(1);
}

bool Character::UpdateAnimation(unsigned int nowTime, vector<int> maxFrame) {
	bool endAnimation = false;
	if (nowTime - m_prevTime >= ANIME_INTERVAL) {
		//アニメーション更新
		vector<short> frame = ReturnNextFrame(m_heng, m_vertical, maxFrame, endAnimation);
		m_heng = frame[0];
		m_vertical = frame[1];
		m_prevTime = nowTime;
	}
	return endAnimation;
}

void Character::MovePos(vector<int> myHurtRect, vector<vector<int>> mapData) {
	//左向きなら当たり判定反転
	if (m_dir == g_left) { myHurtRect = FlipRect(myHurtRect, m_pos.w); }
	
	if (m_xAdd != 0) {//x方向の移動があるとき
		m_pos = MovePositionX(m_pos, myHurtRect, m_xAdd * m_dir, WORLD_WIDTH, mapData);
	}
	m_pos = MovePositionY(m_pos, myHurtRect, m_yAdd, WORLD_HEIGHT, mapData);
}

void Character::CollisionChara(vector<int> myHurtRect, vector<int> oppHurtRect, vector<vector<int>> mapData, Character &opponent) {	
	//左向きなら当たり判定反転
	if (m_dir == g_left) { myHurtRect = FlipRect(myHurtRect, m_pos.w); }
	if (opponent.GetState(CHARA_STATE::DIR) == g_left) { oppHurtRect = FlipRect(oppHurtRect, opponent.GetPos().w); }

	SDL_Rect myPos = ReturnCharaRect(m_pos, myHurtRect);
	SDL_Rect oppPos = ReturnCharaRect(opponent.GetPos(), oppHurtRect);
	if (DetectCollisionRect(myPos, oppPos) == true) {//2キャラクタがぶつかっている
		SDL_Rect my = m_pos;
		SDL_Rect opp = opponent.GetPos();
		const short dst = g_addX;
		int myDst = (myPos.x <= oppPos.x) ? -dst : dst;
		int oppDst = (myPos.x <= oppPos.x) ? dst : -dst;
		SetPos(MovePositionX(my, myHurtRect, myDst, WORLD_WIDTH, mapData));
		opponent.SetPos(MovePositionX(opp, oppHurtRect, oppDst, WORLD_WIDTH, mapData));
	}
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
		if (state == g_right || state == g_left) { m_dir = state; }
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
		m_hurtActive = 0;
		m_attackActive = 0;
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
	else if (request == CHARA_STATE::ATTACK_ACTIVE) {
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


float CalculateSquaredDistance(SDL_Rect r0, SDL_Rect r1) {
	float distanceX = (r0.x + (r0.w / 2)) - (r1.x + (r1.w / 2));
	float distanceY = (r0.y + (r0.h / 2)) - (r1.y + (r1.h / 2));
	return distanceX * distanceX + distanceY * distanceY;
}

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

int ReturnFrameNum(int maxFrameHeng, Character chara) {
	return chara.GetState(CHARA_STATE::FRAME_V) * maxFrameHeng + chara.GetState(CHARA_STATE::FRAME_H);
}

inline vector<int> FlipRect(vector<int> rect, int imgWidth) {
	rect[X] = imgWidth - (rect[X] + rect[W]);
	return rect;
}

SDL_Rect MovePositionX(SDL_Rect nowPos, vector<int> hurtRect, int distance, int worldWidth, vector<vector<int>> mapData) {
	const int prevX = nowPos.x;
	nowPos.x += distance;
	const int charaPos = nowPos.x + hurtRect[X];
	if (charaPos < 0) { nowPos.x = -hurtRect[X]; }
	else if (charaPos + hurtRect[W] > worldWidth * MAP_CHIPSIZE) {
		nowPos.x = worldWidth * MAP_CHIPSIZE - (hurtRect[X] + hurtRect[W]);
	}

	CollisionMapX(nowPos, prevX, hurtRect, mapData);
	return nowPos;
}

SDL_Rect MovePositionY(SDL_Rect nowPos, vector<int> hurtRect, short &yAdd, int worldHeight, vector<vector<int>> mapData) {
	yAdd++;
	const int prevY = nowPos.y;
	nowPos.y += yAdd;

	const int charaPos = nowPos.y + hurtRect[Y];
	if (charaPos < 0) {
		nowPos.y = -hurtRect[Y];
	}
	else if (charaPos + hurtRect[H] > worldHeight * MAP_CHIPSIZE) {
		nowPos.y = worldHeight * MAP_CHIPSIZE - (hurtRect[Y] + hurtRect[H]);
	}
	if (CollisionMapY(nowPos, nowPos.y, hurtRect, mapData)) {
		yAdd = g_yAdd_ground;
	}
	
	return nowPos;
}

void CollisionMapX(SDL_Rect &nowPos, int prevX, vector<int> hurtRect, vector<vector<int>> mapData) {
	const short left = (nowPos.x + hurtRect[X]) / MAP_CHIPSIZE;
	short right = (nowPos.x + hurtRect[X] + hurtRect[W] + MAP_CHIPSIZE - 1) / MAP_CHIPSIZE;
	if (right > WORLD_WIDTH) { right = WORLD_WIDTH; }

	const short top = (nowPos.y + hurtRect[Y]) / MAP_CHIPSIZE;
	short bottom = (nowPos.y + hurtRect[Y] + hurtRect[H]) / MAP_CHIPSIZE;
	if (bottom > WORLD_HEIGHT) { bottom = WORLD_HEIGHT; }
	
	for (int w = left; w < right; w++) {
		for (int h = top; h < bottom; h++) {
			if (mapData[w][h] >= 1) {
				//int x = max(nowPos.x, prevX);
				//x /= MAP_CHIPSIZE;
				//if (nowPos.x - prevX < 0) { x++; }
				//x *= MAP_CHIPSIZE;
				nowPos.x = prevX;//x;
			}
		}
	}
}

bool CollisionMapY(SDL_Rect &nowPos, int prevY, vector<int> hurtRect, vector<vector<int>> mapData) {
	bool landing = false;//着地したか否か
	const short left = (nowPos.x + hurtRect[X]) / MAP_CHIPSIZE;
	short right = (nowPos.x + hurtRect[X] + hurtRect[W] + MAP_CHIPSIZE - 1) / MAP_CHIPSIZE;
	if (right > WORLD_WIDTH) { right = WORLD_WIDTH; }
	const short top = (nowPos.y + hurtRect[Y]) / MAP_CHIPSIZE;
	short bottom = (nowPos.y + hurtRect[Y] + hurtRect[H] + MAP_CHIPSIZE - 1) / MAP_CHIPSIZE;
	if (bottom > WORLD_HEIGHT) { bottom = WORLD_HEIGHT; }
	cout << top << "  " << nowPos.y << "  " << hurtRect[Y] << endl<< endl;
	for (int w = left; w < right; w++) {
		for (int h = top; h < bottom; h++) {
			if (mapData[w][h] >= 1) {
				int y = std::max(nowPos.y, prevY) + hurtRect[Y] + hurtRect[H];
				y /= MAP_CHIPSIZE;
				y *= MAP_CHIPSIZE;
				nowPos.y = y - hurtRect[Y] - hurtRect[H];

				landing = true;
			}
		}
	}

	return landing;
}

SDL_Rect ReturnCharaRect(SDL_Rect pos, vector<int> rect) {
	pos.x += rect[X];
	pos.y += rect[Y];
	pos.w = rect[W];
	pos.h = rect[H];

	return pos;
}

bool DetectCollisionRect(SDL_Rect r0, SDL_Rect r1) {
	float distanceX = abs((r0.x + (r0.w / 2)) - (r1.x + (r1.w / 2)));
	float distanceY = abs((r0.y + (r0.h / 2)) - (r1.y + (r1.h / 2)));

	return (distanceX < (float)((r0.w + r1.w) / 2) && distanceY < (float)((r0.h + r1.h) / 2)) ? true : false;
}

void SetAction(Character &chara, short nextAction, unsigned int hurtActive, unsigned int atkActive) {
	chara.SetState(CHARA_STATE::ACTION, nextAction);
	chara.SetActiveBit(CHARA_STATE::HURT_ACTIVE, hurtActive);
	chara.SetActiveBit(CHARA_STATE::ATTACK_ACTIVE, atkActive);
}