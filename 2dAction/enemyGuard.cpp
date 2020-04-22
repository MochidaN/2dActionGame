#include "stdafx.h"
#include "enemy.hpp"
#include "player.hpp"
#include <random>

void EnemyGuard::ChangeAction(Player player, int maxFrameHeng, vector<int> hurtRect, vector<unsigned int> hurtActive, vector<unsigned int> atkActive) {
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
	short xAdd = 0;
	if (1 <= r && r <= 6) {
		const short nextAction = static_cast<short>(ENEMY::ACTION::GUARD);
		SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
	}
	else {
		float distance = CalculateSquaredDistance(GetPos(), player.GetPos());
		const float attackRange = 100;
		if (distance < attackRange * attackRange) {
			const short nextAction = static_cast<short>(ENEMY::ACTION::PUNCH);
			SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		}
		else {
			SDL_Rect pos = GetPos();
			vector<int> fRect = FlipRect(hurtRect, pos.w);
			if (GetState(CHARA_STATE::DIR) == g_right && player.GetPos().x < GetPos().x) {//‰EŒü‚«‚©‚ç¶Œü‚«‚É‚È‚é‚Æ‚«
				pos.x -= (fRect[0] - hurtRect[0]);
				SetState(CHARA_STATE::DIR, g_left);
			}
			else if (GetState(CHARA_STATE::DIR) == g_left && player.GetPos().x > GetPos().x) {
				pos.x -= (hurtRect[0] - fRect[0]);
				SetState(CHARA_STATE::DIR, g_right);
			}
			SetPos(pos);

			const short nextAction = static_cast<short>(ENEMY::ACTION::WALK);
			SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
			xAdd = g_enemyMoveX;
		}
	}
	SetState(CHARA_STATE::X_ADD, xAdd);
}