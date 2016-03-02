/* ----------------------
 * gtp_cmd.cpp
 *
 * Created on: 2014/10/18
 * Author: chouhh
 * -----------------------
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <stdio.h>
#include <stdlib.h>

#include "gtp_cmd.h"
#include "config.h"
#include "board.h"
#include "pattern.h"
#include "UCT_search.h"
#include "server.h"

#define DECLARE(func) static int func(string sn)
#define GTP_SUCCESS 1
#define GTP_NORESPONSE 2
#define GTP_ENDALL 0
#define GTP_ERROR -1
#define GTP_OK 1

using namespace CJCU;

ofstream fout("commands_log.txt",ios::app);

Board *b = new Board();
Pattern *pattern = new Pattern();
HANDLE thread;
DWORD tid;
bool firstConnent = true;

DECLARE(gtp_list_commands);
DECLARE(gtp_set_boardsize);
DECLARE(gtp_set_komi);
DECLARE(gtp_time_settings);
DECLARE(gtp_name);
DECLARE(gtp_version);
DECLARE(gtp_play_move);
DECLARE(gtp_com_play);
DECLARE(gtp_undo);
DECLARE(gtp_quit);
DECLARE(gtp_show_board);
DECLARE(gtp_clear_board);
DECLARE(gtp_time_left);
DECLARE(gtp_protocol_version);
DECLARE(gtp_finish_game);

static struct gtp_command commands[] =
{
  {"list_commands",    	      gtp_list_commands},
  {"boardsize",               gtp_set_boardsize},
  {"komi",                    gtp_set_komi},
  {"time_settings",           gtp_time_settings},
  {"name",                    gtp_name},
  {"version",                 gtp_version},
  {"play",                    gtp_play_move},
  {"genmove",                 gtp_com_play},
  {"undo",                    gtp_undo},
  {"quit",             	      gtp_quit},
  {"showboard",               gtp_show_board},
  {"clear_board",             gtp_clear_board},
  {"time_left",               gtp_time_left},
  {"protocol_version",        gtp_protocol_version},
  {"final_status_list",       gtp_finish_game},
  {NULL,                      NULL}
};

void start_main_loop()
{
	//create a thread for server
	thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) initServer,NULL, 0, &tid);

	char cmd_line[100] = {0};
	string cmd, arg;

	// 讀進Patterns & Books
	if (pattern->GetPattern())
	{
		fout << "Pattern Success!" << endl;
	}
	else
	{
		fout << "Pattern Fail!" << endl;
	}

	srand ( time(NULL) );

	// 讀進一列命令->拆解命令與參數->呼叫執行命令

	while (cin.getline(cmd_line, 100))
	{

		cmd = cmd_line;
		arg.resize(0);

		if (cmd.find(' ', 1) + 1 != 0)
		{
			arg = cmd.substr(cmd.find(' ', 1) + 1, cmd.length());
			cmd.resize(cmd.find(' ', 1));
		}

		for (int i = 0; commands[i].name != NULL; i++)
		{
			if (cmd == commands[i].name)
			{
				if ((*commands[i].function)(arg) == GTP_ENDALL)
				{
					return;
				}
			}
		}
	}
}

// 轉換座標，從二維座標轉換成一維座標:
// col: A->1 B->2...H->8 J->9 K->10 L->11...S->18 T->19;
// row: 1 2 3...9 10 11 12 13...19;
// A1在左下角 : 19 * 21 + 1 = 400
int GetCoordinate(string str)
{
	if (str.length() == 2)
	{
		LowerCase(str);

		if (str[0] < 'a' || str[0] > 't')
		{
			return -1;
		}
		if (str[0] == 'i')
		{
			return -1;
		}
		if (str[1] < '1' || str[1] > '9')
		{
			return -1;
		}

		int x = str[0] - 'a' + 1;

		if (str[0] > 'i')
		{
			x--;
		}

		int y = RealBoardSize - (str[1] - '1' + 1) + 1;

		return y * BoardSize + x;
	}
	else if (str.length() == 3)
	{
		LowerCase(str);

		if (str[0] < 'a' || str[0] > 't')
		{
			return -1;
		}
		if (str[1] != '1')
		{
			return -1;
		}
		if (str[0] == 'i')
		{
			return -1;
		}
		if (str[2] < '0' || str[2] > '9')
		{
			return -1;
		}

		int x = str[0] - 'a' + 1;

		if (str[0] > 'i')
		{
			x--;
		}

		stringstream ss2;
		string s2;
		ss2 << str[2];
		ss2 >> s2;

		string s = "1" + s2;

		int y = RealBoardSize - StrToInt(s) + 1;

		return y * BoardSize + x;
	}

	return -1;
}

// 英文字串轉換成全部小寫
void LowerCase(string& str)
{
	for (unsigned int i = 0; i < str.size(); i++)
	{
		if (str[i] >= 'A' && str[i] <= 'Z')
		{
			str[i] = str[i] + ('a' - 'A');
		}
	}
}

// 數字字串轉換成整數
int StrToInt(string& str)
{
	int result = 0;

	for (unsigned int i=0; i<str.size(); i++)
	{
		if (str[i] >= '0' && str[i] <= '9')
		{
			str[i] = str[i] - '0';
			result = result + str[i] * pow(10.0, str.size()-i-1.0);
		}
	}

	return result;
}

// 整數轉換成字串
void IntToStr(int num, string& str)
{
	char buffer[50];

	sprintf(buffer, "%d", num);
	str = buffer;
}

// 透過標準I/O channel回應訊息給GTP engine
int GTP_Response(string s, int flag)
{
	switch (flag)
	{
	case GTP_SUCCESS:

		if (s.length() != 0)
		{
			cout << "= " << s << endl;
		}
		else
		{
			cout << "=" << endl;
		}
		break;
	case GTP_ERROR:

		cout << "? " << s << endl;
		break;
	}
	cout << endl;
	return GTP_OK;
}

int find_nearest_joseki(int num_joseki, int *joseki_moves, int lastmove)
{
	int min_move = 1000, min_distance = 1000;
	int lastmove_x = lastmove % BoardSize;
	int lastmove_y = lastmove / BoardSize;

	for (int i=0; i<num_joseki; i++)
	{
		int x = joseki_moves[i] % BoardSize;
		int y = joseki_moves[i] / BoardSize;
		int distance = abs(x-lastmove_x) + abs(y-lastmove_y);

		if (distance < min_distance)
		{
			min_distance = distance;
			min_move = joseki_moves[i];
		}
	}

	return min_move;
}

//------------------------------------------------------------------------------
/*
 * 以下為GTP engine的命令
 */

