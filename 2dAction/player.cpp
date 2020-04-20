#include "stdafx.h"
#include "parameter.hpp"
#include "player.hpp"
#include "enemy.hpp"

const short X = 0, Y = 1, W = 2, H = 3;
const short g_playerStepX = 20;
const short g_yAdd = -28;

void CalculateDamage(Enemy &enemy, short damage, short playerPow);


Player::Player(short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd) {
	Character::InitState(g_right, hp, power, defense, pos, action, time, yAdd);
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
	PLAYER::ACTION action = static_cast<PLAYER::ACTION>(Character::GetState(CHARA_STATE::ACTION));
	switch (action) {
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
	case PLAYER::ACTION::FAINT: {
		const short nextAction = static_cast<short>(PLAYER::ACTION::FAINTING);
		SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		break;
	}
	case PLAYER::ACTION::DOWN: {
		const short nextAction = static_cast<short>(PLAYER::ACTION::DEAD);
		SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		break;
	}
	case PLAYER::ACTION::DEAD: {
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
	switch (event) {
	case EVENT::JOY_CIRCLE: {
		if (GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground) {
			const short nextAction = static_cast<short>(PLAYER::ACTION::STEP);
			SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
			SetState(CHARA_STATE::X_ADD, g_playerStepX);
		}
		break;
	}
	case EVENT::JOY_CROSS: {
		if (GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground) {
			const short nextAction = static_cast<short>(PLAYER::ACTION::JUMP);
			SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
			SetState(CHARA_STATE::Y_ADD, g_yAdd);
		}
		break;
	}
	case EVENT::JOY_L1_DOWN: {
		if (GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground) {
			const short nextAction = static_cast<short>(PLAYER::ACTION::GUARD);
			SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
			SetState(CHARA_STATE::X_ADD, 0);
		}
		break;
	}
	case EVENT::JOY_L1_UP: {
		if (GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground) {
			const short nextAction = static_cast<short>(PLAYER::ACTION::STAND);
			SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		}
		break;
	}
	case EVENT::JOY_R1: {
		if (GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground) {
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
		const short nextAction = static_cast<short>(PLAYER::ACTION::WALK);
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
		const short nextAction = static_cast<short>(PLAYER::ACTION::WALK);
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

//各攻撃のダメージ計算と、相手の行動をHitに変更する
void CalculateDamage(Enemy &enemy, short damage, short playerPow) {
	short enemyHp = enemy.GetState(CHARA_STATE::HP);

	enemyHp -= (damage + (playerPow - enemy.GetState(CHARA_STATE::DEF)));
	if (enemyHp < 0) { enemyHp = 0; }
	enemy.SetState(CHARA_STATE::HP, enemyHp);
	enemy.SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::HIT));
}

/*
const short Player::GetState(CHARA_STATE request) {
return Character::GetState(request);
}

void Player::SetState(CHARA_STATE request, int state) {
Character::SetState(request, state);
}*/