#include "stdafx.h"
#include "parameter.hpp"
#include "enemy.hpp"
#include "player.hpp"

void CalculateDamage(Player &player, short damage, short enemyPow);


Enemy::Enemy(short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd) {
	Character::InitState(g_left, hp, power, defense, pos, action, time, yAdd);
}

void Enemy::Update(unsigned int nowTime, vector<vector<vector<int>>> maxFrame, vector<vector<vector<int>>> myHurtRect, vector<vector<vector<int>>> oppHurtRect, vector<vector<vector<int>>> myAtkRect, vector<unsigned int> myHurtActive, vector<unsigned int> myAtkActive, vector<vector<int>> mapData, Player &player) {
	vector<int> mFrame = maxFrame[0][GetState(CHARA_STATE::ACTION)];
	if (UpdateAnimation(nowTime, mFrame) == true) { ChangeAction(player, mFrame[0], myHurtActive, myAtkActive); }
	
	const int myFrameNum = ReturnFrameNum(maxFrame[0][static_cast<int>(GetState(CHARA_STATE::ACTION))][0], *this);
	const int oppFrameNum = ReturnFrameNum(maxFrame[1][static_cast<int>(player.GetState(CHARA_STATE::ACTION))][0], player);

	HandleAttack(player, myFrameNum, oppFrameNum, myAtkRect[static_cast<int>(GetState(CHARA_STATE::ACTION))][myFrameNum], oppHurtRect[static_cast<int>(player.GetState(CHARA_STATE::ACTION))][oppFrameNum]);
	MovePos(myHurtRect[static_cast<int>(GetState(CHARA_STATE::ACTION))][myFrameNum], mapData);
	CollisionChara(myHurtRect[static_cast<int>(GetState(CHARA_STATE::ACTION))][myFrameNum], oppHurtRect[static_cast<int>(player.GetState(CHARA_STATE::ACTION))][oppFrameNum], mapData, player);
}

void Enemy::HandleAttack(Player &player, int myFrame, int oppFrame, vector<int> myAtkRect, vector<int> oppHurtRect) {
	const unsigned int atkActive = GetActiveBit(CHARA_STATE::ATTACK_ACTIVE, myFrame);
	const unsigned int hurtActive = GetActiveBit(CHARA_STATE::HURT_ACTIVE, oppFrame);
	if ((atkActive != 0) && (hurtActive != 0)){// && (player.GetState(CHARA_STATE::ACTION) != static_cast<short>(PLAYER::ACTION::GUARD))) {

		if (GetState(CHARA_STATE::DIR) == g_left) { myAtkRect = FlipRect(myAtkRect, GetPos().w); }
		if (player.GetState(CHARA_STATE::DIR) == g_left) { oppHurtRect = FlipRect(oppHurtRect, player.GetPos().w); }
		SDL_Rect myPos = ReturnCharaRect(GetPos(), myAtkRect);
		SDL_Rect oppPos = ReturnCharaRect(player.GetPos(), oppHurtRect);

		if (DetectCollisionRect(myPos, oppPos) == true) {//自分の攻撃が相手に当たった
			ENEMY::ACTION action = static_cast<ENEMY::ACTION>(Character::GetState(CHARA_STATE::ACTION));
			switch (action) {
			case ENEMY::ACTION::PUNCH: {
				Punch(player);
				break;
			}
			case ENEMY::ACTION::DIVE: {
				Dive(player);
				break;
			}
			case ENEMY::ACTION::JUMP_OUT: {
				JumpOut(player);
				break;
			}
			case ENEMY::ACTION::RAMMING: {
				Ramming(player);
				break;
			}
			case ENEMY::ACTION::KICK_FRONT: {
				KickFront(player);
				break;
			}
			case ENEMY::ACTION::KICK_BACK: {
				KickBack(player);
				break;
			}
			case ENEMY::ACTION::HARD_PUNCH: {
				HardPunch(player);
				break;
			}
			case ENEMY::ACTION::COMBO_PUNCH: {
				ComboPunch(player);
				break;
			}
			}
		}
	}
}

void Enemy::Punch(Player &player) {

}

void Enemy::Dive(Player &player){

}

void Enemy::JumpOut(Player &player){

}

void Enemy::Ramming(Player &player){

}
void Enemy::KickFront(Player &player){

}

void Enemy::KickBack(Player &player){

}

void Enemy::HardPunch(Player &player){
	const short damage = 20;
	CalculateDamage(player, damage, GetState(CHARA_STATE::POW));
	SetActiveBit(CHARA_STATE::ATTACK_ACTIVE, 0);
}

void Enemy::ComboPunch(Player &player){

}

//各攻撃のダメージ計算と、相手の行動をHitに変更する
void CalculateDamage(Player &player, short damage, short enemyPow) {
	if (player.GetState(CHARA_STATE::ACTION) == static_cast<short>(PLAYER::ACTION::GUARD)) {//ガードされた
		short playerTrunk = player.GetState(CHARA_STATE::TRUNK);

		playerTrunk -= (damage + (enemyPow - player.GetState(CHARA_STATE::DEF)));
		if (playerTrunk < 0) {
			player.SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER::ACTION::FAINT));
			playerTrunk = 0; 
		}
		player.SetState(CHARA_STATE::HP, playerTrunk);
	}
	else {
		short playerHp = player.GetState(CHARA_STATE::HP);
		playerHp -= (damage + (enemyPow - player.GetState(CHARA_STATE::DEF)));
		if (playerHp < 0) { playerHp = 0; }
		player.SetState(CHARA_STATE::HP, playerHp);

		if (player.GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground || player.GetState(CHARA_STATE::Y_ADD) == 0) {
			player.SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER::ACTION::HIT));
		}
		else {
			player.SetState(CHARA_STATE::ACTION, static_cast<short>(PLAYER::ACTION::HIT_AIR));
		}
	}
}

/*
const short Enemy::GetState(CHARA_STATE request) {
return Character::GetState(request);
}

void Enemy::SetState(CHARA_STATE request, int state) {
Character::SetState(request, state);
}
*/