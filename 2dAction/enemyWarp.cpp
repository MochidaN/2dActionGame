#include "stdafx.h"
#include "enemy.hpp"
#include "player.hpp"
#include <random>

void EnemyWarp::ChangeAction(Player player, int maxFrameHeng, vector<int> hurtRect, vector<unsigned int> hurtActive, vector<unsigned int> atkActive) {
	switch (static_cast<ENEMY::ACTION>(GetState(CHARA_STATE::ACTION))) {
	case ENEMY::ACTION::DOWN: {
		const short nextAction = static_cast<short>(ENEMY::ACTION::DEAD);
		SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		return;
	}
	case ENEMY::ACTION::DEAD: {
		return;
	}
	case ENEMY::ACTION::DIVE: {
		const short nextAction = static_cast<short>(ENEMY::ACTION::JUMP_OUT);
		SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		if (player.GetPos().x <= GetPos().x) { SetState(CHARA_STATE::X_ADD, g_enemyMoveX * 2); }
		else { SetState(CHARA_STATE::X_ADD, -g_enemyMoveX * 2); }

		return;
	}
	}

	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> dist(1, 10);
	const int r = dist(mt);
	short xAdd = 0;
	if (1 <= r && r <= 5) {
		const short nextAction = static_cast<short>(ENEMY::ACTION::DIVE);
		SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
	}
	else {
		const short nextAction = static_cast<short>(ENEMY::ACTION::STAND);
		SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
	}
	SetState(CHARA_STATE::X_ADD, xAdd);
}