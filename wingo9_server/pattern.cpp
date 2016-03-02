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

namespace CJCU
{

int Pattern::GetPattern()
{
	int code0, code1, code2, code3;

	//讀取3X3棋形
	ifstream f3in("pattern_3.txt", ios::in);

	if (!f3in.is_open())
	{
		return false;
	}

	f3in >> code0;

	while (code0 != -1)
	{
		Patterns_3[code0] = true;
		f3in >> code0;
	}

	f3in.close();

	//讀取5X5棋形(black)
	ifstream f5bin("pattern_5b.txt", ios::in);

	if (!f5bin.is_open())
	{
		return false;
	}

	f5bin >> code0 >> code1 >> code2 >> code3;

	while (code0 != -1)
	{
		stringstream ss;

		ss << code0;
		ss << " " + code1;
		ss << " " + code2;
		ss << " " + code3;

		string key;
		key = ss.str();

		Pattern5b.insert(key);

		f5bin >> code0 >> code1 >> code2 >> code3;
	}

	f5bin.close();

	//讀取5X5棋形(white)
	ifstream f5win("pattern_5w.txt", ios::in);

	if (!f5win.is_open())
	{
		return false;
	}

	f5win >> code0 >> code1 >> code2 >> code3;

	while (code0 != -1)
	{
		stringstream ss;

		ss << code0;
		ss << " " + code1;
		ss << " " + code2;
		ss << " " + code3;

		string key;
		key = ss.str();

		Pattern5w.insert(key);

		f5win >> code0 >> code1 >> code2 >> code3;
	}

	f5win.close();

	//讀取黑棋棋譜
	ifstream finb("black_book.txt", ios::in); // open file

	if (!finb.is_open())
	{
		return false;
	}

	while (true)
	{
		stringstream ss1, ss2;

		for (int j = 0; j < RealBoardSize; j++)
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

		map<string, string>::iterator iter;
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

	BL: finb.close();

	//讀取白棋棋譜
	ifstream finw("white_book.txt", ios::in);

	if (!finw.is_open()) {
		return false;
	}

	while (true) {
		stringstream ss1, ss2;

		for (int j = 0; j < RealBoardSize; j++) {
			long long subcode;
			if (!(finw >> subcode)) {
				goto WL;
			}
			ss1 << subcode;
		}
		string key;
		key = ss1.str();

		int val;
		finw >> val;
		ss2 << val;

		map<string, string>::iterator iter;
		iter = White_Book.find(key);
		if (iter != White_Book.end()) {
			White_Book[key] = iter->second + " " + ss2.str();
		} else {
			White_Book[key] = ss2.str();
		}
	}

	WL: finw.close();

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
