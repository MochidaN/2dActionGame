#include "stdafx.h"
#include "sequence.hpp"
#include "parameter.hpp"

//worldのサーフェイスにマップを描画
//SDL_Surface *world：ワールドサーフェイス
// SDL_Surface *mapChip：マップチップサーフェイス
//vector<vector<int>> mapData：マップの配置データ
//返値：なし
void DrawWorld(SDL_Renderer *renderer, SDL_Surface *world, SDL_Surface *mapChip, vector<vector<int>> mapData);

Sequence::Sequence() {
	m_next = SEQ_ID::NONE;
}

SEQ_ID Sequence::MoveTo() {
	return m_next;
}

Game::Game(SDL_Renderer *renderer, vector<vector<ENEMY_ACTION>> enemyAction, vector<vector<vector<int>>> maxFrame, vector<vector<unsigned short>> attackActive, vector<vector<vector<vector<int>>>> attackRect, vector<vector<unsigned short>> hurtActive, vector<vector<vector<vector<int>>>> hurtRect) {
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
			if ((img = IMG_Load(imagePath[i].c_str())) == NULL) {
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
}

Game::~Game() {
	if (SDL_JoystickGetAttached(m_joystick)) {
		SDL_JoystickClose(m_joystick);
	}

	SDL_DestroyTexture(m_world);
	SDL_FreeSurface(m_mapChip);
	const int charaNum = static_cast<int>(CHARA_ID::NUM);
	for (int i = 0; i < charaNum; i++) {
		for (int j = 0; j < charaNum; j++) {
			SDL_DestroyTexture(m_characterTexture[i][j]);
		}
		delete[] m_characterTexture[i];
	}
	delete[] m_characterTexture;
}

bool Game::Update(SDL_Renderer *renderer) {
	Draw(renderer);

	EVENT event = GetEvent(m_joystick);
	switch (event) {
	case EVENT::QUIT: {
		m_next = SEQ_ID::QUIT;
		return false;
		break;
	}

	}
	return true;
}

void Game::Draw(SDL_Renderer *renderer) {
	SDL_Rect src = { 0, 0, WINDOW_WIDTH * MAP_CHIPSIZE, WINDOW_HEIGHT * MAP_CHIPSIZE};
	SDL_RenderCopy(renderer, m_world, &src, NULL);
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
			if (SDL_JoystickGetButton(joystick, 2)) {//〇ボタン
				return EVENT::JOY_CIRCLE;
			}
			else if (SDL_JoystickGetButton(joystick, 1)) {//×ボタン
				return EVENT::JOY_CROSS;
			}
			else if (SDL_JoystickGetButton(joystick, 4)) {//L1
				return EVENT::JOY_L1_DOWN;
			}
			else if (SDL_JoystickGetButton(joystick, 5)) {//R1
				return EVENT::JOY_R1;
			}
			break;
		case SDL_JOYBUTTONUP:
			if (!SDL_JoystickGetButton(joystick, 4)) {//L1
				return EVENT::JOY_L1_UP;
			}
			break;
		case SDL_JOYHATMOTION://十字キーが押されたとき
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
		case SDL_QUIT://ウィンドウの×ボタン
			return EVENT::QUIT;
			break;
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