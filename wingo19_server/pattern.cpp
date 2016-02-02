/*
 * pattern.cpp
 *
 * Created on: 2011/7/14
 * Author: chouhh
 */

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <map>

#include "config.h"
#include "pattern.h"

using namespace std;

namespace CJCU {

int Pattern::GetPattern()
 {
	int index0, index1, index2, index3;

	//讀取3X3棋形
	ifstream f3in("pattern_3.txt",ios::in);

	if (!f3in.is_open())
	{
		return false;
	}

	f3in >> index0;

	do
	{
		Patterns_3[index0] = true;
		f3in >> index0;
	} while  (index0 == -1);

	f3in.close();

	//讀取5X5棋形(black)
	ifstream f5bin("pattern_5b.txt",ios::in);

	if (!f5bin.is_open())
	{
		return false;
	}

	f5bin >> index0 >> index1 >> index2 >> index3;

	do
	{
		Patterns_5b[0][index0] = true;
		Patterns_5b[1][index1] = true;
		Patterns_5b[2][index2] = true;
		Patterns_5b[3][index3] = true;
		f5bin >> index0 >> index1 >> index2 >> index3;
	} while  (index0 == -1);

	f5bin.close();

	//讀取5X5棋形(white)
	ifstream f5win("pattern_5w.txt",ios::in);

	if (!f5win.is_open())
	{
		return false;
	}

	f5win >> index0 >> index1 >> index2 >> index3;

	do
	{
		Patterns_5w[0][index0] = true;
		Patterns_5w[1][index1] = true;
		Patterns_5w[2][index2] = true;
		Patterns_5w[3][index3] = true;
		f5win >> index0 >> index1 >> index2 >> index3;
	} while  (index0 == -1);

	f5win.close();

	//讀取黑棋棋譜
	ifstream finb("black_book.txt",ios::in); // open file

	if (!finb.is_open())
	{
		return false;
	}

    while (true)
	{
		stringstream ss1, ss2;

		for (int j=0; j<RealBoardSize; j++)
		{
			long long subcode;
			if (!(finb >> subcode)) // is there any record?
			{
				goto BL;
			}
			ss1 << subcode;
		}
		string key;
		key = ss1.str();

		int val;
		finb >> val;
		ss2 << val;

		map<string,string>::iterator iter;
	    iter = Black_Book.find(key);
	    if (iter != Black_Book.end())
	    {
	    	Black_Book[key] = iter->second + " " + ss2.str();
	    }
	    else
	    {
	    	Black_Book[key] = ss2.str();
	    }
	}

BL:	finb.close();

	//讀取白棋棋譜
	ifstream finw("white_book.txt",ios::in);

	if (!finw.is_open())
	{
		return false;
	}

	while (true)
	{
		stringstream ss1, ss2;

		for (int j=0; j<RealBoardSize; j++)
		{
			long long subcode;
			if (!(finw >> subcode))
			{
				goto WL;
			}
			ss1 << subcode;
		}
		string key;
		key = ss1.str();

		int val;
		finw >> val;
		ss2 << val;

		map<string,string>::iterator iter;
	    iter = White_Book.find(key);
	    if (iter != White_Book.end())
	    {
	    	White_Book[key] = iter->second + " " + ss2.str();
	    }
	    else
	    {
	    	White_Book[key] = ss2.str();
	    }
	}

WL:	finw.close();

//讀取黑棋定石
ifstream finbj("black_joseki.txt",ios::in); // open file

if (!finbj.is_open())
{
	return false;
}

while (true)
{
	stringstream ss1, ss2;

	for (int j=0; j<JosekiSize; j++)
	{
		long long subcode;
		if (!(finbj >> subcode)) // is there any record?
		{
			goto BJ;
		}
		ss1 << subcode;
	}
	string key;
	key = ss1.str();

	int val;
	finbj >> val;
	ss2 << val;

	map<string,string>::iterator iter;
    iter = Black_Joseki.find(key);
    if (iter != Black_Joseki.end())
    {
    	Black_Joseki[key] = iter->second + " " + ss2.str();
    }
    else
    {
    	Black_Joseki[key] = ss2.str();
    }
}

BJ:	finbj.close();

//讀取白棋定石
ifstream finwj("white_joseki.txt",ios::in); // open file

if (!finwj.is_open())
{
	return false;
}

while (true)
{
	stringstream ss1, ss2;

	for (int j=0; j<JosekiSize; j++)
	{
		long long subcode;
		if (!(finwj >> subcode)) // is there any record?
		{
			goto WJ;
		}
		ss1 << subcode;
	}
	string key;
	key = ss1.str();

	int val;
	finwj >> val;
	ss2 << val;

	map<string,string>::iterator iter;
    iter = White_Joseki.find(key);
    if (iter != White_Joseki.end())
    {
    	White_Joseki[key] = iter->second + " " + ss2.str();
    }
    else
    {
    	White_Joseki[key] = ss2.str();
    }
}

WJ:	finwj.close();

	return true;
}

int Pattern::get_open_moves(int c, int *board, int *moves)
{
	int num_moves = 0;
	string key = hashcode_board(board);

	if (c == BLACK)
	{
		map<string,string>::iterator iter;
	    iter = Black_Book.find(key);
	    if (iter != Black_Book.end())
	    {
			istringstream iss(iter->second);
			string moveStrings[300];

			do {
				iss >> moveStrings[num_moves];
				num_moves++;
			} while (iss);
			num_moves--;

			for (int i=0; i<num_moves; i++)
			{
				moves[i] = atoi(moveStrings[i].c_str());
			}
	    }
	    else
	    {
	    	return 0;
	    }
	}
	else
	{
		map<string,string>::iterator iter;
	    iter = White_Book.find(key);
	    if (iter != White_Book.end())
	    {
			istringstream iss(iter->second);
			string moveStrings[300];

			do {
				iss >> moveStrings[num_moves];
				num_moves++;
			} while (iss);
			num_moves--;

			for (int i=0; i<num_moves; i++)
			{
				moves[i] = atoi(moveStrings[i].c_str());
			}
	    }
	    else
	    {
	    	return 0;
	    }
	}

	return num_moves;
}

int Pattern::get_joseki_moves(int c, int *board, int *moves)
{
	int num_moves = 0;

	string key = hashcode_board(board,1,1,10,10);
	int number = 0;
	if (c == BLACK)
	{
		map<string,string>::iterator iter;
	    iter = Black_Joseki.find(key);
	    if (iter != Black_Joseki.end())
	    {
			istringstream iss(iter->second);
			string moveStrings[300];

			do {
				iss >> moveStrings[number];
				number++;
			}while (iss);
			number--;

			for (int i=0; i<number; i++)
			{
				int m = atoi(moveStrings[i].c_str());
				int y = m / JosekiSize;
				int x = m % JosekiSize;

				if (key.compare("0000000000") == 0)
				{
					if (abs(x-4) + abs(y-4) <= 4)
					{
						moves[num_moves+i] = BoardSize * y + x;
					}
				}
				else
				{
					moves[num_moves+i] = BoardSize * y + x;
				}
			}

			num_moves = num_moves + number;
	    }
	}
	else
	{
		map<string,string>::iterator iter;
	    iter = White_Joseki.find(key);
	    if (iter != White_Joseki.end())
	    {
			istringstream iss(iter->second);
			string moveStrings[300];

			do {
				iss >> moveStrings[number];
				number++;
			} while (iss);
			number--;

			for (int i=0; i<number; i++)
			{
				int m = atoi(moveStrings[i].c_str());
				int y = m / JosekiSize;
				int x = m % JosekiSize;

				if (key.compare("0000000000") == 0)
				{
					if (abs(x-4) + abs(y-4) <= 4)
					{
						moves[num_moves+i] = BoardSize * y + x;
					}
				}
				else
				{
					moves[num_moves+i] = BoardSize * y + x;
				}
			}

			num_moves = num_moves + number;
	    }
	}

	key = hashcode_board(board,10,1,19,10);
	number = 0;
	if (c == BLACK)
	{
		map<string,string>::iterator iter;
	    iter = Black_Joseki.find(key);
	    if (iter != Black_Joseki.end())
	    {
			istringstream iss(iter->second);
			string moveStrings[300];

			do {
				iss >> moveStrings[number];
				number++;
			} while (iss);
			number--;

			for (int i=0; i<number; i++)
			{
				int m = atoi(moveStrings[i].c_str());
				int y = m / JosekiSize;
				int x = m % JosekiSize;

				if (key.compare("0000000000") == 0)
				{
					if (abs(x-13) + abs(y-4) <= 4)
					{
						moves[num_moves+i] = BoardSize * y + x;
					}
				}
				else
				{
					moves[num_moves+i] = BoardSize * y + x;
				}
			}

			num_moves = num_moves + number;
	    }
	}
	else
	{
		map<string,string>::iterator iter;
	    iter = White_Joseki.find(key);
	    if (iter != White_Joseki.end())
	    {
			istringstream iss(iter->second);
			string moveStrings[300];

			do {
				iss >> moveStrings[number];
				number++;
			} while (iss);
			number--;

			for (int i=0; i<number; i++)
			{
				int m = atoi(moveStrings[i].c_str());
				int y = m / JosekiSize;
				int x = m % JosekiSize;

				if (key.compare("0000000000") == 0)
				{
					if (abs(x-13) + abs(y-4) <= 4)
					{
						moves[num_moves+i] = BoardSize * y + x + 9;
					}
				}
				else
				{
					moves[num_moves+i] = BoardSize * y + x + 9;
				}
			}

			num_moves = num_moves + number;
	    }
	}

	key = hashcode_board(board,1,10,10,19);
	number = 0;
	if (c == BLACK)
	{
		map<string,string>::iterator iter;
	    iter = Black_Joseki.find(key);
	    if (iter != Black_Joseki.end())
	    {
			istringstream iss(iter->second);
			string moveStrings[300];

			do {
				iss >> moveStrings[number];
				number++;
			} while (iss);
			number--;

			for (int i=0; i<number; i++)
			{
				int m = atoi(moveStrings[i].c_str());
				int y = m / JosekiSize;
				int x = m % JosekiSize;

				if (key.compare("0000000000") == 0)
				{
					if (abs(x-4) + abs(y-13) <= 4)
					{
						moves[num_moves+i] = BoardSize * (y + 9) + x;
					}
				}
				else
				{
					moves[num_moves+i] = BoardSize * (y + 9) + x;
				}
			}

			num_moves = num_moves + number;
	    }
	}
	else
	{
		map<string,string>::iterator iter;
	    iter = White_Joseki.find(key);
	    if (iter != White_Joseki.end())
	    {
			istringstream iss(iter->second);
			string moveStrings[300];

			do {
				iss >> moveStrings[number];
				number++;
			} while (iss);
			number--;

			for (int i=0; i<number; i++)
			{
				int m = atoi(moveStrings[i].c_str());
				int y = m / JosekiSize;
				int x = m % JosekiSize;

				if (key.compare("0000000000") == 0)
				{
					if (abs(x-4) + abs(y-13) <= 4)
					{
						moves[num_moves+i] = BoardSize * (y + 9) + x;
					}
				}
				else
				{
					moves[num_moves+i] = BoardSize * (y + 9) + x;
				}
			}

			num_moves = num_moves + number;
	    }
	}

	key = hashcode_board(board,10,10,19,19);
	number = 0;
	if (c == BLACK)
	{
		map<string,string>::iterator iter;
	    iter = Black_Joseki.find(key);
	    if (iter != Black_Joseki.end())
	    {
			istringstream iss(iter->second);
			string moveStrings[300];

			do {
				iss >> moveStrings[number];
				number++;
			} while (iss);
			number--;

			for (int i=0; i<number; i++)
			{
				int m = atoi(moveStrings[i].c_str());
				int y = m / JosekiSize;
				int x = m % JosekiSize;

				if (key.compare("0000000000") == 0)
				{
					if (abs(x-13) + abs(y-13) <= 4)
					{
						moves[num_moves+i] = BoardSize * (y + 9) + x + 9;
					}
				}
				else
				{
					moves[num_moves+i] = BoardSize * (y + 9) + x + 9;
				}
			}

			num_moves = num_moves + number;
	    }
	}
	else
	{
		map<string,string>::iterator iter;
	    iter = White_Joseki.find(key);
	    if (iter != White_Joseki.end())
	    {
			istringstream iss(iter->second);
			string moveStrings[300];

			do {
				iss >> moveStrings[number];
				number++;
			} while (iss);
			number--;

			for (int i=0; i<number; i++)
			{
				int m = atoi(moveStrings[i].c_str());
				int y = m / JosekiSize;
				int x = m % JosekiSize;

				if (key.compare("0000000000") == 0)
				{
					if (abs(x-13) + abs(y-13) <= 4)
					{
						moves[num_moves+i] = BoardSize * (y + 9) + x + 9;
					}
				}
				else
				{
					moves[num_moves+i] = BoardSize * (y + 9) + x + 9;
				}
			}

			num_moves = num_moves + number;
	    }
	}

	return num_moves;
}

string Pattern::hashcode_board(int *board)
{
	string codeString = "";

	for (int j=1; j<=RealBoardSize; j++)
	{
		long long code = 0;
		for (int i=1; i<=RealBoardSize; i++)
		{
			code = code << 2;
			switch (board[j*BoardSize + i])
			{
			case BLACK:
				code += 1;
				break;
			case WHITE:
				code += 2;
				break;
			}
		}

		stringstream ss;
		ss << code;
		codeString = codeString + ss.str();
	}

	return codeString;
}

string Pattern::hashcode_board(int *board, int x1, int y1, int x2, int y2)
{
	string codeString = "";

	for (int j=y1; j<=y2; j++)
	{
		long long code = 0;
		for (int i=x1; i<=x2; i++)
		{
			code = code << 2;
			switch (board[j*BoardSize + i])
			{
			case BLACK:
				code += 1;
				break;
			case WHITE:
				code += 2;
				break;
			}
		}

		stringstream ss;
		ss << code;
		codeString = codeString + ss.str();
	}

	return codeString;
}

} //namespace CJCU
