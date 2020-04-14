#pragma once
#include "sequence.hpp"
#include "parameter.hpp"

//gameClass���ł̂ݎg�p����֐��̃v���^�C�v�錾


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
//endAnimation�F�A�j���[�V����������������ǂ���
//�Ԓl�F���̃t���[����
vector<short> RetrunNextFrame(short hengFrame, short verticalFrame, vector<int> maxFrame, bool &endAnimation);

//�A�N�V��������z��ԍ��ɕϊ�
//actionList�F�A�N�V�����ꗗ
//nowAction�F���݂̃A�N�V����
template <typename T = ENEMY_ACTION> const int ActionToIndex(vector<T> actionList, int nowAction);

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
vector<int> FlipRect(vector<int> rect, int imgWidth);

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
//dir�F����
//�Ԓl�F�v�Z��̍��W
SDL_Rect ReturnCharaRect(SDL_Rect pos, vector<int> rect, short dir);

//��`�̓����蔻��
//����:���肷��2��`
//�Ԓl true:�Փ˂��Ă��� false:���Ă��Ȃ�
bool DetectCollisionRect(SDL_Rect r0, SDL_Rect r1);

//���̃R�}���ʂ��ŉ��R�}�ڂ��Ԃ�
//maxFrameHeng�F���̃R�}���ő傢����
//chara�F�v�Z�Ώۂ̃L�����N�^�\���l
//�Ԓl�F���݂̃R�}��
inline int ReturnFrameNum(int maxFrameHeng, Character chara);