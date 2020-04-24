#include "stdafx.h"
#include "parameter.hpp"
#include "player.hpp"
#include "enemy.hpp"
#include "getinput.hpp"

const short X = 0, Y = 1, W = 2, H = 3;
const short g_playerStepX = 10;
const short g_yAdd = -28;
const short g_trunkMax = 50;

void CalculateDamage(Enemy &enemy, short damage, short playerPow);


Player::Player(short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd) {
	Character::InitState(g_right, hp, power, defense, pos, action, time, yAdd);
	m_trunk = g_trunkMax;

	m_cannotChangeAction = 0xffffffff;
	const short bit0 = sizeof(unsigned int)*8 - static_cast<int>(PLAYER::ACTION::NUM);
	m_cannotChangeAction = (m_cannotChangeAction << bit0) >> bit0;
	m_cannotChangeAction &= ~(1 << static_cast<int>(PLAYER::ACTION::STAND));//キャンセル可能なアクションはフラグを下げる
	m_cannotChangeAction &= ~(1 << static_cast<int>(PLAYER::ACTION::WALK));
	m_cannotChangeAction &= ~(1 << static_cast<int>(PLAYER::ACTION::GUARD));
	m_cannotChangeAction &= ~(1 << static_cast<int>(PLAYER::ACTION::VERTICAL_ATTACK));
}

void Player::Update(unsigned int nowTime, vector<vector<int>> maxFrame, vector<vector<vector<int>>> myHurtRect, vector<vector<vector<int>>> oppHurtRect, vector<unsigned int> hurtActive, vector<unsigned int> atkActive, vector<vector<int>> mapData, EVENT event) {
	bool endAnimation = UpdateAnimation(nowTime, maxFrame[Character::GetState(CHARA_STATE::ACTION)]);
	if (endAnimation == true) { ChangeAction(hurtActive, atkActive); }

	HandleEvent(endAnimation, event, myHurtRect[static_cast<int>(PLAYER::ACTION::WALK)][0], hurtActive, atkActive);
	const int maxHeng = maxFrame[static_cast<int>(GetState(CHARA_STATE::ACTION))][0];
	const int myFrameNum = ReturnFrameNum(maxHeng, *this);
	MovePos(myHurtRect[static_cast<int>(GetState(CHARA_STATE::ACTION))][myFrameNum], mapData);
}

void Player::ChangeAction(vector<unsigned int> hurtActive, vector<unsigned int> atkActive) {
	if (++m_trunk > g_trunkMax) { m_trunk = g_trunkMax; }
	PLAYER::ACTION action = static_cast<PLAYER::ACTION>(Character::GetState(CHARA_STATE::ACTION));
	switch (action) {
	case PLAYER::ACTION::WALK: { break; }
	case PLAYER::ACTION::HIT_AIR: {
		const short nextAction = static_cast<short>(PLAYER::ACTION::FALL_HIT);
		SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		break;
	}
	case PLAYER::ACTION::JUMP: {
		const short nextAction = static_cast<short>(PLAYER::ACTION::FALL);
		SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		break;
	}
	case PLAYER::ACTION::FALL:
	case PLAYER::ACTION::FALL_HIT: {
		if (GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground || GetState(CHARA_STATE::Y_ADD) == 0) {
			const short nextAction = static_cast<short>(PLAYER::ACTION::LANDING);
			SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
			SetState(CHARA_STATE::X_ADD, 0);
		}
		break;
	}
	case PLAYER::ACTION::FAINT: {
		const short nextAction = static_cast<short>(PLAYER::ACTION::FAINTING);
		SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		break;
	}
	case PLAYER::ACTION::FAINTING: {
		if (m_trunk == g_trunkMax) {
			const short nextAction = static_cast<short>(PLAYER::ACTION::STAND);
			SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		}
		break;
	}
	case PLAYER::ACTION::DOWN: {
		const short nextAction = static_cast<short>(PLAYER::ACTION::DEAD);
		SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		break;
	}
	case PLAYER::ACTION::DEAD: 
	case PLAYER::ACTION::GUARD: {
		break;
	}
	case PLAYER::ACTION::SIDE_ATTACK: {
		const short nextAction = static_cast<short>(PLAYER::ACTION::RIGID);
		SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		break;
	}
	case PLAYER::ACTION::JUST_GUARD: {
		if (m_enabledParry == true) {
			m_enabledParry = false;
			const short nextAction = static_cast<short>(PLAYER::ACTION::PARRY);
			SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		}
		else {
			const short nextAction = static_cast<short>(PLAYER::ACTION::GUARD);
			SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		}
		break;
	}
	default: {
		const short nextAction = static_cast<short>(PLAYER::ACTION::STAND);
		SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		SetState(CHARA_STATE::X_ADD, 0);
		break;
	}
	}
}

