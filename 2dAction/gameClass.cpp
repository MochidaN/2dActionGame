#include "stdafx.h"
#include "gameClass.hpp"
#include "enemy.hpp"
#include "player.hpp"

const short IMG_SIZE[static_cast<short>(CHARA_ID::NUM)] = { ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE, PLAYER_SIZE };
const short CHARA_NUM[static_cast<short>(CHARA_ID::NUM)] = { 0, 0, 1, 0, 1 };
const short X = 0, Y = 1, W = 2, H = 3;
//const short g_playerMoveX = 4;
//const short g_playerStepX = 20;
//const short g_yAdd = -28;
//const short g_yAdd_ground = -1;

Sequence::Sequence() {
	m_next = SEQ_ID::NONE;
}

SEQ_ID Sequence::MoveTo() {
	return m_next;
}

Game::Game(SDL_Renderer *renderer, vector<vector<ENEMY::ACTION>> enemyAction, vector<vector<vector<int>>> maxFrame, vector<vector<unsigned int>> attackActive, vector<vector<vector<vector<int>>>> attackRect, vector<vector<unsigned int>> hurtActive, vector<vector<vector<vector<int>>>> hurtRect) {
	m_joystick = SDL_JoystickOpen(0);//ジョイスティックを構造体に割り当てて有効化
	if (!SDL_JoystickGetAttached(m_joystick)) {
		cout << "failed to open joystick" << endl;
	}

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
	const short action[static_cast<short>(CHARA_ID::NUM)-1] = { static_cast<short>(ENEMY::ACTION::GUARD), static_cast<short>(ENEMY::ACTION::STAND),  static_cast<short>(ENEMY::ACTION::STAND),  static_cast<short>(ENEMY::ACTION::STAND)};
	unsigned int time = SDL_GetTicks();
	m_enemy = new Enemy**[enemyNum];
	for (int id = 0; id < enemyNum; id++) {
		m_enemy[id] = new Enemy*[CHARA_NUM[id]];
		for (int cn = 0; cn < CHARA_NUM[id]; cn++) {
			SDL_Rect pos = { 400, 0, IMG_SIZE[id], IMG_SIZE[id]};
			switch (id) {
			case static_cast<int>(CHARA_ID::GUARD) : {
				m_enemy[id][cn] = new EnemyGuard(100, 10, 10, pos, action[id], time, g_yAdd_ground);
				break;
			}
			case static_cast<int>(CHARA_ID::WARP) : {
				m_enemy[id][cn] = new EnemyWarp(100, 10, 10, pos, action[id], time, g_yAdd_ground);
				break;
			}
			case static_cast<int>(CHARA_ID::KICK) : {
				m_enemy[id][cn] = new EnemyKick(100, 10, 10, pos, action[id], time, g_yAdd_ground);
				break;
			}
			case static_cast<int>(CHARA_ID::BOSS) : {
				m_enemy[id][cn] = new EnemyBoss(100, 10, 10, pos, action[id], time, g_yAdd_ground);
				break;
			}
			}
		}
	}
	const short playerID = static_cast<short>(CHARA_ID::PLAYER);
	const short stand = static_cast<short>(PLAYER::ACTION::STAND);
	SDL_Rect pos = { 0, WINDOW_HEIGHT * MAP_CHIPSIZE - (MAP_CHIPSIZE + m_hurtRect[1][stand][0][Y] + m_hurtRect[1][stand][0][H]), IMG_SIZE[playerID], IMG_SIZE[playerID] };
	m_player = new Player(100, 10, 10, pos, static_cast<short>(PLAYER::ACTION::STAND), time, g_yAdd_ground);
}

