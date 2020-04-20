#pragma once
#include <SDL.h>
#include <vector>

using namespace std;

const short g_right = 1;
const short g_left = -1;
const short g_addX = 4;

enum class CHARA_ID {
	GUARD,
	WARP,
	KICK,
	BOSS,
	PLAYER,
	NUM
};

enum class CHARA_STATE {
	DIR,
	HP,
	POW,
	DEF,
	FRAME_V,
	FRAME_H,
	ACTION,
	X_ADD,
	Y_ADD,
	HURT_ACTIVE,
	ATTACK_ACTIVE,
	TRUNK
};

namespace ENEMY {
	enum class ACTION {
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
		HARD_PUNCH,
		COMBO_PUNCH,
		NUM
	};
};

namespace PLAYER {
	enum class ACTION {
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
}

class Character {
protected:
	//�p�����[�^�̏�����
	void InitState(short dir, short hp, short power, short defense, SDL_Rect pos, short action, unsigned int time, short yAdd);
private:
	short m_dir;//����
	short m_hp;
	short m_power;//�U����
	short m_defense;//�h���
	SDL_Rect m_pos;//�ʒu(�L�����̔���ł͂Ȃ��摜�S��) 
	short m_vertical;//���݂̃A�j���[�V�����R�}��(�c)
	short m_heng;//���݂̃A�j���[�V�����R�}��(��)
	short m_action;
	short m_xAdd;//x�����ړ���
	short m_yAdd;//y�����ړ���
	unsigned int m_prevTime;//�O��̃A�j���[�V�����X�V����
	unsigned int m_hurtActive;//���炢����̂���t���[��
	unsigned int m_attackActive;//�U������̂���t���[��
public:
	Character();
	virtual ~Character();	

	virtual void Update();

	//�A�j���[�V�����t���[���X�V
	bool UpdateAnimation(unsigned int nowTime, std::vector<int> maxFrame);

	//���W�ړ�����
	void MovePos(vector<int> myHurtRect, vector<vector<int>> mapData);

	//�L�������m���Փ˂����ۂ̏���
	void CollisionChara(vector<int> myHurtRect, vector<int> oppHurtRect, vector<vector<int>> mapData, Character &opponent);

	//�w�肳�ꂽ�p�����[�^��Ԃ�
	//request�F�w�肳�ꂽ�p�����[�^��
	//�Ԓl�F�p�����[�^
	virtual const short GetState(CHARA_STATE request);

	//�w�肳�ꂽ�p�����[�^��ݒ肷��
	//request�F�w�肳�ꂽ�p�����[�^��
	//state�F�ύX��̒l
	virtual void SetState(CHARA_STATE request, short state);

	//���݂̍��W��Ԃ�
	const SDL_Rect GetPos();

	//���W��ύX����
	//nextPos�F�ύX��̍��W
	void SetPos(SDL_Rect nextPos);
	
	//�O��̃A�j���[�V�����X�V�^�C�~���O�擾
	const unsigned int GetTime();

	//�Ō�̃A�j���[�V�����X�V�^�C�~���O�ۑ�
	void SetTime(unsigned int time);

	//frame�R�}�ڂ̂��炢���肪�L�����Ԃ�
	//request�FHURT_ACTIVE or ATTACK_ACTIVE
	//frame�F�m�F�������R�}��
	//�Ԓl 0�F�L���łȂ��ꍇ 1�F����ȊO
	const unsigned int GetActiveBit(CHARA_STATE request, int frame);

	//�����̃r�b�g����Z�b�g����
	//request�FHURT_ACTIVE or ATTACK_ACTIVE
	//bit�F�Z�b�g����r�b�g��
	void SetActiveBit(CHARA_STATE request, unsigned int bit);
};


//2�_�Ԃ̋�����2���Ԃ�
float CalculateSquaredDistance(SDL_Rect r0, SDL_Rect r1);

//���̃A�j���[�V�����t���[�����v�Z
//hengFrame�F���݂̉��R�}��
//verticalFrame�F���݂̏c�R�}��
//maxFrame�F�ő�R�}��(0:�� 1:�c)
//endAnimation�F�A�j���[�V����������������ǂ���
//�Ԓl�F���̃t���[����
std::vector<short> ReturnNextFrame(short hengFrame, short verticalFrame, std::vector<int> maxFrame, bool &endAnimation);

//���̃R�}���ʂ��ŉ��R�}�ڂ��Ԃ�
//maxFrameHeng�F���̃R�}���ő傢����
//chara�F�v�Z�Ώۂ̃L�����N�^�\���l
//�Ԓl�F���݂̃R�}��
int ReturnFrameNum(int maxFrameHeng, Character chara);

//�L���������蔻��̍��W�𔽓]
//rect�F�E�����̍��W
//imgWidth�F�摜�̕�
inline vector<int> FlipRect(vector<int> rect, int imgWidth);

//x���W�̈ړ�
//nowPos�F���݂̍��W(�摜�S��)
//hurtRext�F�L�����̓����蔻��
//distance�F�ړ�����
//worldWidth�F���[���h�̒[���W
//mapData�F�}�b�v�ԍ��f�[�^
//�Ԓl�F�ړ���̍��W
SDL_Rect MovePositionX(SDL_Rect nowPos, vector<int> hurtRect, int distance, int worldWidth, vector<vector<int>> mapData);

//y���W�̈ړ�
//nowPos�F���݂̍��W(�摜�S��)
//hurtRext�F�L�����̓����蔻��
//yAdd�Fy�����̈ړ���
//worldHeight�F���[���h�̒[���W
//mapData�F�}�b�v�ԍ��f�[�^
//�Ԓl�F�ړ���̍��W
SDL_Rect MovePositionY(SDL_Rect nowPos, vector<int> hurtRect, short &yAdd, int worldHeight, vector<vector<int>> mapData);

//�L���������蔻��̍��W�𔽓]
//rect�F�E�����̍��W
//imgWidth�F�摜�̕�
//vector<int> FlipRect(vector<int> rect, int imgWidth);

//�ړ����̃}�b�v�Ƃ̏Փ˔���
//nowPos�F���݂̍��W(�摜�S��)
//prevX�F�ړ��O��x���W
//hurtRext�F�L�����̓����蔻��
//mapData�F�}�b�v�ԍ��f�[�^
void CollisionMapX(SDL_Rect &nowPos, int prevX, vector<int> hurtRect, vector<vector<int>> mapData);

//�ړ����̃}�b�v�Ƃ̏Փ˔���
//nowPos�F���݂̍��W(�摜�S��)
//prevY�F�ړ��O��y���W
//hurtRext�F�L�����̓����蔻��
//mapData�F�}�b�v�ԍ��f�[�^
//�Ԓl�F�n��ɂ��邩�ǂ���
bool CollisionMapY(SDL_Rect &nowPos, int prevY, vector<int> hurtRect, vector<vector<int>> mapData);

//�L�����N�^�̔���͈͂��v�Z
//pos�F�L�����摜�̍�����W
//rect�F�L������`�̑��΍��W
//�Ԓl�F�v�Z��̍��W
SDL_Rect ReturnCharaRect(SDL_Rect pos, vector<int> rect);

//��`�̓����蔻��
//����:���肷��2��`
//�Ԓl true:�Փ˂��Ă��� false:���Ă��Ȃ�
bool DetectCollisionRect(SDL_Rect r0, SDL_Rect r1);

//�A�N�V������ActiveBit�̕ύX������
void SetAction(Character &chara, short nextAction, unsigned int hurtActive, unsigned int atkActive);