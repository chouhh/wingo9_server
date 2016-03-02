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
int adj_dir[4] = {BoardSize , -BoardSize , -1 , 1}; //�W�U���k
int all_dir[8] = {-BoardSize-1 , -BoardSize  , -BoardSize+1  , -1 , 1 , BoardSize-1 , BoardSize , BoardSize+1};
int extend_dir[16] = {-2*BoardSize-2 , -2*BoardSize-1 , -2*BoardSize, -2*BoardSize+1, -2*BoardSize+2,
		-BoardSize-2  , -BoardSize+2  , -2 , 2 , BoardSize-2 , BoardSize+2, 2*BoardSize-2, 2*BoardSize-1,
		2*BoardSize, 2*BoardSize+1, 2*BoardSize+2};

int MaxSimulation = 50000; // UCT�����L�ƭ���
int Simulation_Limit = 50000;

int Pattern_Urgency = 110; // �ѧ�
int Capture_Urgency = 115; // ����
int Extension_Urgency = 105; // �k�]
int Distance_Urgency = 20; // ��U���l���񪺺���

float UCT_Komi = 7.5; // �K�ؼ�
int ExEValue  = 7; // UCB�������Ѽ�

bool Patterns_3[PATTERN_LIMIT] = {0};

set<string> Pattern5b;
set<string> Pattern5w;

map<string,string> Black_Book;
map<string,string> White_Book;

bool hit_book = true;

int Main_Time = 1800; // 30 min
int BY_Time = 0;
int BY_Stones = 0;

int LeftTime = 1800;
int TotalTime=0;
int PastTime=0;

int Open_Step_Limit = 60; // �}�����
int MiddleGame_Step_Limit = 60; // �w�]�����L���
int EndGame_Step_Limit = 100; // �w�]���׽L���

int Mode = 1; // 1:���  2:������

} // namespace CJCU