// 列出所有命令
int gtp_list_commands(string sn)
{
	cout << "= ";
	for (int i=0; commands[i].name != NULL; i++)
	{
		cout << commands[i].name << endl;
	}

	cout << endl;

	return GTP_NORESPONSE;
}

// 設定棋盤大小
int gtp_set_boardsize(string sn)
{
	if (sn.length() != 0)
	{
		int k = 0;
		if (sscanf(sn.c_str(), "%d", &k) >= 1)
		{
			return GTP_Response("", GTP_SUCCESS);
		}
	}

	return GTP_Response("", GTP_ERROR);
}

// 設定貼目值
int gtp_set_komi(string sn)
{
	if (sn.length() != 0)
	{
		float k = 0;
		if (sscanf(sn.c_str(), "%f", &k) >= 1)
		{
			UCT_Komi = k;
			b->set_Komi(k);
			return GTP_Response("", GTP_SUCCESS);
		}
	}

	return GTP_Response("", GTP_ERROR);
}

// 設定總時間Main_Time、超時後每子多少秒內出手BY_Time
int gtp_time_settings(string sn)
{
	if (sn.length() != 0)
	{
		int temp1 = 1, temp2, temp3;
		string s1, s2, s3;

		temp1 = sn.find(' ', temp1);
		if (temp1 != -1)
		{
			s1 = sn.substr(0, temp1);
			Main_Time = StrToInt(s1);
			fout << "Main_Time:" << Main_Time << endl;

			temp2 = sn.find(' ', temp1+1);
			s2 = sn.substr(temp1+1, temp2);
			BY_Time = StrToInt(s2);
			fout << "BY_Time:" << BY_Time << endl;

			temp3 = sn.find(' ', temp2+1);
			s3 = sn.substr(temp2+1, temp3);
			BY_Stones = StrToInt(s3);

			return GTP_Response("", GTP_SUCCESS);
		}
	}

	return GTP_Response("", GTP_ERROR);
}

