#include "stdafx.h"
#include <algorithm>

#include "sequence.hpp"
#include "parameter.hpp"

const short ANIME_INTERVAL = 1000 / 30;//アニメーション間隔

const short IMG_SIZE[static_cast<short>(CHARA_ID::NUM)] = { ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE, PLAYER_SIZE };
const short CHARA_NUM[static_cast<short>(CHARA_ID::NUM)] = { 1, 0, 0, 0, 1 };
const short X = 0, Y = 1, W = 2, H = 3;
const short g_playerMoveX = 2;
const short g_playerStepX = 40;
const short g_yAdd = -25;
const short g_yAdd_ground = -1;

//worldのサーフェイスにマップを描画
//SDL_Surface *world：ワールドサーフェイス
// SDL_Surface *mapChip：マップチップサーフェイス
//vector<vector<int>> mapData：マップの配置データ
//返値：なし
void DrawWorld(SDL_Renderer *renderer, SDL_Surface *world, SDL_Surface *mapChip, vector<vector<int>> mapData);

//次のアニメーションフレームを計算
//hengFrame：現在の横コマ数
//verticalFrame：現在の縦コマ数
//maxFrame：最大コマ数(0:横 1:縦)
//endAnimation：アニメーションが一周したかどうか
//返値：次のフレーム数
vector<short> RetrunNextFrame(short hengFrame, short verticalFrame, vector<int> maxFrame, bool &endAnimation);

//アクション名を配列番号に変換
//actionList：アクション一覧
//nowAction：現在のアクション
template <typename T = ENEMY_ACTION> const int ActionToIndex(vector<T> actionList, int nowAction);

//x座標の移動
//nowPos：現在の座標(画像全体)
//hurtRext：キャラの当たり判定
//distance：移動距離
//worldWidth：ワールドの端座標
//mapData：マップ番号データ
//返値：移動後の座標
SDL_Rect MovePositionX(SDL_Rect nowPos, vector<int> hurtRect, int distance, int worldWidth, vector<vector<int>> mapData);

//yAdd：y方向の移動量
SDL_Rect MovePositionY(SDL_Rect nowPos, vector<int> hurtRect, short &yAdd, int worldHeight, vector<vector<int>> mapData);

//キャラ当たり判定の座標を反転
//rect：右向きの座標
//imgWidth：画像の幅
vector<int> FlipRect(vector<int> rect, int imgWidth);

void CollisionX(SDL_Rect &nowPos, int prevX, vector<int> hurtRect, vector<vector<int>> mapData);

//移動時の衝突判定
//返値：地上にいるかどうか
bool CollisionY(SDL_Rect &nowPos, int prevY, vector<int> hurtRect, vector<vector<int>> mapData);

Sequence::Sequence() {
	m_next = SEQ_ID::NONE;
}

SEQ_ID Sequence::MoveTo() {
	return m_next;
}

