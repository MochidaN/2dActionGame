#pragma once
#include "character.hpp"
#include <vector>

using namespace std;
class Player;
const short g_enemyMoveX = 4;

class Enemy : public Character {
private:
	//�e�A�N�V�����Ŏ��s���鏈��
	void Punch();
	void Dive();
	void JumpOut();
	void Ramming();
	void KickFront();
	void KickBack();
	void HardPunch();
	void ComboPunch();

	//���̍s����I��
	virtual void ChangeAction(Player player, int maxFrameHeng) = 0;

	//�U���C�x���g�̏���
	void HandleAttack(Player &player, int myFrame, int oppFrame, vector<int> myAtkRect, vector<int> oppHurtRect);
public:
	Enemy(short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd);

	void Update(unsigned int nowTime, vector<vector<int>> maxFrame, vector<vector<vector<int>>> myHurtRect, vector<vector<vector<int>>> oppHurtRect, vector<vector<vector<int>>> myAtkRect, vector<vector<int>> mapData, Player &player);

	//const short GetState(CHARA_STATE request) override;
	//void SetState(CHARA_STATE request, int state) override;
};


class EnemyGuard : public Enemy {
public:
	using Enemy::Enemy;
	void ChangeAction(Player player, int maxFrameHeng) override;
};

class EnemyWarp : public Enemy {
public:
	using Enemy::Enemy;
	void ChangeAction(Player player, int maxFrameHeng) override;
};

class EnemyKick : public Enemy {
public:
	using Enemy::Enemy;
	void ChangeAction(Player player, int maxFrameHeng) override;
};

class EnemyBoss : public Enemy {
public:
	using Enemy::Enemy;
	void ChangeAction(Player player, int maxFrameHeng) override;
};