Game::~Game() {
	if (SDL_JoystickGetAttached(m_joystick)) { SDL_JoystickClose(m_joystick); }

	SDL_DestroyTexture(m_world);
	const int charaNum = static_cast<int>(CHARA_ID::NUM);
	vector<short> actionNum;
	for (int id = 0, end = m_enemyAction.size(); id < end; id++) {
		actionNum.push_back(m_enemyAction[id].size());
	}
	actionNum.push_back(static_cast<short>(ENEMY::ACTION::NUM));
	actionNum.push_back(static_cast<short>(PLAYER::ACTION::NUM));

	for (int id = 0; id < charaNum - 1; id++) {
		for (int cn = 0; cn < actionNum[id]; cn++) {
			SDL_DestroyTexture(m_characterTexture[id][cn]);
		}
		for (int cn = 0; cn < CHARA_NUM[id]; cn++) {
			delete m_enemy[id][cn];
		}
		delete[] m_characterTexture[id];
		delete[] m_enemy[id];
	}
	delete m_player;
	delete m_characterTexture[static_cast<int>(CHARA_ID::PLAYER)];
	delete[] m_characterTexture;
}

bool Game::Update(SDL_Renderer *renderer) {
	const int winWidth = WINDOW_WIDTH * MAP_CHIPSIZE;
	int windowPosX = (m_player->GetPos().x + m_hurtRect[1][static_cast<short>(PLAYER::ACTION::STAND)][0][X]) - (WINDOW_WIDTH * MAP_CHIPSIZE / 2);//描画領域の左端
	if (windowPosX < 0) { windowPosX = 0; }
	else if (windowPosX + winWidth > WORLD_WIDTH * MAP_CHIPSIZE) { windowPosX = (WORLD_WIDTH - WINDOW_WIDTH) * MAP_CHIPSIZE; }

	Draw(renderer, windowPosX);	

	/*
	const short playerID = static_cast<short>(CHARA_ID::PLAYER);
	const short act = m_player->GetState(CHARA_STATE::ACTION);
	const short pNowFrame = ReturnFrameNum(m_maxFrame[playerID][act][0], *m_player);
	SDL_Rect playerPos = ReturnCharaRect(m_player->GetPos(), m_hurtRect[1][act][pNowFrame], m_player->GetState(CHARA_STATE::DIR));
	SDL_Rect pAtkRect = { -1, -1, -1, -1 };
	if (m_player->GetActiveBit(CHARA_STATE::ATTACK_ACTIVE, pNowFrame) != 0) {
		pAtkRect = ReturnCharaRect(m_player->GetPos(), m_attackRect[1][act][pNowFrame], m_player->GetState(CHARA_STATE::DIR));
	}

#ifdef _DEBUG
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_Rect dst = playerPos;
	dst.x -= windowPosX;
	SDL_RenderDrawRect(renderer, &dst);
	if (pAtkRect.x >= 0) {
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		dst = pAtkRect;
		dst.x -= windowPosX;
		SDL_RenderDrawRect(renderer, &dst);
	}
#endif

	const short charaNum = static_cast<short>(CHARA_ID::NUM) - 1;
	for (int id = 0; id < charaNum; id++) {
		for (int cn = 0; cn < CHARA_NUM[id]; cn++) {
			const short action = m_enemy[id][cn]->GetState(CHARA_STATE::ACTION);
			const short eNowFrame = ReturnFrameNum(m_maxFrame[id][action][0], *m_enemy[id][cn]);
			SDL_Rect enemyPos = ReturnCharaRect(m_enemy[id][cn]->GetPos(), m_hurtRect[0][action][eNowFrame], m_enemy[id][cn]->GetState(CHARA_STATE::DIR));
			if ((windowPosX <= enemyPos.x + enemyPos.w) && (windowPosX + winWidth >= enemyPos.x)) {//ウィンドウ内にいる

#ifdef _DEBUG
				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
				SDL_Rect dst = enemyPos;
				dst.x -= windowPosX;
				SDL_RenderDrawRect(renderer, &dst);
#endif

				if (DetectCollisionRect(playerPos, enemyPos) == true) {//プレイヤと敵がぶつかっている
					SDL_Rect p = m_player->GetPos();
					SDL_Rect e = m_enemy[id][cn]->GetPos();
					const short dst = g_playerMoveX;
					int pDst = (playerPos.x <= enemyPos.x) ? -dst : dst;
					int eDst = (playerPos.x <= enemyPos.x) ? dst : -dst;
					m_player->SetPos(MovePositionX(p, m_hurtRect[1][act][pNowFrame], pDst, WORLD_WIDTH, m_mapData));
					m_enemy[id][cn]->SetPos(MovePositionX(e, m_hurtRect[0][action][eNowFrame], eDst, WORLD_WIDTH, m_mapData));
				}

				if (pAtkRect.x != -1) {
					if (DetectCollisionRect(pAtkRect, enemyPos) == true) {
						m_enemy[id][cn]->SetState(CHARA_STATE::ACTION, static_cast<short>(ENEMY::ACTION::HIT));
						m_player->SetActiveBit(CHARA_STATE::ATTACK_ACTIVE, 0);//攻撃判定をなくす
					}
				}
				
				if (m_enemy[id][cn]->GetActiveBit(CHARA_STATE::ATTACK_ACTIVE, eNowFrame) != 0) {//敵の行動に攻撃判定がある
					SDL_Rect eAtkRect = ReturnCharaRect(m_enemy[id][cn]->GetPos(), m_attackRect[0][action][eNowFrame], m_enemy[id][cn]->GetState(CHARA_STATE::DIR));
					if (DetectCollisionRect(playerPos, eAtkRect) == true) {

					}
#ifdef _DEBUG
					SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
					SDL_Rect dst = eAtkRect;
					dst.x -= windowPosX;
					SDL_RenderDrawRect(renderer, &dst);
#endif
				}
			}
		}
	}*/

	EVENT event = GetEvent(m_joystick);
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
	m_player->Update(nowTime, m_maxFrame[playerID], m_hurtRect[playerID], m_hurtRect[enemyID], m_mapData, event);
	const int pAction = m_player->GetState(CHARA_STATE::ACTION);
	const int pFrame = ReturnFrameNum(m_maxFrame[playerID][pAction][0], *m_player);
	for (int id = 0; id < charaNum; id++) {
		for (int cn = 0; cn < CHARA_NUM[id]; cn++) {
			const int eAction = m_enemy[id][cn]->GetState(CHARA_STATE::ACTION);
			const int eFrame = ReturnFrameNum(m_maxFrame[enemyID][eAction][0], *m_enemy[id][cn]);
			
			m_player->CollisionChara(m_hurtRect[playerID][pAction][pFrame], m_hurtRect[enemyID][eAction][eFrame], m_mapData, *m_enemy[id][cn]);
			m_player->HandleAttack(*m_enemy[id][cn], pFrame, eFrame, m_attackRect[playerID][pAction][pFrame], m_hurtRect[enemyID][eAction][eFrame]);
			m_enemy[id][cn]->Update(nowTime, m_maxFrame[enemyID], m_hurtRect[enemyID], m_hurtRect[playerID], m_attackRect[enemyID], m_mapData, *m_player);
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
			auto UpdateAnimation = [renderer, id, windowPosX](SDL_Texture **charaTexture, Character &my, vector<vector<int>> maxFrame, vector<vector<ENEMY::ACTION>> m_enemyAction, vector<vector<vector<vector<int>>>> hurtRect, vector<vector<int>> m_mapData) {
				SDL_Rect srcChara = { my.GetState(CHARA_STATE::FRAME_H) * IMG_SIZE[id], my.GetState(CHARA_STATE::FRAME_V) * IMG_SIZE[id], IMG_SIZE[id], IMG_SIZE[id] };
				SDL_Rect dstRect = my.GetPos();
				dstRect.x -= windowPosX;

				short action = my.GetState(CHARA_STATE::ACTION);
				if (id < static_cast<int>(CHARA_ID::BOSS)) { action = ActionToIndex(m_enemyAction[id], action); }

				if (my.GetState(CHARA_STATE::DIR) == g_right) {//右向き
					SDL_RenderCopy(renderer, charaTexture[action], &srcChara, &dstRect);
				}
				else {//左
					if (id == 2) { cout << action << endl; }

					SDL_RenderCopyEx(renderer, charaTexture[action], &srcChara, &dstRect, 180, NULL, SDL_FLIP_VERTICAL);
				}
			};

			if (id == playerID) {
				UpdateAnimation(m_characterTexture[id], *m_player,  m_maxFrame[1], m_enemyAction, m_hurtRect, m_mapData);
			}
			else {
				UpdateAnimation(m_characterTexture[id], *m_enemy[id][cn], m_maxFrame[0], m_enemyAction, m_hurtRect, m_mapData);
			}
		}
	}

}