// 設定程式名稱
int gtp_name(string sn)
{
	return GTP_Response("wingo-19", GTP_SUCCESS);
}

// 設定程式版本
int gtp_version(string sn)
{
	return GTP_Response("0.1", GTP_SUCCESS);
}

//分散UCT
int ParallelUCT(int handnum,int color,int *MSQ)
{
	int sendSuccess=-1;

	if(firstConnent)
	{
		Sleep(10000);//waiting 10 seconds for clients connecting to server.
		firstConnent = false;
	}

	fout << endl << "Trying to send Board of No." << handnum << "..." << endl;
	int numFailConnect=0;

	while((sendSuccess = sendBoard(MaxSimulation,handnum,color,MSQ)) != 1)// sendBoard是server.cpp中的函數
	{
		Sleep(500);//每次休息0.5秒
		numFailConnect++;

		if(numFailConnect > 60)
		{
			fout << "send Unsuccess!!!!!" << endl;
			return 404;//未連接成功至HDP
		}
	}

	fout << "Send Success!" << endl;

	int bestMove = receiveResult(handnum);
	return bestMove;
}

// 對手落子
int gtp_play_move(string sn)
{
	if (sn.length() != 0)
	{
		int m = 0;
		string s1 = sn;

		int empty_p = s1.find(' ', 1);
		if (empty_p != -1) //代表此參數有兩截
		{
			sn = s1.substr(empty_p + 1, s1.length());
			s1.resize(empty_p);
			if (s1 != "BLACK" && s1 != "WHITE" && s1 != "Black" && s1 != "White" &&
					s1 != "black" && s1 != "white" && s1 != "b" && s1 != "w" && s1 != "B" && s1 != "W")
			{
				return GTP_Response("", GTP_ERROR);
			}
		}

		if (sn == "pass" || sn == "PASS" || sn == "Pass")
		{
			m = 0;
		}
		else
		{
			m = GetCoordinate(sn);
		}

		if (m != -1)
		{
			if (b->update_Board(m)) // 更新盤面
			{
				return GTP_Response("", GTP_SUCCESS);
			}
		}
	}

	return GTP_Response("", GTP_ERROR);
}