Game::Game(SDL_Renderer *renderer, vector<vector<ENEMY_ACTION>> enemyAction, vector<vector<vector<int>>> maxFrame, vector<vector<unsigned int>> attackActive, vector<vector<vector<vector<int>>>> attackRect, vector<vector<unsigned int>> hurtActive, vector<vector<vector<vector<int>>>> hurtRect) {
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
	m_mapChip = IMG_Load(mapImgPath);
	if (worldSurface == NULL || m_mapChip == NULL) {
		OutputError("failed to create world surface");
	}
	m_mapData = ReadFileSplit(mapDataPath, ',');
	DrawWorld(renderer, worldSurface, m_mapChip, m_mapData);
	if ((m_world = SDL_CreateTextureFromSurface(renderer, worldSurface)) == NULL) {
		OutputError("failed to create world texture");
	}

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
	const short action[static_cast<short>(CHARA_ID::NUM)-1] = { static_cast<short>(ENEMY_ACTION::GUARD), static_cast<short>(ENEMY_ACTION::STAND),  static_cast<short>(ENEMY_ACTION::STAND),  static_cast<short>(ENEMY_ACTION::STAND)};
	unsigned int time = SDL_GetTicks();
	for (int id = 0; id < enemyNum; id++) {
		m_chara[id] = new Character*[CHARA_NUM[id]];
		for (int cn = 0; cn < CHARA_NUM[id]; cn++) {
			SDL_Rect pos = { 400, 0, IMG_SIZE[id], IMG_SIZE[id]};
			m_chara[id][cn] = new Enemy(100, 10, 10, pos, action[id], time);
		}
	}
	const short playerID = static_cast<short>(CHARA_ID::PLAYER);
	const short stand = static_cast<short>(PLAYER_ACTION::STAND);
	SDL_Rect pos = { 0, WINDOW_HEIGHT * MAP_CHIPSIZE - (MAP_CHIPSIZE + m_hurtRect[1][stand][0][Y] + m_hurtRect[1][stand][0][H]), IMG_SIZE[playerID], IMG_SIZE[playerID] };
	m_chara[playerID] = new Character*[CHARA_NUM[playerID]];
	m_chara[playerID][0] = new Player(100, 10, 10, pos, static_cast<short>(PLAYER_ACTION::STAND), time, g_yAdd_ground);
}

Game::~Game() {
	if (SDL_JoystickGetAttached(m_joystick)) {
		SDL_JoystickClose(m_joystick);
	}

	SDL_DestroyTexture(m_world);
	SDL_FreeSurface(m_mapChip);
	const int charaNum = static_cast<int>(CHARA_ID::NUM);
	vector<short> actionNum;
	for (int id = 0, end = m_enemyAction.size(); id < end; id++) {
		actionNum.push_back(m_enemyAction[id].size());
	}
	actionNum.push_back(static_cast<short>(ENEMY_ACTION::NUM));
	actionNum.push_back(static_cast<short>(PLAYER_ACTION::NUM));

	for (int id = 0; id < charaNum; id++) {
		for (int cn = 0; cn < actionNum[id]; cn++) {
			SDL_DestroyTexture(m_characterTexture[id][cn]);
		}
		for (int cn = 0; cn < CHARA_NUM[id]; cn++) {
			delete m_chara[id][cn];
		}
		delete[] m_characterTexture[id];
		delete[] m_chara[id];
	}
	delete[] m_characterTexture;
}

