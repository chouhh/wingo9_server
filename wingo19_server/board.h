/*
 * board.h
 *
 * Created on: 2011/7/13
 * Author: chouhh
 */

#ifndef PARALLELWINGO_BOARD_H_
#define PARALLELWINGO_BOARD_H_

#include <string.h>
#include <stdlib.h>
#include <mem.h>

#include "config.h"
#include "go_string.h"
#include "linked_list.h"

namespace CJCU
{

class Board
{
private:
	int num_LegalMoves; 					// 合法著點總數
	int num_bLegalMoves;
	int num_wLegalMoves;
	int KO_point; 							// 劫點位置，若為0代表無劫點
	int color; 								// 目前輪到的顏色
	int hand_num; 							// 目前手數
	float komi; 							// 貼目，一般為7.5
	bool renew_legal;

	GO_String strings[MAXSTRNUM]; 			// 儲存盤面上的棋串

	int string_id[BoardArraySize]; 			// 儲存盤面上每個點所對應到的棋串編號
	int next_string_stone[BoardArraySize]; 		// 同棋串中下一顆棋子的位置

	int max_adj_stones[BoardArraySize]; 	// 每個點最多氣數，角為2，邊為3，其他為4

	int detected[BoardArraySize]; 			// 偵測棋串氣數
	INT_LinkedList updated_strings_id; 		// 紀錄被更動過的棋串編號
	int max_strings_id;                     // 目前最大棋串編號

	int Move_Sequence[M_Sequence_Limit];

public:
	int last_move;
	int board[BoardArraySize];     			// 目前盤面
	int *LegalMoves;
	int bLegalMoves[RealBoardSize * RealBoardSize];
	int wLegalMoves[RealBoardSize * RealBoardSize];

	Board();
	Board(Board* b);
	Board(int* MSQ, int hn, float km); 	// Move sequence, hand number, and komi

	bool update_Board(int m);
	void show_Board();

	void set_Komi(float k);
	void set_PointColor(int p, int c);

	int play_RandomGame();
	int select_Move(int hn);
	int select_urgency_moves(int *moves);
	int decide_Winner();
	void decide_Legal();
	void decide_Liberty(int p, int c, int *num, int* detect, int* lp); // 計算點p所在棋串的氣數(0,1,2)
	void flip_Color();
	void capture_String(int t); 		// 提取string s的所有棋子並更改sn、next
	void remove_Legal(int num_moves, int *moves, int p);

	bool is_Legal(int p); 			// 判斷此點是否為合法點
	bool is_Legal(int p, int c); 			// 判斷此點是否為合法點
	bool is_Suicide(int p); 			// 判斷以next_color落子於此點是否為自殺
	bool is_Suicide(int p, int c); 			// 判斷以next_color落子於此點是否為自殺
	bool is_Eye(int p);				// 點p是否為(暫時)眼(new)
	bool is_Eye(int p, int c);				// 點p是否為(暫時)眼(new)
	bool is_TrueEye(int p); 				// 點p是否為眼
	bool is_TrueEye(int p, int c); 				// 點p是否為眼

	void get_Board(int nb[BoardArraySize]);
	void get_String(GO_String s[MAXSTRNUM]);
	int get_color();
	int get_LegalNum();
	int get_PointColor(int p); 			//得知NowBoard中某一點的顏色
	int get_hand_num();
	int get_last_move();
	void get_Move_Sequence(int *ts);
	int num_adj_stones(int p);
	int num_all_neighbors(int p);
	int num_adj_friends(int p);
	int num_all_friends(int p);			//某一點的8個方位全部同色棋子的數量(new)
	int num_adj_edge(int p);			//某一點周遭的邊界數量，角:2個、邊:1個、中間:0個(new)
	int get_adj_empty_points(int p, int *points);
	int hashcode_pattern3(int p);
	int hashcode_pattern5(int p, string *pattern_code);
	int distance(int p1, int p2);
};

} //namespace CJCU

#endif /* PARALLELWINGO_BOARD_H_ */
