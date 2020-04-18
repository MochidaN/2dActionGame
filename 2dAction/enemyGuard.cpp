#include "stdafx.h"
#include "enemy.hpp"
#include "player.hpp"
#include <random>

void EnemyGuard::ChangeAction(Player player, int maxFrameHeng) {
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> dist(1, 10);
	const int r = dist(mt);

	if (1 <= r && r <= 6) {
		SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::GUARD));
		SetState(CHARA_STATE::X_ADD, 0);
	}
	else {
		float distance = CalculateSquaredDistance(GetPos(), player.GetPos());
		const float attackRange = 100;
		if (distance < attackRange * attackRange) {
			SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::PUNCH));
			SetState(CHARA_STATE::X_ADD, 0);
		}
		else {
			SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::WALK));
			SetState(CHARA_STATE::X_ADD, g_enemyMoveX);
		}
	}
}