/*
 * config.h
 *
 * Created on: 2013/8/2
 * Author: chouhh
 */

#ifndef PARALLELWINGO_CONFIG_H_
#define PARALLELWINGO_CONFIG_H_

#include <string>
#include <map>
#include <set>

using namespace std;

namespace CJCU
{
#define M_Sequence_Limit 400 // 著手個數限制
#define Same_Pattern_num 16

#define EMPTY  0
#define BLACK  1
#define WHITE  -1
#define EDGE   8

#define RESIGN -1
#define PASS 0

#define RealBoardSize 19                                //棋盤原始大小
#define BoardSize  (RealBoardSize+2)                   //加上邊的棋盤大小
#define BoardArraySize (BoardSize*BoardSize)           //一維的棋盤大小(含邊界)
#define JosekiSize 10

#define PATTERN_LIMIT 65536                            // 最多棋形數
#define MAXSTRNUM 400                                  //最大棋串個數

#define MAX_BUF 1024								   //socket 溝通最大傳輸量

extern int adj_dir[4];
extern int all_dir[8];
extern int extend_dir[16];

extern int MaxSimulation;
extern int Simulation_Limit;

extern int Pattern_Urgency;
extern int Extension_Urgency;
extern int Capture_Urgency;
extern int Distance_Urgency;

extern float UCT_Komi ;
extern int ExEValue ;       //UCB參數

extern bool Patterns_3[PATTERN_LIMIT];
extern bool Patterns_5b[4][PATTERN_LIMIT];
extern bool Patterns_5w[4][PATTERN_LIMIT];

extern  set<string> Pattern5b;
extern  set<string> Pattern5w;

extern map<string,string> Black_Book;
extern map<string,string> White_Book;

extern map<string,string> Black_Joseki;
extern map<string,string> White_Joseki;

extern bool hit_book;
extern bool hit_joseki;

extern int Main_Time;
extern int BY_Time;
extern int BY_Stones;

extern int LeftTime;
extern int TotalTime;
extern int PastTime;

extern void setup();

extern int Open_Step_Limit;
extern int MiddleGame_Step_Limit;
extern int EndGame_Step_Limit;

extern int Mode;
} //namespace CJCU

#endif /* PARALLELWINGO_CONFIG_H_ */
