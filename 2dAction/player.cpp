#include "stdafx.h"
#include "parameter.hpp"
#include "player.hpp"
#include "enemy.hpp"

const short X = 0, Y = 1, W = 2, H = 3;
const short g_playerStepX = 20;
const short g_yAdd = -28;

Player::Player(short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd) {
	Character::InitState(g_right, hp, power, defense, pos, action, time, yAdd);
}

void Player::Update(unsigned int nowTime, vector<vector<int>> maxFrame, vector<vector<vector<int>>> myHurtRect, vector<vector<vector<int>>> oppHurtRect, vector<vector<int>> mapData, EVENT event) {
	bool endAnimation = UpdateAnimation(nowTime, maxFrame[Character::GetState(CHARA_STATE::ACTION)]);
	if (endAnimation == true) {
		ChangeAction();
	}

	const int maxHeng = maxFrame[static_cast<int>(GetState(CHARA_STATE::ACTION))][0];
	const int myFrameNum = ReturnFrameNum(maxHeng, *this);
	MovePos(myHurtRect[static_cast<int>(GetState(CHARA_STATE::ACTION))][myFrameNum], mapData);
}

void Player::ChangeAction() {
	PLAYER::ACTION action = static_cast<PLAYER::ACTION>(Character::GetState(CHARA_STATE::ACTION));
	switch (action) {
	case PLAYER::ACTION::HIT_AIR: {
		SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER::ACTION::FALL_HIT));
		break;
	}
	case PLAYER::ACTION::JUMP: {
		SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER::ACTION::FALL));
		break;
	}
	case PLAYER::ACTION::FAINT: {
		SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER::ACTION::FAINTING));
		break;
	}
	case PLAYER::ACTION::DOWN: {
		SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER::ACTION::DEAD));
		break;
	}
	case PLAYER::ACTION::DEAD: {
		break;
	}
	default: {
		SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER::ACTION::STAND));
		SetState(CHARA_STATE::X_ADD, 0);
		break;
	}
	}
}

void Player::HandleEvent(bool endAnimation, EVENT event, vector<int> hurtRect, vector<unsigned int> attackActive) {
	switch (event) {
	case EVENT::JOY_CIRCLE: {
		if (GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground) {
			SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER::ACTION::STEP));
			SetState(CHARA_STATE::X_ADD, g_playerStepX);
		}
		break;
	}
	case EVENT::JOY_CROSS: {
		if (GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground) {
			SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER::ACTION::JUMP));
			SetState(CHARA_STATE::Y_ADD, g_yAdd);
		}
		break;
	}
	case EVENT::JOY_L1_DOWN: {
		if (GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground) {
			SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER::ACTION::GUARD));
			SetState(CHARA_STATE::X_ADD, 0);
		}
		break;
	}
	case EVENT::JOY_L1_UP: {
		if (GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground) {
			SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER::ACTION::STAND));
		}
		break;
	}
	case EVENT::JOY_R1: {
		if (GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground) {
			if (GetState(CHARA_STATE::ACTION) == static_cast<short>(PLAYER::ACTION::VERTICAL_ATTACK)) {
				SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER::ACTION::SIDE_ATTACK));
				SetActiveBit(CHARA_STATE::ATTACK_ACTIVE, attackActive[static_cast<short>(PLAYER::ACTION::SIDE_ATTACK)]);
				SetState(CHARA_STATE::X_ADD, 0);
			}
			else if (GetState(CHARA_STATE::ACTION) != static_cast<short>(PLAYER::ACTION::SIDE_ATTACK)) {
				SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER::ACTION::VERTICAL_ATTACK));
				SetActiveBit(CHARA_STATE::ATTACK_ACTIVE, attackActive[static_cast<int>(PLAYER::ACTION::VERTICAL_ATTACK)]);
				SetState(CHARA_STATE::X_ADD, 0);
			}
		}
		break;
	}
	case EVENT::JOY_HAT_LEFT: {
		SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER::ACTION::WALK));
		SetState(CHARA_STATE::X_ADD, g_addX);

		if (GetState(CHARA_STATE::DIR) == g_right) {//âEå¸Ç´Ç©ÇÁç∂å¸Ç´Ç…Ç»ÇÈÇ∆Ç´
			SDL_Rect pos = GetPos();
			const short action = GetState(CHARA_STATE::ACTION);
			vector<int> fRect = FlipRect(hurtRect, pos.w);
			pos.x -= (fRect[X] - hurtRect[X]);
			SetPos(pos);
			SetState(CHARA_STATE::DIR, g_left);
		}
		break;
	}
	case EVENT::JOY_HAT_RIGHT: {
		SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER::ACTION::WALK));
		SetState(CHARA_STATE::X_ADD, g_addX);

		if (GetState(CHARA_STATE::DIR) == g_left) {//ç∂å¸Ç´Ç©ÇÁâEå¸Ç´Ç…Ç»ÇÈÇ∆Ç´
			SDL_Rect pos = GetPos();
			const short action = GetState(CHARA_STATE::ACTION);
			vector<int> fRect = FlipRect(hurtRect, pos.w);
			pos.x -= (hurtRect[X] - fRect[X]);
			SetPos(pos);
			SetState(CHARA_STATE::DIR, g_right);
		}
		break;
	}
	case EVENT::JOY_HAT_CENTERED: {
		SetState(CHARA_STATE::X_ADD, 0);
		SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER::ACTION::STAND));
		break;
	}

	}
}

void Player::HandleAttack(Enemy &enemy, int myFrame, int oppFrame, vector<int> myAtkRect, vector<int> oppHurtRect) {
	const unsigned int atkActive = GetActiveBit(CHARA_STATE::ATTACK_ACTIVE, myFrame);
	const unsigned int hurtActive = GetActiveBit(CHARA_STATE::HURT_ACTIVE, oppFrame);
	if ((atkActive != 0) && (hurtActive != 0) && (enemy.GetState(CHARA_STATE::ACTION) != static_cast<short>(ENEMY::ACTION::GUARD))) {
		SDL_Rect myPos = ReturnCharaRect(GetPos(), myAtkRect);
		SDL_Rect oppPos = ReturnCharaRect(enemy.GetPos(), oppHurtRect);
		if (DetectCollisionRect(myPos, oppPos) == true) {//é©ï™ÇÃçUåÇÇ™ëäéËÇ…ìñÇΩÇ¡ÇΩ
			PLAYER::ACTION action = static_cast<PLAYER::ACTION>(Character::GetState(CHARA_STATE::ACTION));
			switch (action) {
			case PLAYER::ACTION::PARRY: {
				Parry();
				break;
			}
			case PLAYER::ACTION::SIDE_ATTACK: {
				SideAttack();
				break;
			}
			case PLAYER::ACTION::VERTICAL_ATTACK: {
				VerticalAttack();
				break;
			}
			}
		}
	}
}

void Player::Parry() {

}

void Player::SideAttack() {

}

void Player::VerticalAttack() {

}

/*
const short Player::GetState(CHARA_STATE request) {
return Character::GetState(request);
}

void Player::SetState(CHARA_STATE request, int state) {
Character::SetState(request, state);
}*/