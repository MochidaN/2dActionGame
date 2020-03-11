#include "stdafx.h"

#include "sequence.hpp"
#include "mainLoop.hpp"
#include "parameter.hpp"

using namespace std;

void Init();

//ファイルの読み込み
//string fileName：読み込むファイル名
//返値：読み込んだデータ
vector<string> ReadFile(string fileName);

//strをmarkで分割し、数値に変換したものを返す
//string str：文字列
//const char mark：区切り文字
//返値：変換後の数字配列
vector<int> split(string str, const char mark);

//区切り文字のあるデータの読み込み
//string fileName：ファイル名
//const char mark：区切り文字
//返値：読み込んだマップデータ
vector<vector<int>>ReadFileSplit(string fileName, const char mark);


MainLoop::MainLoop() {
	Init();

	m_next = SEQ_ID::NONE;

	m_window = SDL_CreateWindow("window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH * MAP_CHIPSIZE, WINDOW_HEIGHT * MAP_CHIPSIZE, SDL_WINDOW_OPENGL);
	if (m_window == NULL) {
		OutputError("failed to create window");
	}
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_SOFTWARE);
	if (m_renderer == NULL) {
		OutputError("failed to create renderer");
	}
	//GUARD,WARP,KICK
	vector<vector<ENEMY_ACTION>> action{ {ENEMY_ACTION::WALK, ENEMY_ACTION::HIT, ENEMY_ACTION::DOWN, ENEMY_ACTION::DEAD, ENEMY_ACTION::GUARD, ENEMY_ACTION::PUNCH}, {ENEMY_ACTION::STAND, ENEMY_ACTION::HIT, ENEMY_ACTION::DOWN, ENEMY_ACTION::DEAD, ENEMY_ACTION::DIVE, ENEMY_ACTION::JUMP_OUT}, { ENEMY_ACTION::STAND, ENEMY_ACTION::HIT, ENEMY_ACTION::DOWN, ENEMY_ACTION::DEAD, ENEMY_ACTION::RAMMING, ENEMY_ACTION::KICK_FRONT, ENEMY_ACTION::KICK_BACK} };
	const short actionSize = action.size();
	m_enemyAction.resize(actionSize);
	for (int i = 0; i < actionSize; i++) {
		m_enemyAction[i] = action[i];
	}

#ifdef _DEBUG
	vector<string> filePath{"../txt/guard/", "../txt/warp/", "../txt/kick/", "../txt/boss/", "../txt/player/"};
	string enemyHurtRect("../txt/enemy_hurtRect/");
	string enemyAttackRect("../txt/enemy_attackRect/");
	string imgPathFile("imgPath_debug.txt");
	string hurtRectFile("hurtRectPath_debug.txt");
	string attackRectFile("attackRectPath_debug.txt");
#endif // _DEBUG
#ifdef NDEBUG
	vector<string> filePath{ "../../txt/guard/", "../../txt/warp/", "../../txt/kick/", "../../txt/boss/", "../../txt/player/" };
	string enemyHurtRect("../../txt/enemy_hurtRect/");
	string enemyAttackRect("../../txt/enemy_attackRect/");
	string imgPathFile("imgPath.txt");
	string hurtRectFile("hurtRectPath.txt");
	string attackRectFile("attackRectPath.txt");
#endif // NDEBUG

	string animFrameFile("animationFrame.txt");
	string attackActiveFile("attackActive.txt");
	string hurtActiveFile("hurtActive.txt");

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
			if ((m_characterTexture[i][j] = SDL_CreateTextureFromSurface(m_renderer, img)) == NULL) {
				OutputError("failed to create character texture");
			}
			SDL_FreeSurface(img);
		}
	}

	for (auto fp:filePath) {
		m_maxFrame.push_back(ReadFileSplit(fp + animFrameFile, ' '));
	}

	auto paramToBit = [](vector<vector<int>> num) {
		vector<unsigned short> flag;
		for (auto i : num) {
			if (i[0] < 0) {
				flag.push_back(0);
				break;
			}
			else {
				unsigned short bit = 0;
				for (auto j : i) {
					bit |= (1 << j);
				}
				flag.push_back(bit);
			}
		}
		return flag;
	};
	m_attackActive.push_back(paramToBit(ReadFileSplit(enemyAttackRect + attackActiveFile, ' ')));
	m_attackActive.push_back(paramToBit(ReadFileSplit(filePath[static_cast<int>(CHARA_ID::PLAYER)] + attackActiveFile, ' ')));
	m_hurtActive.push_back(paramToBit(ReadFileSplit(enemyHurtRect + hurtActiveFile, ' ')));
	m_hurtActive.push_back( paramToBit(ReadFileSplit(filePath[static_cast<int>(CHARA_ID::PLAYER)] + hurtActiveFile, ' ')) );

	auto setRect = [](vector<unsigned short> active, vector<string> rectPath) {
		vector<vector<vector<int>>> rect;
		for (int i = 0; i < active.size(); i++) {
			if (active[i] == 0) {
				rect.resize(rect.size() + 1);
				rect[i].resize(rect.size() + 1);
				rect[i][0].push_back(-1);
			}
			else {
				rect.push_back(ReadFileSplit(rectPath[i], ' '));
			}
		}
		return rect;
	};
	vector<string> hurtPathHead{ enemyHurtRect, filePath[static_cast<int>(CHARA_ID::PLAYER)] + "hurtRect/" };
	vector<string> attackPathHead{ enemyAttackRect, filePath[static_cast<int>(CHARA_ID::PLAYER)] + "attackRect/" };
	for (int i = 0; i < hurtPathHead.size(); i++) {
		vector<string> hurtRectPath( ReadFile(hurtPathHead[i] + hurtRectFile) );
		m_hurtRect.push_back(setRect(m_hurtActive[i], hurtRectPath));
		vector<string> attackRectPath(ReadFile(attackPathHead[i] + attackRectFile));
		m_attackRect.push_back(setRect(m_attackActive[i], attackRectPath));
	}

	CreateText(m_renderer, m_loadingText, u8"Loading...", 100, "white");
	m_sequence = new Game();
}