EVENT GetEvent(SDL_Joystick *joystick) {
	if (!SDL_JoystickGetAttached(joystick)) {//まだ開いてないとき
		joystick = SDL_JoystickOpen(0);
	}

	SDL_Event event;
	if (SDL_PollEvent(&event)) {//キューにイベントがあったとき
		switch (event.type) {
		case SDL_JOYBUTTONDOWN:
			if (SDL_JoystickGetButton(joystick, 1)) {//〇ボタン
				return EVENT::JOY_CIRCLE;
			}
			else if (SDL_JoystickGetButton(joystick, 0)) {//×ボタン
				return EVENT::JOY_CROSS;
			}
			else if (SDL_JoystickGetButton(joystick, 9)) {//L1
				return EVENT::JOY_L1_DOWN;
			}
			else if (SDL_JoystickGetButton(joystick, 10)) {//R1
				return EVENT::JOY_R1;
			}
			else if (SDL_JoystickGetButton(joystick, 13)) {//左
				return EVENT::JOY_HAT_LEFT;
			}
			else if (SDL_JoystickGetButton(joystick, 14)) {//右
				return EVENT::JOY_HAT_RIGHT;
			}
			break;
		case SDL_JOYBUTTONUP:
			if ((int)event.jbutton.button == 9) {//L1
				return EVENT::JOY_L1_UP;
			}
			else if ((int)event.jbutton.button == 13 || (int)event.jbutton.button == 14) {
				return EVENT::JOY_HAT_CENTERED;
			}
			break;
		case SDL_JOYHATMOTION: {//十字キーが押されたとき
			switch (SDL_JoystickGetHat(joystick, 0)) {
			case SDL_HAT_RIGHT:
				return EVENT::JOY_HAT_RIGHT;
				break;
			case SDL_HAT_LEFT:
				return EVENT::JOY_HAT_LEFT;
				break;
			default:
				return EVENT::JOY_HAT_CENTERED;
				break;
			}
			break;
		}
		case SDL_QUIT: {//ウィンドウの×ボタン
			return EVENT::QUIT;
			break;
		}
		}
	}
	return EVENT::NONE;
}