void Player::HandleEvent(bool endAnimation, EVENT event, vector<int> hurtRect, vector<unsigned int> hurtActive, vector<unsigned int> atkActive) {
	if ((m_cannotChangeAction & ((unsigned int)1 << GetState(CHARA_STATE::ACTION))) == 0) {
		switch (event) {
		case EVENT::JOY_CIRCLE: {
			if (GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground || GetState(CHARA_STATE::Y_ADD) == 0) {
				const short nextAction = static_cast<short>(PLAYER::ACTION::STEP);
				SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
				SetState(CHARA_STATE::X_ADD, g_playerStepX);
			}
			break;
		}
		case EVENT::JOY_CROSS: {
			if (GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground || GetState(CHARA_STATE::Y_ADD) == 0) {
				const short nextAction = static_cast<short>(PLAYER::ACTION::JUMP);
				SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
				SetState(CHARA_STATE::Y_ADD, g_yAdd);
			}
			break;
		}
		case EVENT::JOY_L1_DOWN: {
			if (GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground || GetState(CHARA_STATE::Y_ADD) == 0) {
				const short nextAction = static_cast<short>(PLAYER::ACTION::JUST_GUARD);
				SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
				SetState(CHARA_STATE::X_ADD, 0);
			}
			break;
		}
		case EVENT::JOY_L1_UP: {
			if (GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground || GetState(CHARA_STATE::Y_ADD) == 0) {
				const short nextAction = static_cast<short>(PLAYER::ACTION::STAND);
				SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
			}
			break;
		}
		case EVENT::JOY_R1: {
			if (GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground || GetState(CHARA_STATE::Y_ADD) == 0) {
				if (GetState(CHARA_STATE::ACTION) == static_cast<short>(PLAYER::ACTION::VERTICAL_ATTACK)) {
					const short nextAction = static_cast<short>(PLAYER::ACTION::SIDE_ATTACK);
					SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
					SetState(CHARA_STATE::X_ADD, 0);
				}
				else if (GetState(CHARA_STATE::ACTION) != static_cast<short>(PLAYER::ACTION::SIDE_ATTACK)) {
					const short nextAction = static_cast<short>(PLAYER::ACTION::VERTICAL_ATTACK);
					SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
					SetState(CHARA_STATE::X_ADD, 0);
				}
			}
			break;
		}
		case EVENT::JOY_HAT_LEFT: {
			const short action = GetState(CHARA_STATE::ACTION);
			if (action == static_cast<short>(PLAYER::ACTION::VERTICAL_ATTACK) || action == static_cast<short>(PLAYER::ACTION::WALK)) { break; }

			short nextAction = static_cast<short>(PLAYER::ACTION::WALK);
			if (GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground || GetState(CHARA_STATE::Y_ADD) == 0) {
				nextAction = static_cast<short>(PLAYER::ACTION::WALK);
			}
			else { nextAction = static_cast<short>(PLAYER::ACTION::FALL); }
			
			SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
			SetState(CHARA_STATE::X_ADD, g_addX);

			if (GetState(CHARA_STATE::DIR) == g_right) {//右向きから左向きになるとき
				SDL_Rect pos = GetPos();
				vector<int> fRect = FlipRect(hurtRect, pos.w);
				pos.x -= (fRect[X] - hurtRect[X]);
				SetPos(pos);
				SetState(CHARA_STATE::DIR, g_left);
			}
			break;
		}
		case EVENT::JOY_HAT_RIGHT: {
			const short action = GetState(CHARA_STATE::ACTION);
			if (action == static_cast<short>(PLAYER::ACTION::VERTICAL_ATTACK) || action == static_cast<short>(PLAYER::ACTION::WALK)) { break; }

			short nextAction = static_cast<short>(PLAYER::ACTION::WALK);
			if (GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground || GetState(CHARA_STATE::Y_ADD) == 0) {
				nextAction = static_cast<short>(PLAYER::ACTION::WALK); 
			}
			else { nextAction = static_cast<short>(PLAYER::ACTION::FALL); }

			SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
			SetState(CHARA_STATE::X_ADD, g_addX);

			if (GetState(CHARA_STATE::DIR) == g_left) {//左向きから右向きになるとき
				SDL_Rect pos = GetPos();
				vector<int> fRect = FlipRect(hurtRect, pos.w);
				pos.x -= (hurtRect[X] - fRect[X]);
				SetPos(pos);
				SetState(CHARA_STATE::DIR, g_right);
			}
			break;
		}
		case EVENT::JOY_HAT_CENTERED: {
			const short nextAction = static_cast<short>(PLAYER::ACTION::STAND);
			SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
			SetState(CHARA_STATE::X_ADD, 0);
			break;
		}
		}
	}
}