bool Game::Update(SDL_Renderer *renderer) {
	Draw(renderer);

	EVENT event = GetEvent(m_joystick);
	const short playerID = static_cast<short>(CHARA_ID::PLAYER);
	switch (event) {
	case EVENT::QUIT: {
		m_next = SEQ_ID::QUIT;
		return false;
		break;
	}
	case EVENT::JOY_CIRCLE: {
		if (m_chara[playerID][0]->GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground) {
			m_chara[playerID][0]->SetState(CHARA_STATE::ACTION, static_cast<int>(PLAYER_ACTION::STEP));
			m_chara[playerID][0]->SetState(CHARA_STATE::X_ADD, g_playerStepX);
		}
		break;
	}
	case EVENT::JOY_CROSS: {
		if (m_chara[playerID][0]->GetState(CHARA_STATE::Y_ADD) == g_yAdd_ground) {
			m_chara[playerID][0]->SetState(CHARA_STATE::ACTION, static_cast<int>(PLAYER_ACTION::JUMP));
			m_chara[playerID][0]->SetState(CHARA_STATE::Y_ADD, g_yAdd);
		}
		else {
			//cout << m_chara[playerID][0]->GetState(CHARA_STATE::Y_ADD) << endl;
		}
		break;
	}
	case EVENT::JOY_HAT_LEFT: {
		m_chara[playerID][0]->SetState(CHARA_STATE::ACTION, static_cast<int>(PLAYER_ACTION::WALK));
		m_chara[playerID][0]->SetState(CHARA_STATE::X_ADD, g_playerMoveX);
		
		if (m_chara[playerID][0]->GetState(CHARA_STATE::DIR) == g_right) {//右向きから左向きになるとき
			SDL_Rect pos = m_chara[playerID][0]->GetPos();
			const short action = m_chara[playerID][0]->GetState(CHARA_STATE::ACTION);
			vector<int> fRect = FlipRect(m_hurtRect[1][action][0], IMG_SIZE[playerID]);
			pos.x -= (fRect[X] - m_hurtRect[1][action][0][X]);
			m_chara[playerID][0]->SetPos(pos);
			m_chara[playerID][0]->SetState(CHARA_STATE::DIR, g_left);
		}
		break;
	}
	case EVENT::JOY_HAT_RIGHT: {
		m_chara[playerID][0]->SetState(CHARA_STATE::ACTION, static_cast<int>(PLAYER_ACTION::WALK));
		m_chara[playerID][0]->SetState(CHARA_STATE::X_ADD, g_playerMoveX);

		if (m_chara[playerID][0]->GetState(CHARA_STATE::DIR) == g_left) {//左向きから右向きになるとき
			SDL_Rect pos = m_chara[playerID][0]->GetPos();
			const short action = m_chara[playerID][0]->GetState(CHARA_STATE::ACTION);
			vector<int> fRect = FlipRect(m_hurtRect[1][action][0], IMG_SIZE[playerID]);
			pos.x -= (m_hurtRect[1][action][0][X] - fRect[X]);
			m_chara[playerID][0]->SetPos(pos);
			m_chara[playerID][0]->SetState(CHARA_STATE::DIR, g_right);
		}
		break;
	}
	case EVENT::JOY_HAT_CENTERED: {
		m_chara[playerID][0]->SetState(CHARA_STATE::X_ADD, 0);
		m_chara[playerID][0]->SetState(CHARA_STATE::ACTION, static_cast<int>(PLAYER_ACTION::STAND));
		break;
	}
	}

	SDL_Rect pos = m_chara[playerID][0]->GetPos();
	//const short action = m_chara[playerID][0]->GetState(CHARA_STATE::ACTION);
	vector<int> hurtRect = m_hurtRect[1][m_chara[playerID][0]->GetState(CHARA_STATE::ACTION)][0];
	if (m_chara[playerID][0]->GetState(CHARA_STATE::DIR) == g_left ) {//左向きの時反転
		hurtRect = FlipRect(hurtRect, IMG_SIZE[playerID]);
	}
	if (m_chara[playerID][0]->GetState(CHARA_STATE::X_ADD) > 0) {
		pos = MovePositionX(pos, hurtRect, m_chara[playerID][0]->GetState(CHARA_STATE::X_ADD) * m_chara[playerID][0]->GetState(CHARA_STATE::DIR), WORLD_WIDTH, m_mapData);
	}
	short yAdd = m_chara[playerID][0]->GetState(CHARA_STATE::Y_ADD);
	m_chara[playerID][0]->SetPos(MovePositionY(pos, hurtRect, yAdd, WORLD_HEIGHT, m_mapData));
	//m_hurtRect[1][action][0]　コマ数参照に変更する
	m_chara[playerID][0]->SetState(CHARA_STATE::Y_ADD, yAdd);

	return true;
}

