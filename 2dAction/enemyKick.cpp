#include "stdafx.h"
#include "enemy.hpp"
#include "player.hpp"
#include <random>

void EnemyKick::ChangeAction(Player player, int maxFrameHeng) {
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> dist(1, 10);
	const int r = dist(mt);

	float distance = CalculateSquaredDistance(GetPos(), player.GetPos());
	const float attackRange = 200;
	short xAdd = 0;
	if (distance < attackRange * attackRange) {//ƒvƒŒƒCƒ„‚ª‹ß‚­‚É‚¢‚é‚Æ‚«
		if (1 <= r && r <= 5) {
			SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::KICK_FRONT));
		}
		else {
			SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::KICK_BACK));
		}
	}
	else {
		if (1 <= r && r <= 6) {
			SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::STAND));
		}
		else {
			SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::RAMMING));
			//xAdd = ;
		}
	}
	SetState(CHARA_STATE::X_ADD, xAdd);
}