#pragma once
#include "character.hpp"
#include <vector>

using namespace std;
class Player;
const short g_enemyMoveX = 2;

class Enemy : public Character {
private:
	//各アクションで実行する処理
	void Punch(Player &player);
	void JumpOut(Player &player);
	void Ramming(Player &player);
	void KickFront(Player &player);
	void KickBack(Player &player);
	void HardPunch(Player &player);
	void ComboPunch(Player &player, int myFrame);

	//次の行動を選択
	virtual void ChangeAction(Player player, int maxFrameHeng, vector<int> hurtRect, vector<unsigned int> hurtActive, vector<unsigned int> atkActive) = 0;

	//攻撃イベントの処理
	void HandleAttack(Player &player, int myFrame, int oppFrame, vector<int> myAtkRect, vector<int> oppHurtRect);
public:
	Enemy(short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd);

	void Update(unsigned int nowTime, vector<vector<vector<int>>> maxFrame, vector<vector<vector<int>>> myHurtRect, vector<vector<vector<int>>> oppHurtRect, vector<vector<vector<int>>> myAtkRect, vector<unsigned int> myHurtActive, vector<unsigned int> myAtkActive, vector<vector<int>> mapData, Player &player);
};


class EnemyGuard : public Enemy {
public:
	using Enemy::Enemy;
	void ChangeAction(Player player, int maxFrameHeng, vector<int> hurtRect, vector<unsigned int> hurtActive, vector<unsigned int> atkActive) override;
};

class EnemyWarp : public Enemy {
public:
	using Enemy::Enemy;
	void ChangeAction(Player player, int maxFrameHeng, vector<int> hurtRect, vector<unsigned int> hurtActive, vector<unsigned int> atkActive) override;
};

class EnemyKick : public Enemy {
public:
	using Enemy::Enemy;
	void ChangeAction(Player player, int maxFrameHeng, vector<int> hurtRect, vector<unsigned int> hurtActive, vector<unsigned int> atkActive) override;
};

class EnemyBoss : public Enemy {
public:
	using Enemy::Enemy;
	void ChangeAction(Player player, int maxFrameHeng, vector<int> hurtRect, vector<unsigned int> hurtActive, vector<unsigned int> atkActive) override;
};