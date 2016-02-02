/*
 * config.cpp
 *
 * Created on: 2013/8/2
 * Modified on: 2015/10/10
 * Author: sjean
 */

#include "config.h"

using namespace std;

namespace CJCU
{
int adj_dir[4] = {BoardSize , -BoardSize , -1 , 1}; //上下左右
int all_dir[8] = {-BoardSize-1 , -BoardSize  , -BoardSize+1  , -1 , 1 , BoardSize-1 , BoardSize , BoardSize+1};
int extend_dir[16] = {-2*BoardSize-2 , -2*BoardSize-1 , -2*BoardSize, -2*BoardSize+1, -2*BoardSize+2,
		-BoardSize-2  , -BoardSize+2  , -2 , 2 , BoardSize-2 , BoardSize+2, 2*BoardSize-2, 2*BoardSize-1,
		2*BoardSize, 2*BoardSize+1, 2*BoardSize+2};

int MaxSimulation = 200; // UCT模擬盤數限制
int Simulation_Limit = 200;

int Pattern_Urgency = 110; // 棋形
int Capture_Urgency = 115; // 攻擊
int Extension_Urgency = 105; // 逃跑
int Distance_Urgency = 100; // 當下落子附近的緊急值

float UCT_Komi = 7.5; // 貼目數
int ExEValue  = 7; // UCB公式的參數

bool Patterns_3[PATTERN_LIMIT] = {0};
bool Patterns_5b[4][PATTERN_LIMIT] = {{0}};
bool Patterns_5w[4][PATTERN_LIMIT] = {{0}};

map<string,string> Black_Book;
map<string,string> White_Book;

map<string,string> Black_Joseki;
map<string,string> White_Joseki;

bool hit_book = true;
bool hit_joseki = true;

int Main_Time = 1800; // 30 min
int BY_Time = 0;
int BY_Stones = 0;

int LeftTime = 1800;
int TotalTime=0;
int PastTime=0;

int Open_Step_Limit = 60; // 開局手數
int MiddleGame_Step_Limit = 60; // 預設的中盤手數
int EndGame_Step_Limit = 100; // 預設的終盤手數

int Mode = 1; // 1:單機  2:分散式

} // namespace CJCU
