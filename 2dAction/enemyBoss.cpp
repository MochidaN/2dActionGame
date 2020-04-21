#include "stdafx.h"
#include "enemy.hpp"
#include "player.hpp"
#include <random>

void CloseAction(int randNum, EnemyBoss &chara, vector<unsigned int> hurtActive, vector<unsigned int> atkActive);

void EnemyBoss::ChangeAction(Player player, int maxFrameHeng, vector<int> hurtRect, vector<unsigned int> hurtActive, vector<unsigned int> atkActive) {
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
		int xAdd = g_enemyMoveX;
		SetState(CHARA_STATE::X_ADD, xAdd);
		return;
	}
	}

	mt19937 mt{ std::random_device{}() };
	uniform_int_distribution<int> dist(1, 100);
	const int r = dist(mt);
	float distance = CalculateSquaredDistance(GetPos(), player.GetPos());
	const float attackRange = 300;
	if (distance < attackRange * attackRange) {//プレイヤが近くにいるとき
		if (player.GetActiveBit(CHARA_STATE::ATTACK_ACTIVE, ReturnFrameNum(maxFrameHeng, player)) == 0) {//攻撃ではない
			SDL_Rect pos = GetPos();
			vector<int> fRect = FlipRect(hurtRect, pos.w);
			if (GetState(CHARA_STATE::DIR) == g_right && player.GetPos().x < GetPos().x) {//右向きから左向きになるとき
				pos.x -= (fRect[0] - hurtRect[0]);
				SetState(CHARA_STATE::DIR, g_left);
			}
			else if(GetState(CHARA_STATE::DIR) == g_left && player.GetPos().x > GetPos().x) {
				pos.x -= (hurtRect[0] - fRect[0]);
				SetState(CHARA_STATE::DIR, g_right);
			}
			SetPos(pos);
			

			CloseAction(r, *this, hurtActive, atkActive);
		}
		else {//プレイヤが攻撃してきたとき
			if (1 <= r && r <= 60) {
				const short nextAction = static_cast<short>(ENEMY::ACTION::GUARD);
				SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
			}
			else {
				CloseAction(r, *this, hurtActive, atkActive);
			}
		}
		SetState(CHARA_STATE::X_ADD, 0);
	}
	else {
		short xAdd = 0;
		if (1 <= r && r <= 25) {
			const short nextAction = static_cast<short>(ENEMY::ACTION::DIVE);
			SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		}
		else if(26 <= r && r <= 50) {
			const short nextAction = static_cast<short>(ENEMY::ACTION::RAMMING);
			SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
			xAdd = g_enemyMoveX * 2;
		}
		else if (51 <= r && r <= 80) {
			const short nextAction = static_cast<short>(ENEMY::ACTION::WALK);
			SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
			xAdd = g_enemyMoveX;
		}
		else {
			const short nextAction = static_cast<short>(ENEMY::ACTION::STAND);
			SetAction(*this, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		}
		SetState(CHARA_STATE::X_ADD, xAdd);
	}
}

void CloseAction(int randNum, EnemyBoss &chara, vector<unsigned int> hurtActive, vector<unsigned int> atkActive) {
	short xAdd = 0;
	if (1 <= randNum && randNum <= 20) {
		const short nextAction = static_cast<short>(ENEMY::ACTION::HARD_PUNCH);
		SetAction(chara, nextAction, hurtActive[nextAction], atkActive[nextAction]);
	}
	else if (21 <= randNum && randNum <= 40) {
		const short nextAction = static_cast<short>(ENEMY::ACTION::COMBO_PUNCH);
		SetAction(chara, nextAction, hurtActive[nextAction], atkActive[nextAction]);
	}
	else if (41 <= randNum && randNum <= 55) {
		const short nextAction = static_cast<short>(ENEMY::ACTION::KICK_FRONT);
		SetAction(chara, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		xAdd = g_enemyMoveX / 2;
	}
	else if (56 <= randNum && randNum <= 70) {
		const short nextAction = static_cast<short>(ENEMY::ACTION::KICK_BACK);
		SetAction(chara, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		xAdd = (g_enemyMoveX / 2) * -1;//後ろへ移動
	}
	else if (71 <= randNum && randNum <= 80) {
		const short nextAction = static_cast<short>(ENEMY::ACTION::PUNCH);
		SetAction(chara, nextAction, hurtActive[nextAction], atkActive[nextAction]);
	}
	else if (81 <= randNum && randNum <= 90) {
		const short nextAction = static_cast<short>(ENEMY::ACTION::WALK);
		SetAction(chara, nextAction, hurtActive[nextAction], atkActive[nextAction]);
		xAdd = g_enemyMoveX;
	}
	else if (91 <= randNum && randNum <= 100) {
		const short nextAction = static_cast<short>(ENEMY::ACTION::STAND);
		SetAction(chara, nextAction, hurtActive[nextAction], atkActive[nextAction]);
	}
	chara.SetState(CHARA_STATE::X_ADD, xAdd);

}