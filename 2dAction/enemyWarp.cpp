#include "stdafx.h"
#include "enemy.hpp"
#include "player.hpp"
#include <random>

void EnemyWarp::ChangeAction(Player player, int maxFrameHeng) {
	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> dist(1, 10);
	const int r = dist(mt);
	short xAdd = 0;
	if (1 <= r && r <= 4) {
		SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::DIVE));
	}
	else {
		SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::STAND));
	}
	SetState(CHARA_STATE::X_ADD, xAdd);
}