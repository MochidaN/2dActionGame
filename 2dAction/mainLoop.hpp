#pragma once

using namespace std;
//#include "joystick.hpp"
//#include "font.hpp"

class Sequence;
enum class ENEMY_ACTION;

class MainLoop {
private:
	SDL_Window *m_window;
	SDL_Renderer *m_renderer;
	Sequence *m_sequence;//�V�[�P���X�J�ڗp
	SDL_Texture *m_loadingText;//���[�h��ʗp����
	SDL_Texture ***m_characterTexture;
	vector<vector<ENEMY_ACTION>> m_enemyAction;//�e�G�����A�N�V����
	vector<vector<vector<int>>> m_maxFrame;//�e�A�j���[�V�����̃t���[����
	vector<vector<unsigned short>> m_attackActive;//�U������̂���t���[��
	vector<vector<vector<vector<int>>>> m_attackRect;//�U������͈�
	vector<vector<unsigned short>> m_hurtActive;//���炢����̂���t���[��
	vector<vector<vector<vector<int>>>> m_hurtRect;//���炢����͈�

public:
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

	//���[�v��񕪂̏���
	bool Update();

	//�V�[�P���X�J�ڂ��s��
	//SEQ_ID next:���̃V�[�P���X
	//�Ԓl:�Ȃ�
	void MoveTo(SEQ_ID next);

	//���[�h��ʂ̕\��
	//�Ԓl:�Ȃ�
	void Loading();

private:
	SEQ_ID m_next;//���̃V�[�P���X
};

enum class CHARA_ID {
	GUARD,
	WARP,
	KICK,
	//THROW,
	BOSS,
	PLAYER,
	NUM
};

enum class ENEMY_ACTION {
	STAND,
	WALK,
	HIT,
	DOWN,
	DEAD,
	GUARD,
	PUNCH,
	DIVE,
	JUMP_OUT,
	RAMMING,
	KICK_FRONT,
	KICK_BACK,
	//THROW,
	HARD_PUNCH,
	COMBO_PUNCH,
	NUM
};

enum class PLAYER_ACTION {
	STAND,
	WALK,
	STEP,
	HIT,
	HIT_AIR,
	JUMP,
	FALL,
	FALL_HIT,
	LANDING,
	FAINT,
	FAINTING,
	DOWN,
	DEAD,
	GUARD,
	PARRY,
	SIDE_ATTACK,
	VERTICAL_ATTACK,
	NUM
};

//�G���[�o��
//const char* msg:�o�͂���G���[��
//�Ԓl:�Ȃ�
inline void OutputError(const char *msg);

//�F���ɑΉ�����RGB�l��Ԃ�
//const char* color:�`��F��
//�Ԓl:�`��F�̃p�����[�^
SDL_Color ConvertToRGB(const char *color);

//�����\���p�̃e�L�X�`�����쐬
//SDL_Renderer *renderer�F�\����̃����_��
//SDL_Texture *texture�F�쐬�����e�N�X�`���ۑ��p
//const char *text�F������
//int size�F�����T�C�Y
//const char *color�F�����F
//�Ԓl�F�Ȃ�
void CreateText(SDL_Renderer *renderer, SDL_Texture *texture, const char *text, int size, const char *color);