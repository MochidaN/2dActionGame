#include "stdafx.h"
#include "parameter.hpp"
#include "gameClass.hpp"
#include "enemy.hpp"
#include "player.hpp"
#include "readFile.hpp"

const short IMG_SIZE[static_cast<short>(CHARA_ID::NUM)] = { ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE, PLAYER_SIZE };
const short CHARA_NUM[static_cast<short>(CHARA_ID::NUM)] = { 2, 2, 2, 1, 1 };
const short X = 0, Y = 1, W = 2, H = 3;

//worldのサーフェイスにマップを描画
void DrawWorld(SDL_Renderer *renderer, SDL_Surface *world, SDL_Surface *mapChip, vector<vector<int>> mapData);
//アクション名を配列番号に変換
template <typename T = ENEMY_ACTION> const int ActionToIndex(vector<T> actionList, int nowAction);
//体力バー等の描画
void DrawGauge(SDL_Renderer *renderer, SDL_Rect dst, int value, const char *color);
//各キャラクタのアニメーション更新
void UpdateAnimation(SDL_Renderer *renderer, SDL_Texture **charaTexture, int id, Character &my, vector<vector<ENEMY::ACTION>> m_enemyAction, int windowPosX);

Sequence::Sequence() {
	m_next = SEQ_ID::NONE;
}

SEQ_ID Sequence::MoveTo() {
	return m_next;
}

Game::Game(SDL_Renderer *renderer, vector<vector<ENEMY::ACTION>> enemyAction, vector<vector<vector<int>>> maxFrame, vector<vector<unsigned int>> attackActive, vector<vector<vector<vector<int>>>> attackRect, vector<vector<unsigned int>> hurtActive, vector<vector<vector<vector<int>>>> hurtRect) {

#ifdef NDEBUG
	const char *mapImgPath = "../../image/map/mapchip.png";
	const char *mapDataPath = "../../image/map/map.csv";
	vector<string> filePath{ "../../txt/guard/", "../../txt/warp/", "../../txt/kick/", "../../txt/boss/", "../../txt/player/" };
	string imgPathFile("imgPath.txt");
#else
	const char *mapImgPath = "../image/map/mapchip.png";
	const char *mapDataPath = "../image/map/map.csv";
	vector<string> filePath{ "../txt/guard/", "../txt/warp/", "../txt/kick/", "../txt/boss/", "../txt/player/" };
	string imgPathFile("imgPath_debug.txt");
#endif 

	SDL_Surface *worldSurface = SDL_CreateRGBSurface(0, WORLD_WIDTH * MAP_CHIPSIZE, WORLD_HEIGHT * MAP_CHIPSIZE, 32, 0, 0, 0, 0);
	SDL_Surface *mapChip = IMG_Load(mapImgPath);
	if (worldSurface == NULL || mapChip == NULL) {
		OutputError("failed to create world surface");
	}
	m_mapData = ReadFileSplit(mapDataPath, ',');
	DrawWorld(renderer, worldSurface, mapChip, m_mapData);
	if ((m_world = SDL_CreateTextureFromSurface(renderer, worldSurface)) == NULL) {
		OutputError("failed to create world texture");
	}
	SDL_FreeSurface(mapChip);

	const int charaNum = static_cast<int>(CHARA_ID::NUM);
	m_characterTexture = new SDL_Texture**[charaNum];
	for (int i = 0; i < charaNum; i++) {
		vector<string> imagePath = ReadFile(filePath[i] + imgPathFile);
		m_characterTexture[i] = new SDL_Texture*[imagePath.size()];
		for (int j = 0, end = imagePath.size(); j < end; j++) {
			SDL_Surface *img;
			if ((img = IMG_Load(imagePath[j].c_str())) == NULL) {
				OutputError("failed to open character image");
			}
			if ((m_characterTexture[i][j] = SDL_CreateTextureFromSurface(renderer, img)) == NULL) {
				OutputError("failed to create character texture");
			}
			SDL_FreeSurface(img);
		}
	}

	m_enemyAction = enemyAction;
	m_maxFrame = maxFrame;
	m_attackActive = attackActive;
	m_attackRect = attackRect;
	m_hurtActive = hurtActive;
	m_hurtRect = hurtRect;

	const short enemyNum = static_cast<short>(CHARA_ID::NUM) - 1;
	SDL_Rect pos[2][2] = {
		{{ MAP_CHIPSIZE * 20, MAP_CHIPSIZE*8, ENEMY_SIZE, ENEMY_SIZE }, { MAP_CHIPSIZE * 45, MAP_CHIPSIZE * 8, ENEMY_SIZE, ENEMY_SIZE }},
		{{ MAP_CHIPSIZE * (WORLD_WIDTH - 2), MAP_CHIPSIZE * 8, ENEMY_SIZE, ENEMY_SIZE }, {}} };

	unsigned int time = SDL_GetTicks();
	m_enemy = new Enemy**[enemyNum];
	for (int id = 0; id < enemyNum; id++) {
		m_enemy[id] = new Enemy*[CHARA_NUM[id]];
		for (int cn = 0; cn < CHARA_NUM[id]; cn++) {
			switch (id) {
			case static_cast<int>(CHARA_ID::GUARD) : {
				m_enemy[id][cn] = new EnemyGuard(60, 5, 20, pos[0][cn], 0, time, g_yAdd_ground);
				break;
			}
			case static_cast<int>(CHARA_ID::WARP) : {
				m_enemy[id][cn] = new EnemyWarp(60, 5, 5, pos[0][cn], 0, time, g_yAdd_ground);
				break;
			}
			case static_cast<int>(CHARA_ID::KICK) : {
				m_enemy[id][cn] = new EnemyKick(60, 10, 5, pos[0][cn], 0, time, g_yAdd_ground);
				break;
			}
			case static_cast<int>(CHARA_ID::BOSS) : {
				m_enemy[id][cn] = new EnemyBoss(100, 20, 15, pos[1][cn], 0, time, g_yAdd_ground);
				break;
			}
			}
		}
	}
	const short playerID = static_cast<short>(CHARA_ID::PLAYER);
	const short stand = static_cast<short>(PLAYER::ACTION::STAND);
	SDL_Rect playerPos = { 0, WINDOW_HEIGHT * MAP_CHIPSIZE - (MAP_CHIPSIZE + m_hurtRect[1][stand][0][Y] + m_hurtRect[1][stand][0][H]), IMG_SIZE[playerID], IMG_SIZE[playerID] };
	m_player = new Player(100, 15, 15, playerPos, static_cast<short>(PLAYER::ACTION::STAND), time, g_yAdd_ground);
}