MainLoop::~MainLoop() {
	delete m_sequence;
	const int charaNum = static_cast<int>(CHARA_ID::NUM);
	for (int i = 0; i < charaNum; i++) {
		for (int j = 0; j < charaNum; j++) {
			SDL_DestroyTexture(m_characterTexture[i][j]);
		}
		delete[] m_characterTexture[i];
	}
	delete[] m_characterTexture;

	SDL_DestroyTexture(m_loadingText);
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	TTF_Quit();
	SDL_Quit();
}

bool MainLoop::Update() {
	m_sequence->Update(m_renderer);
	SDL_RenderPresent(m_renderer);//描画を反映

	//遷移判定
	switch (m_next) {
	case SEQ_ID::GAME:
		delete m_sequence;
		Loading();
		m_sequence = new Game();
		break;
	case SEQ_ID::QUIT: //終了
		return false;
		break;
	default:
		break;
	}
	m_next = SEQ_ID::NONE;

	return true;
}

void MainLoop::MoveTo(SEQ_ID next) {
	m_next = next;;
}

void MainLoop::Loading() {
	SDL_Color c = ConvertToRGB("black");
	SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a);
	SDL_RenderFillRect(m_renderer, NULL);

	int w, h;
	SDL_QueryTexture(m_loadingText, NULL, NULL, &w, &h);
	SDL_Rect dst_rect = { WINDOW_WIDTH - w, WINDOW_HEIGHT - h, w, h };
	SDL_RenderCopy(m_renderer, m_loadingText, NULL, &dst_rect);

	SDL_RenderPresent(m_renderer);
	SDL_Delay(1000);
}

void Init() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		OutputError("unable to initialize SDL");
	}

	if (TTF_Init() < 0) {
		cout << TTF_GetError() << endl; 
		exit(1);
	}

	//jpgとpng画像を読み込めるようにする
	int flags = IMG_INIT_PNG;
	int initted = IMG_Init(flags);
	if ((flags & initted) != flags) {
		cout << "could not init IMG:" << IMG_GetError() << endl;
		exit(1);
	}
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