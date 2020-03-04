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
	SDL_RenderPresent(m_renderer);//�`��𔽉f

	//�J�ڔ���
	switch (m_next) {
	case SEQ_ID::GAME:
		delete m_sequence;
		Loading();
		m_sequence = new Game();
		break;
	case SEQ_ID::QUIT: //�I��
		return false;
		break;
	default:
		break;
	}
	m_next = SEQ_ID::NONE;

	return true;
}

/*
* �V�[�P���X�J�ڂ��s��
* SEQ_ID next:���̃V�[�P���X
* �Ԓl:�Ȃ�
*/
void MainLoop::MoveTo(SEQ_ID next) {
	m_next = next;;
}

/*
* ���[�h��ʂ̕\��
* �Ԓl:�Ȃ�
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


//������
void Init() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		OutputError("unable to initialize SDL");
	}

	if (TTF_Init() < 0) {
		cout << TTF_GetError() << endl; 
		exit(1);
	}

	//jpg��png�摜��ǂݍ��߂�悤�ɂ���
	int flags = IMG_INIT_PNG;
	int initted = IMG_Init(flags);
	if ((flags & initted) != flags) {
		cout << "could not init IMG:" << IMG_GetError() << endl;
		exit(1);
	}
}

/*
* �G���[�o��
* const char* msg:�o�͂���G���[��
* �Ԓl:�Ȃ�
*/
inline void OutputError(const char* msg) {
	cout << msg << ":" << SDL_GetError() << endl;  exit(1);
}

/*
* �F���ɑΉ�����RGB�l��Ԃ�
* const char* color:�`��F��
* �Ԓl:�`��F�̃p�����[�^
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
* �����\���p�̃e�L�X�`�����쐬
* SDL_Renderer *renderer�F�\����̃����_��
* const char *text�F������
* int size�F�����T�C�Y
* const char *color�F�����F
* �Ԓl�F�쐬�����e�N�X�`��
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
* �t�@�C���̓ǂݍ���
* const char* fileName�F�ǂݍ��ރt�@�C����
* �Ԓl�F�ǂݍ��񂾃f�[�^
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
* str��mark�ŕ������A���l�ɕϊ��������̂�Ԃ�
* string str�F������
* const char mark�F��؂蕶��
* �Ԓl�F�ϊ���̐����z��
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
* ��؂蕶���̂���f�[�^�̓ǂݍ���
* const char* fileName�F�t�@�C����
* const char mark�F��؂蕶��
* �Ԓl�F�ǂݍ��񂾃}�b�v�f�[�^
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