// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: プログラムに必要な追加ヘッダーをここで参照してください
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#ifdef NDEBUG
#define ASSERT(x)
#else
#define ASSERT(x) {\
	if(!(x)) {\
		exit();\
	}\
}
#endif