void Player::HandleAttack(Enemy &enemy, int myFrame, int oppFrame, vector<int> myAtkRect, vector<int> oppHurtRect) {
	const unsigned int atkActive = GetActiveBit(CHARA_STATE::ATTACK_ACTIVE, myFrame);
	const unsigned int hurtActive = enemy.GetActiveBit(CHARA_STATE::HURT_ACTIVE, oppFrame);
	if ((atkActive != 0) && (hurtActive != 0) && (enemy.GetState(CHARA_STATE::ACTION) != static_cast<short>(ENEMY::ACTION::GUARD))) {

		if (GetState(CHARA_STATE::DIR) == g_left) { myAtkRect = FlipRect(myAtkRect, GetPos().w); }
		if(enemy.GetState(CHARA_STATE::DIR) == g_left) { oppHurtRect = FlipRect(oppHurtRect, enemy.GetPos().w); }
		SDL_Rect myPos = ReturnCharaRect(GetPos(), myAtkRect);
		SDL_Rect oppPos = ReturnCharaRect(enemy.GetPos(), oppHurtRect);

		if (DetectCollisionRect(myPos, oppPos) == true) {//自分の攻撃が相手に当たった
			PLAYER::ACTION action = static_cast<PLAYER::ACTION>(Character::GetState(CHARA_STATE::ACTION));
			switch (action) {
			case PLAYER::ACTION::PARRY: {
				Parry(enemy);
				break;
			}
			case PLAYER::ACTION::SIDE_ATTACK: {
				SideAttack(enemy);
				break;
			}
			case PLAYER::ACTION::VERTICAL_ATTACK: {
				VerticalAttack(enemy);
				break;
			}
			}
		}
	}
}

void Player::Parry(Enemy &enemy) {
	const short damage = 20;
	CalculateDamage(enemy, damage, GetState(CHARA_STATE::POW));
	SetActiveBit(CHARA_STATE::ATTACK_ACTIVE, 0);
}

void Player::SideAttack(Enemy &enemy) {
	const short damage = 10;
	CalculateDamage(enemy, damage, GetState(CHARA_STATE::POW));
	SetActiveBit(CHARA_STATE::ATTACK_ACTIVE, 0);
}

void Player::VerticalAttack(Enemy &enemy) {
	const short damage = 10;
	CalculateDamage(enemy, damage, GetState(CHARA_STATE::POW));
	SetActiveBit(CHARA_STATE::ATTACK_ACTIVE, 0);
}

void Player::EnabledParry() {
	m_enabledParry = true;
}

const short Player::GetState(CHARA_STATE request) {
	if (request == CHARA_STATE::TRUNK) { return m_trunk; }
	return Character::GetState(request);
}

void Player::SetState(CHARA_STATE request, short state) {
	if (request == CHARA_STATE::TRUNK) { 
		m_trunk = state; 
		return;
	}
	Character::SetState(request, state);
}

//各攻撃のダメージ計算と、相手の行動をHitに変更する
void CalculateDamage(Enemy &enemy, short damage, short playerPow) {
	short enemyHp = enemy.GetState(CHARA_STATE::HP);
	int d = (damage + (playerPow - enemy.GetState(CHARA_STATE::DEF)));
	if (d <= 0) { d = 1; }
	enemyHp -= d;
	if (enemyHp <= 0) {
		enemy.SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::DOWN));
		enemy.SetState(CHARA_STATE::HP, 0);
		return;
	}

	enemy.SetState(CHARA_STATE::HP, enemyHp);
	enemy.SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::HIT));
}