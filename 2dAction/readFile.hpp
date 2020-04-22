#pragma once
#include <vector>
#include <string>

using namespace std;

//ファイルの読み込み
//string fileName：読み込むファイル名
//返値：読み込んだデータ
vector<string> ReadFile(string fileName);

//strをmarkで分割し、数値に変換したものを返す
//string str：文字列
//const char mark：区切り文字
//返値：変換後の数字配列
vector<int> split(string str, const char mark);

//区切り文字のあるデータの読み込み
//string fileName：ファイル名
//const char mark：区切り文字
//返値：読み込んだマップデータ
vector<vector<int>>ReadFileSplit(string fileName, const char mark);