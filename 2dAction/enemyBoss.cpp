#include "stdafx.h"
#include "enemy.hpp"
#include "player.hpp"
#include <random>

void CloseAction(int randNum, EnemyBoss &chara);

void EnemyBoss::ChangeAction(Player player, int maxFrameHeng) {
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> dist(1, 100);
	const int r = dist(mt);

	float distance = CalculateSquaredDistance(GetPos(), player.GetPos());
	const float attackRange = 200;
	if (distance < attackRange * attackRange) {//ƒvƒŒƒCƒ„‚ª‹ß‚­‚É‚¢‚é‚Æ‚«
		if (player.GetActiveBit(CHARA_STATE::ATTACK_ACTIVE, ReturnFrameNum(maxFrameHeng, player)) == 0) {//UŒ‚‚Å‚Í‚È‚¢
			CloseAction(r, *this);
		}
		else {//ƒvƒŒƒCƒ„‚ªUŒ‚‚µ‚Ä‚«‚½‚Æ‚«
			if (1 <= r && r <= 60) {
				SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::GUARD));
			}
			else {
				CloseAction(r, *this);
			}
		}
		SetState(CHARA_STATE::X_ADD, 0);
	}
	else {
		short xAdd = 0;
		if (1 <= r && r <= 25) {
			SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::DIVE));
		}
		else if(26 <= r && r <= 50) {
			SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::RAMMING));
		}
		else if (51 <= r && r <= 80) {
			SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::WALK));
			xAdd = g_enemyMoveX;
		}
		else {
			SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::STAND));
		}
		SetState(CHARA_STATE::X_ADD, xAdd);
	}
}

void CloseAction(int randNum, EnemyBoss &chara) {
	short xAdd = 0;
	if (1 <= randNum && randNum <= 20) {
		chara.SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::HARD_PUNCH));
	}
	else if (21 <= randNum && randNum <= 40) {
		chara.SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::COMBO_PUNCH));
	}
	else if (41 <= randNum && randNum <= 55) {
		chara.SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::KICK_FRONT));
		xAdd = g_enemyMoveX;
	}
	else if (56 <= randNum && randNum <= 70) {
		chara.SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::KICK_BACK));
		//xAdd = g_enemyMoveX * (-1 * chara.GetState(CHARA_STATE::DIR));
	}
	else if (71 <= randNum && randNum <= 80) {
		chara.SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::PUNCH));
	}
	else if (81 <= randNum && randNum <= 90) {
		chara.SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::WALK));
		xAdd = g_enemyMoveX;
	}
	else if (91 <= randNum && randNum <= 100) {
		chara.SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::STAND));
	}
	chara.SetState(CHARA_STATE::X_ADD, xAdd);
}