inline void OutputError(const char* msg) {
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

void CreateText(SDL_Renderer *renderer, SDL_Texture *texture, const char *text, int size, const char *color) {
#ifdef _DEBUG
	const char* fontPath = "../font/ipaexg.ttf";
#endif

#ifdef NDEBUG
	const char* fontPath = "../../font/ipaexg.ttf";
#endif

	TTF_Font *font = TTF_OpenFont(fontPath, size);
	if (!font) {
		cout << "TTF_OpenFont:" << TTF_GetError() << endl;
		exit(1);
	}

	SDL_Color fontColor = ConvertToRGB(color);
	SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text, fontColor);
	if (surface == NULL) {
		OutputError("failed to create font surface");
	}

	texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == NULL) {
		OutputError("failed to create texture");
	}
	SDL_FreeSurface(surface);
	TTF_CloseFont(font);
}

vector<string> ReadFile(string fileName) {
	std::ifstream ifs(fileName);
	if (!ifs) {
		cout << "failed to open file:" << fileName << endl;
		exit(1);
	}

	vector<string> data;
	std::string str;
	while (getline(ifs, str)) {
		data.push_back(str);
	}

	return data;
}

vector<int> split(string str, const char mark) {
	stringstream ss{ str };
	string buf;
	vector<int> data;
	while (getline(ss, buf, mark)) {
		data.push_back(atoi(buf.c_str()));
	}
	return data;
}

vector<vector<int>> ReadFileSplit(string fileName, const char mark) {
	ifstream ifs(fileName);
	if (!ifs) {
		cout << "failed to open file:" << fileName << endl;
		exit(1);
	}

	string str;
	vector<vector<int>> data;
	while (getline(ifs, str)) {
		data.push_back(split(str, mark));
	}
	return data;
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