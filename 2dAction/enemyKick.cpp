#include "stdafx.h"
#include "enemy.hpp"
#include "player.hpp"
#include <random>

void EnemyKick::ChangeAction(Player player, int maxFrameHeng, vector<int> hurtRect, vector<unsigned int> hurtActive, vector<unsigned int> atkActive) {
	switch (static_cast<ENEMY::ACTION>(GetState(CHARA_STATE::ACTION))) {
	case ENEMY::ACTION::DOWN: {
		const short nextAction = static_cast<short>(ENEMY::ACTION::DEAD);
		SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		return;
	}
	case ENEMY::ACTION::DEAD: { return; }
	}

	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> dist(1, 10);
	const int r = dist(mt);

	float distance = CalculateSquaredDistance(GetPos(), player.GetPos());
	const float attackRange = 200;
	short xAdd = 0;
	if (distance < attackRange * attackRange) {//ƒvƒŒƒCƒ„‚ª‹ß‚­‚É‚¢‚é‚Æ‚«
		if (1 <= r && r <= 5) {
			const short nextAction = static_cast<short>(ENEMY::ACTION::KICK_FRONT);
			SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
			xAdd = g_enemyMoveX / 2;
		}
		else {
			const short nextAction = static_cast<short>(ENEMY::ACTION::KICK_BACK);
			SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
			xAdd = (g_enemyMoveX / 2) * -1;//Œã‚ë‚ÖˆÚ“®
		}
	}
	else {
		if (1 <= r && r <= 6) {
			const short nextAction = static_cast<short>(ENEMY::ACTION::STAND);
			SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		}
		else {
			const short nextAction = static_cast<short>(ENEMY::ACTION::RAMMING);
			SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
			xAdd = g_enemyMoveX * 2;
		}
	}
	SetState(CHARA_STATE::X_ADD, xAdd);
}