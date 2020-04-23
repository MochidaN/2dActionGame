#pragma once
#include "sequence.hpp"
#include "getInput.hpp"
#include <SDL.h>
#include <vector>

using namespace std;

class Player;
class Enemy;

class Game : public Sequence {
private:
	GetInput m_eventInput;
	SDL_Texture *m_world;
	vector<vector<int>> m_mapData;
	SDL_Texture ***m_characterTexture;

	vector<vector<ENEMY::ACTION>> m_enemyAction;//各敵が持つアクション
	vector<vector<vector<int>>> m_maxFrame;//各アニメーションのフレーム数
	vector<vector<unsigned int>> m_attackActive;//攻撃判定のあるフレーム
	vector<vector<vector<vector<int>>>> m_attackRect;//攻撃判定範囲
	vector<vector<unsigned int>> m_hurtActive;//くらい判定のあるフレーム
	vector<vector<vector<vector<int>>>> m_hurtRect;//くらい判定範囲

	Player *m_player;
	Enemy ***m_enemy;
public:
	Game(SDL_Renderer *renderer, vector<vector<ENEMY::ACTION>> enemyAction, vector<vector<vector<int>>> maxFrame, vector<vector<unsigned int>> attackActive, vector<vector<vector<vector<int>>>> attackRect, vector<vector<unsigned int>> hurtActive, vector<vector<vector<vector<int>>>> hurtRect);
	~Game();

	bool Update(SDL_Renderer *renderer);
	void Draw(SDL_Renderer *renderer, int windowPosX);
};