Game::~Game() {
	SDL_DestroyTexture(m_world);
	const int charaNum = static_cast<int>(CHARA_ID::NUM);
	vector<short> actionNum;
	for (int id = 0, end = m_enemyAction.size(); id < end; id++) { actionNum.push_back(m_enemyAction[id].size()); }
	actionNum.push_back(static_cast<short>(ENEMY::ACTION::NUM));
	actionNum.push_back(static_cast<short>(PLAYER::ACTION::NUM));

	for (int id = 0; id < charaNum - 1; id++) {
		for (int cn = 0; cn < actionNum[id]; cn++) { SDL_DestroyTexture(m_characterTexture[id][cn]); }
		for (int cn = 0; cn < CHARA_NUM[id]; cn++) { delete m_enemy[id][cn]; }
		delete[] m_characterTexture[id];
		delete[] m_enemy[id];
	}
	delete m_player;
	delete m_characterTexture[static_cast<int>(CHARA_ID::PLAYER)];
	delete[] m_characterTexture;
}

bool Game::Update(SDL_Renderer *renderer) {
	if (m_enemy[3][0]->GetState(CHARA_STATE::ACTION) == static_cast<short>(ENEMY::ACTION::DEAD)) {
		m_next = SEQ_ID::GAME_CLEAR;
		return false;
	}
	else if (m_player->GetState(CHARA_STATE::ACTION) == static_cast<short>(PLAYER::ACTION::DEAD)) { 
		m_next = SEQ_ID::GAME_OVER; 
		return false; 
	}

	const int winWidth = WINDOW_WIDTH * MAP_CHIPSIZE;
	int windowPosX = (m_player->GetPos().x + m_hurtRect[1][static_cast<short>(PLAYER::ACTION::STAND)][0][X]) - (WINDOW_WIDTH * MAP_CHIPSIZE / 2);//描画領域の左端
	if (windowPosX < 0) { windowPosX = 0; }
	else if (windowPosX + winWidth > WORLD_WIDTH * MAP_CHIPSIZE) { windowPosX = (WORLD_WIDTH - WINDOW_WIDTH) * MAP_CHIPSIZE; }

	Draw(renderer, windowPosX);
	
#ifdef _DEBUG
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

	const short pid = static_cast<short>(CHARA_ID::PLAYER);
	const short act = m_player->GetState(CHARA_STATE::ACTION);
	const short pNowFrame = ReturnFrameNum(m_maxFrame[1][act][0], *m_player);

	vector<int> hurtrect = m_hurtRect[1][act][pNowFrame];
	vector<int> atkrect = m_attackRect[1][act][pNowFrame];
	if (m_player->GetState(CHARA_STATE::DIR) == g_left) {
		hurtrect = FlipRect(hurtrect, IMG_SIZE[pid]);
		atkrect = FlipRect(atkrect, IMG_SIZE[pid]);
	}

	SDL_Rect playerPos = ReturnCharaRect(m_player->GetPos(), hurtrect);
	SDL_Rect dst = playerPos;
	dst.x -= windowPosX;
	SDL_RenderDrawRect(renderer, &dst);

	SDL_Rect pAtkRect = { -1, -1, -1, -1 };
	if (m_player->GetActiveBit(CHARA_STATE::ATTACK_ACTIVE, pNowFrame) != 0) {
		pAtkRect = ReturnCharaRect(m_player->GetPos(), atkrect);
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		dst = pAtkRect;
		dst.x -= windowPosX;
		SDL_RenderDrawRect(renderer, &dst);
	}
	
	const short eneNum = static_cast<short>(CHARA_ID::NUM) - 1;
	for (int id = 0; id < eneNum; id++) {
		for (int cn = 0; cn < CHARA_NUM[id]; cn++) {
			const short a = m_enemy[id][cn]->GetState(CHARA_STATE::ACTION);
			const short eNowFrame = ReturnFrameNum(m_maxFrame[0][a][0], *m_enemy[id][cn]);

			vector<int> eHurtrect = m_hurtRect[0][a][eNowFrame];
			vector<int> eAtkrect = m_attackRect[0][a][eNowFrame];
			if (m_enemy[id][cn]->GetState(CHARA_STATE::DIR) == g_left) {
				eHurtrect = FlipRect(eHurtrect, IMG_SIZE[id]);
				eAtkrect = FlipRect(eAtkrect, IMG_SIZE[id]);
			}
			SDL_Rect enemyPos = ReturnCharaRect(m_enemy[id][cn]->GetPos(),eHurtrect);

			if ((windowPosX <= enemyPos.x + enemyPos.w) && (windowPosX + winWidth >= enemyPos.x)) {//ウィンドウ内にいる
				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
				dst = enemyPos;
				dst.x -= windowPosX;
				SDL_RenderDrawRect(renderer, &dst);

				if (m_enemy[id][cn]->GetActiveBit(CHARA_STATE::ATTACK_ACTIVE, eNowFrame) != 0) {
					SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
					dst = ReturnCharaRect(m_enemy[id][cn]->GetPos(), eAtkrect);
					dst.x -= windowPosX;
					SDL_RenderDrawRect(renderer, &dst);
				}
			}
		}
	}
#endif

	EVENT event = m_eventInput.GetEvent();
	switch (event) {
	case EVENT::QUIT: {
		m_next = SEQ_ID::QUIT;
		return false;
		break;
	}
	}
	
	unsigned int nowTime = SDL_GetTicks();
	const short charaNum = static_cast<short>(CHARA_ID::NUM) - 1;
	const int enemyID = 0, playerID = 1;
	m_player->Update(nowTime, m_maxFrame[playerID], m_hurtRect[playerID], m_hurtRect[enemyID], m_hurtActive[playerID], m_attackActive[playerID], m_mapData, event);
	
	const int pAction = m_player->GetState(CHARA_STATE::ACTION);
	const int pFrame = ReturnFrameNum(m_maxFrame[playerID][pAction][0], *m_player);
	const short enemyDead = static_cast<short>(ENEMY::ACTION::DEAD);

	for (int id = 0; id < charaNum; id++) {
		for (int cn = 0; cn < CHARA_NUM[id]; cn++) {
			SDL_Rect enemyPos = m_enemy[id][cn]->GetPos();
			if ((windowPosX <= enemyPos.x + enemyPos.w) && (windowPosX + winWidth >= enemyPos.x)) {//ウィンドウ内にいる
				if (m_enemy[id][cn]->GetState(CHARA_STATE::ACTION) != enemyDead) {
					const int eAction = m_enemy[id][cn]->GetState(CHARA_STATE::ACTION);
					const int eFrame = ReturnFrameNum(m_maxFrame[enemyID][eAction][0], *m_enemy[id][cn]);

					m_player->CollisionChara(m_hurtRect[playerID][pAction][pFrame], m_hurtRect[enemyID][eAction][eFrame], m_mapData, *m_enemy[id][cn]);
					m_player->HandleAttack(*m_enemy[id][cn], pFrame, eFrame, m_attackRect[playerID][pAction][pFrame], m_hurtRect[enemyID][eAction][eFrame]);
					m_enemy[id][cn]->Update(nowTime, m_maxFrame, m_hurtRect[enemyID], m_hurtRect[playerID], m_attackRect[enemyID], m_hurtActive[enemyID], m_attackActive[enemyID], m_mapData, *m_player);
				}
			}
		}
	}
	return true;
}