void Game::Draw(SDL_Renderer *renderer) {
	int windowPosX = (m_chara[static_cast<short>(CHARA_ID::PLAYER)][0]->GetPos().x + m_hurtRect[1][static_cast<short>(PLAYER_ACTION::STAND)][0][X]) - (WINDOW_WIDTH * MAP_CHIPSIZE / 2);
	
	if (windowPosX < 0) { windowPosX = 0; }
	else if (windowPosX + WINDOW_WIDTH * MAP_CHIPSIZE > WORLD_WIDTH * MAP_CHIPSIZE) { windowPosX = (WORLD_WIDTH - WINDOW_WIDTH) * MAP_CHIPSIZE; }
	
	SDL_Rect src = { windowPosX, 0, WINDOW_WIDTH * MAP_CHIPSIZE, WINDOW_HEIGHT * MAP_CHIPSIZE };
	SDL_RenderCopy(renderer, m_world, &src, NULL);

	unsigned int nowTime = SDL_GetTicks();
	const short charaNum = static_cast<short>(CHARA_ID::NUM);
	for (int id = 0; id < charaNum; id++) {
		for (int cn = 0; cn < CHARA_NUM[id]; cn++) {
			short action = m_chara[id][cn]->GetState(CHARA_STATE::ACTION);
			if (id < static_cast<short>(CHARA_ID::BOSS)) {
				action = ActionToIndex(m_enemyAction[id], action);
			}
			vector<short> frame = { m_chara[id][cn]->GetState(CHARA_STATE::FRAME_H), m_chara[id][cn]->GetState(CHARA_STATE::FRAME_V) };
			if (nowTime - m_chara[id][cn]->GetTime() >= ANIME_INTERVAL) {
				//アニメーション更新
				bool endAnimation = false;
				frame = RetrunNextFrame(frame[0], frame[1], m_maxFrame[id][action], endAnimation);
				if (endAnimation == true && m_chara[id][cn]->GetState(CHARA_STATE::ACTION) != static_cast<short>(ENEMY_ACTION::WALK)) {
					if (id != 0) {
						m_chara[id][cn]->SetState(CHARA_STATE::ACTION, 0);
						m_chara[id][cn]->SetState(CHARA_STATE::X_ADD, 0);
					}
					else {
						m_chara[id][cn]->SetState(CHARA_STATE::ACTION, static_cast<int>(ENEMY_ACTION::GUARD));
					}
				}

				m_chara[id][cn]->SetState(CHARA_STATE::FRAME_H, frame[0]);
				m_chara[id][cn]->SetState(CHARA_STATE::FRAME_V, frame[1]);
				m_chara[id][cn]->SetTime(nowTime);
			}

			static const short playerID = static_cast<short>(CHARA_ID::PLAYER);
			SDL_Rect srcChara = { frame[0] * IMG_SIZE[id], frame[1] * IMG_SIZE[id], IMG_SIZE[id], IMG_SIZE[id] };
			SDL_Rect dstRect = m_chara[id][cn]->GetPos();
			dstRect.x -= windowPosX;
			if (m_chara[id][cn]->GetState(CHARA_STATE::DIR) == true) {//右向き
				SDL_RenderCopy(renderer, m_characterTexture[id][action], &srcChara, &dstRect);
			}
			else {//左
				SDL_RenderCopyEx(renderer, m_characterTexture[id][action], &srcChara, &dstRect, 180, NULL, SDL_FLIP_VERTICAL);
			}
		}
	}

}

