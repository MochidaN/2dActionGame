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

	vector<vector<ENEMY::ACTION>> m_enemyAction;//�e�G�����A�N�V����
	vector<vector<vector<int>>> m_maxFrame;//�e�A�j���[�V�����̃t���[����
	vector<vector<unsigned int>> m_attackActive;//�U������̂���t���[��
	vector<vector<vector<vector<int>>>> m_attackRect;//�U������͈�
	vector<vector<unsigned int>> m_hurtActive;//���炢����̂���t���[��
	vector<vector<vector<vector<int>>>> m_hurtRect;//���炢����͈�

	Player *m_player;
	Enemy ***m_enemy;
public:
	Game(SDL_Renderer *renderer, vector<vector<ENEMY::ACTION>> enemyAction, vector<vector<vector<int>>> maxFrame, vector<vector<unsigned int>> attackActive, vector<vector<vector<vector<int>>>> attackRect, vector<vector<unsigned int>> hurtActive, vector<vector<vector<vector<int>>>> hurtRect);
	~Game();

	bool Update(SDL_Renderer *renderer);
	void Draw(SDL_Renderer *renderer, int windowPosX);
};