void Game::Draw(SDL_Renderer *renderer, int windowPosX) {
	SDL_Rect src = { windowPosX, 0, WINDOW_WIDTH * MAP_CHIPSIZE, WINDOW_HEIGHT * MAP_CHIPSIZE };
	SDL_RenderCopy(renderer, m_world, &src, NULL);

	const short charaNum = static_cast<short>(CHARA_ID::NUM);
	const short playerID = static_cast<short>(CHARA_ID::PLAYER);
	for (int id = 0; id < charaNum; id++) {
		for (int cn = 0; cn < CHARA_NUM[id]; cn++) {
			if (id == playerID) {
				SDL_Rect dst = {};
				DrawGauge(renderer, dst, m_player->GetState(CHARA_STATE::HP), "green");
				dst.y += 40;
				DrawGauge(renderer, dst, m_player->GetState(CHARA_STATE::TRUNK), "yellow");
				UpdateAnimation(renderer, m_characterTexture[id], id, *m_player, m_enemyAction, windowPosX);
			}
			else if (m_enemy[id][cn]->GetState(CHARA_STATE::ACTION) != static_cast<short>(ENEMY::ACTION::DEAD)) {
				SDL_Rect dst = m_enemy[id][cn]->GetPos();
				dst.x -= windowPosX;
				DrawGauge(renderer, dst, m_enemy[id][cn]->GetState(CHARA_STATE::HP), "green");
				UpdateAnimation(renderer, m_characterTexture[id], id, *m_enemy[id][cn], m_enemyAction, windowPosX);
			}
		}
	}

}