EVENT GetEvent(SDL_Joystick *joystick) {
	if (!SDL_JoystickGetAttached(joystick)) {//まだ開いてないとき
		joystick = SDL_JoystickOpen(0);
		if (SDL_JoystickGetAttached(joystick)) {
			cout << "joystick opened" << endl;
		}
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
			if ((int)event.jbutton.button == 10) {//L1
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

vector<short> RetrunNextFrame(short hengFrame, short verticalFrame, vector<int> maxFrame, bool &endAnimation) {
	endAnimation = false;
	if (++hengFrame >= maxFrame[0]) {
		hengFrame = 0;
		if (++verticalFrame >= maxFrame[1]) {
			verticalFrame = 0;
			endAnimation = true;
		}
	}

	vector<short> nextFrame = { hengFrame, verticalFrame };
	return nextFrame;
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

SDL_Rect MovePositionX(SDL_Rect nowPos, vector<int> hurtRect, int distance, int worldWidth, vector<vector<int>> mapData) {
	const int prevX = nowPos.x;
	nowPos.x += distance;
	const int charaPos = nowPos.x + hurtRect[X];
	if (charaPos < 0) {
		nowPos.x = -hurtRect[X];
	}
	else if(charaPos + hurtRect[W] > worldWidth * MAP_CHIPSIZE) {
		nowPos.x = worldWidth * MAP_CHIPSIZE - (hurtRect[X] + hurtRect[W]);
	}
	
	CollisionX(nowPos, prevX, hurtRect, mapData);
	return nowPos;
}

SDL_Rect MovePositionY(SDL_Rect nowPos, vector<int> hurtRect, short &yAdd, int worldHeight, vector<vector<int>> mapData) {
	yAdd++;
	const int prevY = nowPos.y;
	nowPos.y += yAdd;
	
	const int charaPos = nowPos.y + hurtRect[Y];
	if (charaPos < 0) {
		nowPos.y = -hurtRect[Y];
	}
	else if (charaPos + hurtRect[H] > worldHeight * MAP_CHIPSIZE) {
		nowPos.y = worldHeight * MAP_CHIPSIZE - (hurtRect[Y] + hurtRect[H]);
	}

	if (CollisionY(nowPos, nowPos.y, hurtRect, mapData)) {
		yAdd = g_yAdd_ground;
	}
	
	return nowPos;
}

inline vector<int> FlipRect(vector<int> rect, int imgWidth) {
	rect[X] = imgWidth - (rect[X] + rect[W]);
	return rect;
}


void CollisionX(SDL_Rect &nowPos, int prevX, vector<int> hurtRect, vector<vector<int>> mapData) {
	const short left = (nowPos.x + hurtRect[X]) / MAP_CHIPSIZE;
	short right = (nowPos.x + hurtRect[X] + hurtRect[W] + MAP_CHIPSIZE - 1) / MAP_CHIPSIZE;
	if (right > WORLD_WIDTH) { right = WORLD_WIDTH; }

	const short top = (nowPos.y + hurtRect[Y]) / MAP_CHIPSIZE;
	short bottom = (nowPos.y + hurtRect[Y] + hurtRect[H]) / MAP_CHIPSIZE;
	if (bottom > WORLD_HEIGHT) { bottom = WORLD_HEIGHT; }

	for (int w = left; w < right; w++) {
		for (int h = top; h < bottom; h++) {
			if (mapData[w][h] >= 1) {
				int x = max(nowPos.x, prevX);
				x /= MAP_CHIPSIZE;
				if (nowPos.x - prevX < 0) { x++; }
				x *= MAP_CHIPSIZE;
				nowPos.x = x;
				
				//nowPos.x = prevX;
			}
		}
	}
}

bool CollisionY(SDL_Rect &nowPos, int prevY, vector<int> hurtRect, vector<vector<int>> mapData) {
	bool landing = false;//着地したか否か
	const short left = (nowPos.x + hurtRect[X]) / MAP_CHIPSIZE;
	short right = (nowPos.x + hurtRect[X] + hurtRect[W] + MAP_CHIPSIZE - 1) / MAP_CHIPSIZE;
	if (right > WORLD_WIDTH) { right = WORLD_WIDTH; }
	const short top = (nowPos.y + hurtRect[Y]) / MAP_CHIPSIZE;
	short bottom = (nowPos.y + hurtRect[Y] + hurtRect[H] + MAP_CHIPSIZE - 1) / MAP_CHIPSIZE;
	if (bottom > WORLD_HEIGHT) {
		bottom = WORLD_HEIGHT;
	}
	
	for (int w = left; w < right; w++) {
		for (int h = top; h < bottom; h++) {
			if (mapData[w][h] >= 1) {
				cout << nowPos.y << "  " << prevY << endl;
				int y = max(nowPos.y, prevY);
				y /= MAP_CHIPSIZE;
				y *= MAP_CHIPSIZE;
				nowPos.y = y;
				
				landing = true;
			}
		}
	}

	return landing;
}