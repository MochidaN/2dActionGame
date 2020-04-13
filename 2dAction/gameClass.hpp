#pragma once
#include "sequence.hpp"
#include "parameter.hpp"

//gameClass内でのみ使用する関数のプロタイプ宣言


//worldのサーフェイスにマップを描画
//SDL_Surface *world：ワールドサーフェイス
// SDL_Surface *mapChip：マップチップサーフェイス
//vector<vector<int>> mapData：マップの配置データ
//返値：なし
void DrawWorld(SDL_Renderer *renderer, SDL_Surface *world, SDL_Surface *mapChip, vector<vector<int>> mapData);

//次のアニメーションフレームを計算
//hengFrame：現在の横コマ数
//verticalFrame：現在の縦コマ数
//maxFrame：最大コマ数(0:横 1:縦)
//endAnimation：アニメーションが一周したかどうか
//返値：次のフレーム数
vector<short> RetrunNextFrame(short hengFrame, short verticalFrame, vector<int> maxFrame, bool &endAnimation);

//アクション名を配列番号に変換
//actionList：アクション一覧
//nowAction：現在のアクション
template <typename T = ENEMY_ACTION> const int ActionToIndex(vector<T> actionList, int nowAction);

//x座標の移動
//nowPos：現在の座標(画像全体)
//hurtRext：キャラの当たり判定
//distance：移動距離
//worldWidth：ワールドの端座標
//mapData：マップ番号データ
//返値：移動後の座標
SDL_Rect MovePositionX(SDL_Rect nowPos, vector<int> hurtRect, int distance, int worldWidth, vector<vector<int>> mapData);

//y座標の移動
//nowPos：現在の座標(画像全体)
//hurtRext：キャラの当たり判定
//yAdd：y方向の移動量
//worldHeight：ワールドの端座標
//mapData：マップ番号データ
//返値：移動後の座標
SDL_Rect MovePositionY(SDL_Rect nowPos, vector<int> hurtRect, short &yAdd, int worldHeight, vector<vector<int>> mapData);

//キャラ当たり判定の座標を反転
//rect：右向きの座標
//imgWidth：画像の幅
vector<int> FlipRect(vector<int> rect, int imgWidth);

//移動時の衝突判定
//nowPos：現在の座標(画像全体)
//prevX：移動前のx座標
//hurtRext：キャラの当たり判定
//mapData：マップ番号データ
void CollisionX(SDL_Rect &nowPos, int prevX, vector<int> hurtRect, vector<vector<int>> mapData);

//移動時の衝突判定
//nowPos：現在の座標(画像全体)
//prevY：移動前のy座標
//hurtRext：キャラの当たり判定
//mapData：マップ番号データ
//返値：地上にいるかどうか
bool CollisionY(SDL_Rect &nowPos, int prevY, vector<int> hurtRect, vector<vector<int>> mapData);

//今のコマが通しで何コマ目か返す
//maxFrameHeng：横のコマが最大いくつか
//chara：計算対象のキャラクタ構造値
//返値：現在のコマ数
inline int ReturnFrameNum(int maxFrameHeng, Character chara);