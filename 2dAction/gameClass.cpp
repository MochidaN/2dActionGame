#include "stdafx.h"
#include "sequence.hpp"
#include "parameter.hpp"

const short ANIME_INTERVAL = 1000 / 30;//�A�j���[�V�����Ԋu

const short IMG_SIZE[static_cast<short>(CHARA_ID::NUM)] = { ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE, PLAYER_SIZE };
const short CHARA_NUM[static_cast<short>(CHARA_ID::NUM)] = { 1, 0, 0, 0, 1 };

//world�̃T�[�t�F�C�X�Ƀ}�b�v��`��
//SDL_Surface *world�F���[���h�T�[�t�F�C�X
// SDL_Surface *mapChip�F�}�b�v�`�b�v�T�[�t�F�C�X
//vector<vector<int>> mapData�F�}�b�v�̔z�u�f�[�^
//�Ԓl�F�Ȃ�
void DrawWorld(SDL_Renderer *renderer, SDL_Surface *world, SDL_Surface *mapChip, vector<vector<int>> mapData);

//���̃A�j���[�V�����t���[�����v�Z
//hengFrame�F���݂̉��R�}��
//verticalFrame�F���݂̏c�R�}��
//maxFrame�F�ő�R�}��(0:�� 1:�c)
//���̃t���[����
vector<short> RetrunNextFrame(short hengFrame, short verticalFrame, vector<int> maxFrame);

//�A�N�V��������z��ԍ��ɕϊ�
//actionList�F�A�N�V�����ꗗ
//nowAction�F���݂̃A�N�V����
template <typename T = ENEMY_ACTION> const int ActionToIndex(vector<T> actionList, int nowAction);

Sequence::Sequence() {
	m_next = SEQ_ID::NONE;
}

SEQ_ID Sequence::MoveTo() {
	return m_next;
}

Game::Game(SDL_Renderer *renderer, vector<vector<ENEMY_ACTION>> enemyAction, vector<vector<vector<int>>> maxFrame, vector<vector<unsigned short>> attackActive, vector<vector<vector<vector<int>>>> attackRect, vector<vector<unsigned short>> hurtActive, vector<vector<vector<vector<int>>>> hurtRect) {
	m_joystick = SDL_JoystickOpen(0);//�W���C�X�e�B�b�N���\���̂Ɋ��蓖�ĂėL����
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
			m_chara[id][cn] = new Enemy(100, 10, 10, 10, pos, action[id], time);
		}
	}
	const short playerID = static_cast<short>(CHARA_ID::PLAYER);
	SDL_Rect pos = { 0, 0, IMG_SIZE[playerID], IMG_SIZE[playerID]};
	m_chara[playerID] = new Character*[CHARA_NUM[playerID]];
	m_chara[playerID][0] = new Player(100, 10, 10, 10, pos, static_cast<short>(PLAYER_ACTION::STAND), time);
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

	//delete m_player;
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

	unsigned int nowTime = SDL_GetTicks();
	const short charaNum = static_cast<short>(CHARA_ID::NUM);
	for (int id = 0; id < charaNum; id++) {
		for (int cn = 0; cn < CHARA_NUM[id]; cn++) {
			short action = m_chara[id][cn]->GetState(CHARA_STATE::ACTION);
			if (id < static_cast<short>(CHARA_ID::BOSS)) {
				action = ActionToIndex(m_enemyAction[id], action);
			}
			vector<short> frame = { m_chara[id][cn]->GetState(CHARA_STATE::FRAME_H), m_chara[id][cn]->GetState(CHARA_STATE::FRAME_V) };
			if (nowTime - m_chara[id][cn]->GetState(CHARA_STATE::TIME) >= ANIME_INTERVAL) {
				//�A�j���[�V�����X�V
				frame = RetrunNextFrame(frame[0], frame[1], m_maxFrame[id][action]);
				m_chara[id][cn]->SetState(CHARA_STATE::FRAME_H, frame[0]);
				m_chara[id][cn]->SetState(CHARA_STATE::FRAME_V, frame[1]);
				m_chara[id][cn]->SetState(CHARA_STATE::TIME, nowTime);
			}

			SDL_Rect srcChara = { frame[0] * IMG_SIZE[id], frame[1] * IMG_SIZE[id], IMG_SIZE[id], IMG_SIZE[id] };
			if ( m_chara[id][cn]->GetState(CHARA_STATE::DIR) == true) {//�E����
				SDL_RenderCopy(renderer, m_characterTexture[id][action], &srcChara, &m_chara[id][cn]->GetPos());
			}
			else {//��
				SDL_RenderCopyEx(renderer, m_characterTexture[id][action], &srcChara, &m_chara[id][cn]->GetPos(), 180, NULL, SDL_FLIP_VERTICAL);
			}
		}
	}
}

EVENT GetEvent(SDL_Joystick *joystick) {
	if (!SDL_JoystickGetAttached(joystick)) {//�܂��J���ĂȂ��Ƃ�
		joystick = SDL_JoystickOpen(0);
		if (SDL_JoystickGetAttached(joystick)) {
			cout << "joystick opened" << endl;
		}
	}

	SDL_Event event;
	if (SDL_PollEvent(&event)) {//�L���[�ɃC�x���g���������Ƃ�
		switch (event.type) {
		case SDL_JOYBUTTONDOWN:
			if (SDL_JoystickGetButton(joystick, 2)) {//�Z�{�^��
				return EVENT::JOY_CIRCLE;
			}
			else if (SDL_JoystickGetButton(joystick, 1)) {//�~�{�^��
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
		case SDL_JOYHATMOTION://�\���L�[�������ꂽ�Ƃ�
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
		case SDL_QUIT://�E�B���h�E�́~�{�^��
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

vector<short> RetrunNextFrame(short hengFrame, short verticalFrame, vector<int> maxFrame) {
	if (++hengFrame >= maxFrame[0]) {
		hengFrame = 0;
		if (++verticalFrame >= maxFrame[1]) {
			verticalFrame = 0;
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