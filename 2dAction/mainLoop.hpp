#pragma once
#include <vector>
#include <SDL.h>

//#include "joystick.hpp"
//#include "font.hpp"

//const int g_winPosX = 100;
//const int g_winPosY = 100;

class Sequence;
inline void OutputError(const char *msg);
SDL_Color ConvertToRGB(const char *color);
SDL_Texture *CreateText(SDL_Renderer *renderer, const char *text, int size, const char *color);

class MainLoop {
private:
	Sequence *m_sequence;//�V�[�P���X�J�ڗp
	SDL_Texture *m_loadingText;//���[�h��ʗp����
	//std::vector<std::vector<std::vector<int>>> m_maxFrame;//�e�A�j���[�V�����̃t���[����
	//SDL_Rect m_collisionRect[g_charaNum];//�����蔻��p��`�̍��W
	//std::vector<std::vector<std::vector<int>>> m_attackRect;//�U������p��`���W
	//std::vector<std::vector<std::vector<int>>> m_attackFrame;//�U������̂���t���[��

public:
	SDL_Window *m_window;
	SDL_Renderer *m_renderer;
	//SDL_Texture*** m_characterTexture;
	//SDL_Texture* m_backgroundTexture;
	//Joystick m_joyEvent; //�W���C�X�e�b�N�̃C�x���g�����p

	//�e�V�[�P���X�̒l
	enum class SEQ_ID {
		//TITLE,           //�^�C�g��
		//MODE_SELECT,     //���[�h�I��
		GAME,            //�Q�[�����

		NONE, //�����l
		QUIT  //�I��
	};

	MainLoop();
	~MainLoop();
	bool Update();//���[�v��񕪂̏���
	void MoveTo(SEQ_ID next); //�J�ڐ��������
	void Loading();

private:
	SEQ_ID m_next;//���̃V�[�P���X
};