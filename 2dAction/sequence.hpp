#pragma once

using namespace std;

//�e�V�[�P���X�̒l
enum class SEQ_ID {
	//TITLE,           //�^�C�g��
	//MODE_SELECT,     //���[�h�I��
	GAME,            //�Q�[�����

	NONE, //�����l
	QUIT  //�I��
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

//�V�[�P���X�J�ڂ̊��N���X
class Sequence {
protected:
	SEQ_ID m_next;//���̃V�[�P���X
public:
	Sequence();
	virtual ~Sequence() {};
	virtual bool Update(SDL_Renderer *renderer) = 0;

	//�V�[�P���X�J�ڂ��s��
	//�Ԓl:���̃V�[�P���X
	SEQ_ID MoveTo();
};

class Game : public Sequence {
private:
	SDL_Joystick *m_joystick;
	SDL_Texture *m_world;
	SDL_Surface *m_mapChip;
	vector<vector<int>> m_mapData;
	SDL_Texture ***m_characterTexture;

	vector<vector<ENEMY_ACTION>> m_enemyAction;//�e�G�����A�N�V����
	vector<vector<vector<int>>> m_maxFrame;//�e�A�j���[�V�����̃t���[����
	vector<vector<unsigned short>> m_attackActive;//�U������̂���t���[��
	vector<vector<vector<vector<int>>>> m_attackRect;//�U������͈�
	vector<vector<unsigned short>> m_hurtActive;//���炢����̂���t���[��
	vector<vector<vector<vector<int>>>> m_hurtRect;//���炢����͈�
public :
	Game(SDL_Renderer *renderer, vector<vector<ENEMY_ACTION>> enemyAction, vector<vector<vector<int>>> maxFrame, vector<vector<unsigned short>> attackActive, vector<vector<vector<vector<int>>>> attackRect, vector<vector<unsigned short>> hurtActive, vector<vector<vector<vector<int>>>> hurtRect);
	~Game();

	bool Update(SDL_Renderer *renderer);
	void Draw(SDL_Renderer *renderer);
};

enum class EVENT {
	//JOY_TRIANGLE,
	JOY_CIRCLE,
	JOY_CROSS,
	//JOY_SQUARE,
	JOY_R1,
	JOY_L1_UP,
	JOY_L1_DOWN,
	//JOY_HAT_UP,
	//JOY_HAT_DOWN,
	JOY_HAT_RIGHT,
	//JOY_HAT_RIGHTUP,
	//JOY_HAT_RIGHTDOWN,
	JOY_HAT_LEFT,
	//JOY_HAT_LEFTUP,
	//JOY_HAT_LEFTDOWN,
	JOY_HAT_CENTERED,
	QUIT,
	NONE
};

//�C�x���g�擾
//SDL_Joystick *joystick�F�W���C�X�e�B�b�N�̍\����
//�Ԓl�F�擾�����C�x���g
EVENT GetEvent(SDL_Joystick *joystick);

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

//�t�@�C���̓ǂݍ���
//string fileName�F�ǂݍ��ރt�@�C����
//�Ԓl�F�ǂݍ��񂾃f�[�^
vector<string> ReadFile(string fileName);

//str��mark�ŕ������A���l�ɕϊ��������̂�Ԃ�
//string str�F������
//const char mark�F��؂蕶��
//�Ԓl�F�ϊ���̐����z��
vector<int> split(string str, const char mark);

//��؂蕶���̂���f�[�^�̓ǂݍ���
//string fileName�F�t�@�C����
//const char mark�F��؂蕶��
//�Ԓl�F�ǂݍ��񂾃}�b�v�f�[�^
vector<vector<int>>ReadFileSplit(string fileName, const char mark);