// 本程式落子
int gtp_com_play(string sn)
{
	float winrate = 0;
	int m = 0, MSQ[M_Sequence_Limit] = {0}; // move & move sequence
	int hn = b->get_hand_num(); 		// hand number
	int num_open_moves = 0, open_moves[300] = {0};
	int c = b->get_color();

	if (hn <= 2)
	{
		MaxSimulation = Simulation_Limit;
		LeftTime = Main_Time;
	}

	if (LeftTime <=  180)
	{
		MaxSimulation = Simulation_Limit / 10;
	}

	fout << "MaxSimulation:" << MaxSimulation << endl;

	int startTime = time(NULL);

	if (hit_book && (hn <= Open_Step_Limit))
	{
		num_open_moves = pattern->get_open_moves(c,b->board,open_moves);

		if (num_open_moves > 0)
		{
			m = open_moves[rand() % num_open_moves];
			fout << "Hit book success!" << m << endl;
		}
		else
		{
			hit_book = false;
			fout << "Hit book fail!" << endl;

			b->get_Move_Sequence(MSQ);

			if (Mode == 1)
			{
				m = UCTSearch(MSQ, hn, &winrate);
			}
			else
			{
				if ((m = ParallelUCT(hn, c, MSQ)) == 404)
				{
					m = UCTSearch(MSQ, hn, &winrate);
					fout << "Parallel UCT fail!" << endl;
				}
			}
		}
	}
	else
	{
		hit_book = false;

		b->get_Move_Sequence(MSQ);

		if (Mode == 1)
		{
			m = UCTSearch(MSQ, hn, &winrate);
		}
		else
		{
			if ((m = ParallelUCT(hn, c, MSQ)) == 404)
			{
				fout << "Parallel UCT fail!" << endl;
				m = UCTSearch(MSQ, hn, &winrate);
			}
		}
	}

	if (m != -1)
	{
		string s1 = "", s2 = "";

		if (m == PASS)
		{
			s1 = "pass";
		}
		else
		{
			char c = (char) ('A' - 1 + m % BoardSize);

			if (c >= 'I')
			{
				c++;
			}

			IntToStr(BoardSize - m / BoardSize - 1, s2);
			s1 = s1 + c + s2 + "";
		}

		PastTime= time(NULL) - startTime; //紀錄每著手時間
		fout<<"第"<<hn<<"手，花費"<<PastTime<<"秒"<<endl;
		TotalTime += time(NULL) - startTime;

		fout<<"LeftTime:"<<LeftTime<<endl;

		if (b->update_Board(m)) // 更新盤面
		{
			return GTP_Response(s1, GTP_SUCCESS); // 回覆GTP engine程式落子
		}
		else
		{
			return GTP_Response("", GTP_ERROR);
		}
	}
	else
	{
		return GTP_Response("resign", GTP_SUCCESS); // 回覆GTP engine認輸
	}
}

int gtp_undo(string sn)
{
	if (b->get_hand_num() > 1)
	{
		Board *c = new Board();
		int Move_Sequence[M_Sequence_Limit];
		b->get_Move_Sequence(Move_Sequence);
		for (int i = 1; i < b->get_hand_num() - 1; i++)
		{
			c->update_Board(Move_Sequence[i]);
		}
		delete b;
		b = c;
	}

	return GTP_Response("", GTP_SUCCESS);
}

int gtp_quit(string sn)
{
	GTP_Response("", GTP_SUCCESS);
	return GTP_ENDALL;
}

int gtp_show_board(string sn = "")
{
	b->show_Board();

	cout << "********************************" << endl;
	cout << "#";
	cout << endl << endl;

	return GTP_NORESPONSE;
}

int gtp_clear_board(string sn)
{
	Board *c = new Board();

	delete b;
	b = c;

	return GTP_Response("", GTP_SUCCESS);
}

int gtp_time_left(string sn)
{
	if (sn.length() != 0)
	{
		int temp = 1;
		string s1, s2, s3;

		temp = sn.find(' ', temp);
		if (temp != -1)
			s1 = sn.substr(0, temp);

		int temp2 = sn.find(' ', temp + 1);

		if (temp2 != -1) {
			s2 = sn.substr(temp + 1, temp2 - (temp + 1));
			LeftTime = StrToInt(s2);
			return GTP_Response("", GTP_SUCCESS);
		}
	}

	return GTP_Response("", GTP_ERROR);
}

int gtp_protocol_version(string sn)
{
	return GTP_Response("2", GTP_SUCCESS);
}

// 電腦對弈結束時，告知伺服器可結束比賽
int gtp_finish_game(string sn)
{
	hit_book = true; // 重設棋譜搜尋

	return GTP_Response("", GTP_SUCCESS);
}
