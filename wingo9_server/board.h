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
	int num_LegalMoves; 					// �X�k���I�`��
	int num_bLegalMoves;
	int num_wLegalMoves;
	int KO_point; 							// �T�I��m�A�Y��0�N��L�T�I
	int color; 								// �ثe���쪺�C��
	int hand_num; 							// �ثe���
	float komi; 							// �K�ءA�@�묰7.5
	bool renew_legal;

	GO_String strings[MAXSTRNUM]; 			// �x�s�L���W���Ѧ�

	int string_id[BoardArraySize]; 			// �x�s�L���W�C���I�ҹ����쪺�Ѧ�s��
	int next_string_stone[BoardArraySize]; 		// �P�Ѧꤤ�U�@���Ѥl����m

	int max_adj_stones[BoardArraySize]; 	// �C���I�̦h��ơA����2�A�䬰3�A��L��4

	int detected[BoardArraySize]; 			// �����Ѧ���
	INT_LinkedList updated_strings_id; 		// �����Q��ʹL���Ѧ�s��
	int max_strings_id;                     // �ثe�̤j�Ѧ�s��

	int Move_Sequence[M_Sequence_Limit];

public:
	int last_move;
	int board[BoardArraySize];     			// �ثe�L��
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
	void decide_Liberty(int p, int c, int *num, int* detect, int* lp); // �p���Ip�Ҧb�Ѧꪺ���(0,1,2)
	void flip_Color();
	void capture_String(int t); 		// ����string s���Ҧ��Ѥl�ç��sn�Bnext
	void remove_Legal(int num_moves, int *moves, int p);

	bool is_Legal(int p); 			// �P�_���I�O�_���X�k�I
	bool is_Legal(int p, int c); 			// �P�_���I�O�_���X�k�I
	bool is_Suicide(int p); 			// �P�_�Hnext_color���l���I�O�_���۱�
	bool is_Suicide(int p, int c); 			// �P�_�Hnext_color���l���I�O�_���۱�
	bool is_Eye(int p);				// �Ip�O�_��(�Ȯ�)��(new)
	bool is_Eye(int p, int c);				// �Ip�O�_��(�Ȯ�)��(new)
	bool is_TrueEye(int p); 				// �Ip�O�_����
	bool is_TrueEye(int p, int c); 				// �Ip�O�_����

	void get_Board(int nb[BoardArraySize]);
	void get_String(GO_String s[MAXSTRNUM]);
	int get_color();
	int get_LegalNum();
	int get_PointColor(int p); 			//�o��NowBoard���Y�@�I���C��
	int get_hand_num();
	int get_last_move();
	void get_Move_Sequence(int *ts);
	int num_adj_stones(int p);
	int num_all_neighbors(int p);
	int num_adj_friends(int p);
	int num_all_friends(int p);			//�Y�@�I��8�Ӥ������P��Ѥl���ƶq(new)
	int num_adj_edge(int p);			//�Y�@�I�P�D����ɼƶq�A��:2�ӡB��:1�ӡB����:0��(new)
	int get_adj_empty_points(int p, int *points);
	int hashcode_pattern3(int p);
	int hashcode_pattern5(int p, string *pattern_code);
	int distance(int p1, int p2);
};

} //namespace CJCU

#endif /* PARALLELWINGO_BOARD_H_ */
