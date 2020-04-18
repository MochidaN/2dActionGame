#include "stdafx.h"
#include "enemy.hpp"
#include "player.hpp"

Enemy::Enemy(short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd) {
	Character::InitState(g_left, hp, power, defense, pos, action, time, yAdd);
}

void Enemy::Update(unsigned int nowTime, vector<vector<int>> maxFrame, vector<vector<vector<int>>> myHurtRect, vector<vector<vector<int>>> oppHurtRect, vector<vector<vector<int>>> myAtkRect, vector<vector<int>> mapData, Player &player) {
	vector<int> mFrame = maxFrame[GetState(CHARA_STATE::ACTION)];
	if (UpdateAnimation(nowTime, mFrame) == true) { ChangeAction(player, mFrame[0]); }
	
	const int myFrameNum = ReturnFrameNum(maxFrame[static_cast<int>(GetState(CHARA_STATE::ACTION))][0], *this);
	const int oppFrameNum = ReturnFrameNum(maxFrame[static_cast<int>(player.GetState(CHARA_STATE::ACTION))][0], player);

	HandleAttack(player, myFrameNum, oppFrameNum, myAtkRect[static_cast<int>(GetState(CHARA_STATE::ACTION))][myFrameNum], oppHurtRect[static_cast<int>(player.GetState(CHARA_STATE::ACTION))][oppFrameNum]);
	MovePos(myHurtRect[static_cast<int>(GetState(CHARA_STATE::ACTION))][myFrameNum], mapData);
	CollisionChara(myHurtRect[static_cast<int>(GetState(CHARA_STATE::ACTION))][myFrameNum], oppHurtRect[static_cast<int>(player.GetState(CHARA_STATE::ACTION))][oppFrameNum], mapData, player);
}

void Enemy::HandleAttack(Player &player, int myFrame, int oppFrame, vector<int> myAtkRect, vector<int> oppHurtRect) {
	const unsigned int atkActive = GetActiveBit(CHARA_STATE::ATTACK_ACTIVE, myFrame);
	const unsigned int hurtActive = GetActiveBit(CHARA_STATE::HURT_ACTIVE, oppFrame);
	if ((atkActive != 0) && (hurtActive != 0) && (player.GetState(CHARA_STATE::ACTION) != static_cast<short>(PLAYER::ACTION::GUARD))) {
		SDL_Rect myPos = ReturnCharaRect(GetPos(), myAtkRect);
		SDL_Rect oppPos = ReturnCharaRect(player.GetPos(), oppHurtRect);
		if (DetectCollisionRect(myPos, oppPos) == true) {//é©ï™ÇÃçUåÇÇ™ëäéËÇ…ìñÇΩÇ¡ÇΩ
			ENEMY::ACTION action = static_cast<ENEMY::ACTION>(Character::GetState(CHARA_STATE::ACTION));
			switch (action) {
			case ENEMY::ACTION::PUNCH: {
				Punch();
				break;
			}
			case ENEMY::ACTION::DIVE: {
				Dive();
				break;
			}
			case ENEMY::ACTION::JUMP_OUT: {
				JumpOut();
				break;
			}
			case ENEMY::ACTION::RAMMING: {
				Ramming();
				break;
			}
			case ENEMY::ACTION::KICK_FRONT: {
				KickFront();
				break;
			}
			case ENEMY::ACTION::KICK_BACK: {
				KickBack();
				break;
			}
			case ENEMY::ACTION::HARD_PUNCH: {
				HardPunch();
				break;
			}
			case ENEMY::ACTION::COMBO_PUNCH: {
				ComboPunch();
				break;
			}
			}
		}
	}
}

void Enemy::Punch() {

}

void Enemy::Dive(){

}

void Enemy::JumpOut(){

}

void Enemy::Ramming(){

}
void Enemy::KickFront(){

}

void Enemy::KickBack(){

}

void Enemy::HardPunch(){

}

void Enemy::ComboPunch(){

}

/*
const short Enemy::GetState(CHARA_STATE request) {
return Character::GetState(request);
}

void Enemy::SetState(CHARA_STATE request, int state) {
Character::SetState(request, state);
}
*/