#pragma once

enum class CHARA_STATE {
	DIR,
	HP,
	POW,
	DEF,
	AGI,
	FRAME_V,
	FRAME_H,
	ACTION,
	TIME
};

class Character {
private:
	bool m_dir;//�����@true:�E false:��
	short m_hp;
	short m_power;//�U����
	short m_defense;//�h���
	short m_agility;//����
	//int m_trunk;//�̊�
	SDL_Rect m_pos;//�ʒu
	short m_vertical;//���݂̃A�j���[�V�����R�}��(�c)
	short m_heng;//���݂̃A�j���[�V�����R�}��(��)
	short m_action;
	unsigned int m_prevTime;//�O��̃A�j���[�V�����X�V����
public:
	Character();
	virtual ~Character();	

	//�w�肳�ꂽ�p�����[�^��Ԃ�
	//request�F�w�肳�ꂽ�p�����[�^��
	//�Ԓl�F�p�����[�^
	const short GetState(CHARA_STATE request);

	//�w�肳�ꂽ�p�����[�^��ݒ肷��
	//request�F�w�肳�ꂽ�p�����[�^��
	//state�F�ύX��̒l
	//�Ԓl�F�Ȃ�
	void SetState(CHARA_STATE request, int state);

	//���W��ύX����
	//nextPos�F�ύX��̍��W
	void SetPos(SDL_Rect nextPos);

	//���݂̍��W��Ԃ�
	SDL_Rect GetPos();

protected:
	//�p�����[�^�̏�����
	void InitState(bool dir, short hp, short power, short defense, short agility, SDL_Rect pos, short action, unsigned int time);
};

class Player : public Character {
private:
	//short m_jumpMovement;//�W�����v����x�����ړ���
	//short m_yPrev;//�O��y���W
public:
	Player(short hp, short power, short defense, short agility, SDL_Rect pos, short action, unsigned int time);
	//~Player();

	const short GetState(CHARA_STATE request);
};

class Enemy : public Character {
private:

public:
	Enemy(short hp, short power, short defense, short agility, SDL_Rect pos, short action, unsigned int time);
	//~Enemy();

	const short GetState(CHARA_STATE request);
};