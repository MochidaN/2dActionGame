#include <iostream>
#include <fstream>
#include <sstream>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "sequence.hpp"
#include "mainLoop.hpp"
#include "parameter.hpp"

using namespace std;

void Init();
vector<string> ReadFile(const char* fileName);
vector<int> split(string str, const char mark);
vector<vector<int>>ReadFileSplit(const char* fileName);


MainLoop::MainLoop() {
	Init();

	m_next = SEQ_ID::NONE;

	m_window = SDL_CreateWindow("window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	if (m_window == NULL) {
		OutputError("failed to create window");
	}

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_SOFTWARE);
	if (m_renderer == NULL) {
		OutputError("failed to create renderer");
	}

	m_loadingText = CreateText(m_renderer, u8"Loading...", 100, "white");
	m_sequence = new Game();
}

MainLoop::~MainLoop() {
	delete m_sequence;

	SDL_DestroyTexture(m_loadingText);
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	TTF_Quit();
	SDL_Quit();
}

bool MainLoop::Update() {
	//m_sequence->Update();
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

/*
* シーケンス遷移を行う
* SEQ_ID next:次のシーケンス
* 返値:なし
*/
void MainLoop::MoveTo(SEQ_ID next) {
	m_next = next;;
}

/*
* ロード画面の表示
* 返値:なし
*/
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


//初期化
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

/*
* エラー出力
* const char* msg:出力するエラー文
* 返値:なし
*/
inline void OutputError(const char* msg) {
	cout << msg << ":" << SDL_GetError() << endl;  exit(1);
}

/*
* 色名に対応したRGB値を返す
* const char* color:描画色名
* 返値:描画色のパラメータ
*/
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

/*
* 文字表示用のテキスチャを作成
* SDL_Renderer *renderer：表示先のレンダラ
* const char *text：文字列
* int size：文字サイズ
* const char *color：文字色
* 返値：作成したテクスチャ
*/
SDL_Texture *CreateText(SDL_Renderer *renderer, const char *text, int size, const char *color) {
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
	
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == NULL) {
		OutputError("failed to create texture");
	}
	SDL_FreeSurface(surface);
	TTF_CloseFont(font);

	return texture;
}

/*
* ファイルの読み込み
* const char* fileName：読み込むファイル名
* 返値：読み込んだデータ
*/
vector<string> ReadFile(const char* fileName) {
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

/*
* strをmarkで分割し、数値に変換したものを返す
* string str：文字列
* const char mark：区切り文字
* 返値：変換後の数字配列
*/
vector<int> split(string str, const char mark) {
	stringstream ss{ str };
	string buf;
	vector<int> data;
	while (getline(ss, buf, mark)) {
		data.push_back(atoi(buf.c_str()));
	}

	return data;
}

/*
* 区切り文字のあるデータの読み込み
* const char* fileName：ファイル名
* const char mark：区切り文字
* 返値：読み込んだマップデータ
*/
vector<vector<int>> ReadFileSplit(const char* fileName, const char mark) {
	ifstream ifs(fileName);
	if (!ifs) {
		cout << "failed to open file:" << fileName << endl;
		exit(1);
	}

	string str;
	getline(ifs, str);
	vector<int> bufSize = split(str, mark);//[0]:width, [1]:height

	vector<vector<int>> data(bufSize[0], vector<int>(bufSize[1]));
	for (int w = 0; w < bufSize[0]; w++) {
		getline(ifs, str);
		data[w] = split(str, mark);
	}

	return data;
}