inline const void OutputError(const char* msg) {
	cout << msg << ":" << SDL_GetError() << endl;  exit(1);
}

SDL_Color ConvertToRGB(const char* color) {
	if (color == "red") {
		SDL_Color c = { 255, 0, 0, 255 };
		return c;
	}
	else if (color == "green") {
		SDL_Color c = { 0, 255, 0, 255 };
		return c;
	}
	else if (color == "blue") {
		SDL_Color c = { 0, 0, 255, 255 };
		return c;
	}
	else if (color == "yellow") {
		SDL_Color c = { 255, 255, 0, 255 };
		return c;
	}
	else if (color == "black") {
		SDL_Color c = { 0, 0, 0, 255 };
		return c;
	}
	else if (color == "white") {
		SDL_Color c = { 255, 255, 255, 255 };
		return c;
	}
	else {
		cout << color << "is not ready or not exist." << endl;
		exit(1);
	}
}

void DrawWorld(SDL_Renderer *renderer, SDL_Surface *world, SDL_Surface *mapChip, vector<vector<int>> mapData) {
	for (int w = 0; w < WORLD_WIDTH; w++) {
		for (int h = 0; h < WORLD_HEIGHT; h++) {
			SDL_Rect src = { MAP_CHIPSIZE * mapData[w][h], 0 , MAP_CHIPSIZE, MAP_CHIPSIZE }, dst = { MAP_CHIPSIZE * w, MAP_CHIPSIZE * h, MAP_CHIPSIZE, MAP_CHIPSIZE };
			SDL_BlitSurface(mapChip, &src, world, &dst);
		}
	}
}

template <typename T>
const int ActionToIndex( vector<T> actionList, int nowAction) {
	T action = static_cast<T>(nowAction);
	int min = 0;
	int max = actionList.size();

	while (min < max) {
		int mid = (max + min) / 2;
		if (actionList[mid] == action) {
			return mid;
		}
		else if (action < actionList[mid]) {
			max = mid;
		}
		else {
			min = mid;
		}
	}

	cout << "that action is not exist." << endl;
	return -1;
}

void DrawGauge(SDL_Renderer *renderer, SDL_Rect dst, int value, const char *color) {
	dst.w = value * 4;
	dst.h = 40;
	SDL_Rect pos = dst;
	SDL_Color c = ConvertToRGB(color);
	SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
	SDL_RenderFillRect(renderer, &pos);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderDrawRect(renderer, &dst);
}

void UpdateAnimation(SDL_Renderer *renderer, SDL_Texture **charaTexture, int id, Character &my, vector<vector<ENEMY::ACTION>> m_enemyAction, int windowPosX) {
	SDL_Rect srcChara = { my.GetState(CHARA_STATE::FRAME_H) * IMG_SIZE[id], my.GetState(CHARA_STATE::FRAME_V) * IMG_SIZE[id], IMG_SIZE[id], IMG_SIZE[id] };
	SDL_Rect dstRect = my.GetPos();
	dstRect.x -= windowPosX;

	short action = my.GetState(CHARA_STATE::ACTION);
	if (id < static_cast<int>(CHARA_ID::BOSS)) { action = ActionToIndex(m_enemyAction[id], action); }

	if (my.GetState(CHARA_STATE::DIR) == g_right) {//右向き
		SDL_RenderCopy(renderer, charaTexture[action], &srcChara, &dstRect);
	}
	else {//左
		SDL_RenderCopyEx(renderer, charaTexture[action], &srcChara, &dstRect, 180, NULL, SDL_FLIP_VERTICAL);
	}
};