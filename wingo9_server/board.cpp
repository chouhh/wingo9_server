/*
 * board.cpp
 *
 * Created on: 2013/8/2
 * Modified on: 2016/2/27
 * Author: sjean
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <cstdlib>

#include "board.h"
#include "config.h"

using namespace std;

namespace CJCU
{

ofstream fout;

Board::Board()
{
	// �L������l�Ƭ����I
	memset(board, EMPTY, sizeof(board));

	//�䪺��l��
	for (int i = 0; i < BoardSize; i++)
	{
		board[i] = EDGE; // �W�����
		board[i * BoardSize] = EDGE; // �������
		board[i + BoardSize * (BoardSize - 1)] = EDGE; // �U�����
		board[i * BoardSize + (BoardSize - 1)] = EDGE; // �k�����
	}

	memset(strings, 0, sizeof(strings));
	memset(string_id, 0, sizeof(string_id));
	memset(next_string_stone, 0, sizeof(next_string_stone));

	memset(max_adj_stones, 0, sizeof(max_adj_stones));
	memset(bLegalMoves, 0, sizeof(bLegalMoves));
	memset(wLegalMoves, 0, sizeof(wLegalMoves));

	int k = 0;
	for (int i=0; i<BoardArraySize; i++)
	{
		max_adj_stones[i] = 4;
		// ���I����
		if (i/BoardSize == 1 || i/BoardSize == BoardSize-2 || i%BoardSize == 1 || i%BoardSize == BoardSize-2)
		{
			max_adj_stones[i] = 3;
		}

		// �w�]�X�k�I���h�䪺�Ҧ��I
		if (i/BoardSize != 0 && i/BoardSize != BoardSize-1 && i%BoardSize != 0 && i%BoardSize != BoardSize-1)
		{
			bLegalMoves[k] = i;
			wLegalMoves[k] = i;
			k++;
		}
	}

	// ���I����
	max_adj_stones[BoardSize + 1] = 2; 						// ���W��
	max_adj_stones[2 * BoardSize - 2] = 2; 				    // �k�W��
	max_adj_stones[(BoardSize - 2) * BoardSize + 1] = 2; 	// ���U��
	max_adj_stones[(BoardSize - 1) * BoardSize - 2] = 2; 	// �k�U��

	num_LegalMoves = k; 		                            //�ŽL���A�C�@�I�ҥi�U
	num_bLegalMoves = k;
	num_wLegalMoves = k;
	LegalMoves = bLegalMoves;
	renew_legal = true;
	KO_point = 0; 											//�N��ثe�L�T�I
	color = BLACK;
	hand_num = 1;
	komi = UCT_Komi;

	last_move = 0;
	max_strings_id = 0;

	srand(time(NULL));
}

Board::Board(Board* b)
{
	memcpy(board, b->board, sizeof(board));
	memcpy(strings, b->strings, sizeof(strings));
	memcpy(string_id, b->string_id, sizeof(string_id));
	memcpy(next_string_stone, b->next_string_stone, sizeof(next_string_stone));
	memcpy(max_adj_stones, b->max_adj_stones, sizeof(max_adj_stones));
	memcpy(bLegalMoves, b->bLegalMoves, sizeof(bLegalMoves));
	memcpy(wLegalMoves, b->wLegalMoves, sizeof(wLegalMoves));
	memcpy(Move_Sequence, b->Move_Sequence, sizeof(Move_Sequence));

	KO_point = b->KO_point;
	color = b->color;
	hand_num = b->hand_num;
	komi = b->komi;
	last_move = b->last_move;
	max_strings_id = b->max_strings_id;
	num_LegalMoves = b->num_LegalMoves;
	num_bLegalMoves = b->num_bLegalMoves;
	num_wLegalMoves = b->num_wLegalMoves;
	renew_legal = b->renew_legal;

	if (color == BLACK)
	{
		LegalMoves = bLegalMoves;
	}
	else
	{
		LegalMoves = wLegalMoves;
	}

	srand(time(NULL));
}

Board::Board(int* MSQ, int hn, float km)
{
	//��l��Board
	memset(board, EMPTY, sizeof(board));
	//�䪺��l��
	for (int i = 0; i < BoardSize; i++)
	{
		board[i] = EDGE;
		board[i * BoardSize] = EDGE;
		board[i + BoardSize * (BoardSize - 1)] = EDGE;
		board[i * BoardSize + (BoardSize - 1)] = EDGE;
	}

	memset(strings, 0, sizeof(strings));
	memset(string_id, 0, sizeof(string_id));
	memset(next_string_stone, 0, sizeof(next_string_stone));
	memset(max_adj_stones, 0, sizeof(max_adj_stones));
	memset(bLegalMoves, 0, sizeof(bLegalMoves));
	memset(wLegalMoves, 0, sizeof(wLegalMoves));

	int k = 0;
	for (int i = 0; i < BoardArraySize; i++)
	{
		max_adj_stones[i] = 4;
		if (i/BoardSize == 1 || i/BoardSize == BoardSize-2 || i%BoardSize == 1 || i % BoardSize == BoardSize-2)
		{
			max_adj_stones[i] = 3;
		}

		if (i/BoardSize != 0 && i/BoardSize != BoardSize-1 && i%BoardSize != 0 && i%BoardSize != BoardSize-1)
		{
			bLegalMoves[k] = i;
			wLegalMoves[k] = i;
			k++;
		}
	}

	max_adj_stones[BoardSize + 1] = 2;
	max_adj_stones[(BoardSize - 1) * 2] = 2;
	max_adj_stones[BoardSize * (BoardSize - 2) + 1] = 2;
	max_adj_stones[BoardSize * (BoardSize - 1) - 2] = 2;

	num_LegalMoves = k;
	num_bLegalMoves = k;
	num_wLegalMoves = k;
	LegalMoves = bLegalMoves;
	renew_legal = true;
	KO_point = 0;
	color = BLACK;
	hand_num = 1;
	komi = km;

	last_move = 0;
	max_strings_id = 0;

	for (int i=1; i<hn; i++) // �̧ǿ�J�ⶶ�åB��s�L��
	{
		update_Board(MSQ[i]);
	}

	srand(time(NULL));
}

int Board::play_RandomGame()
{
	bool Pass = false;
	int m = 0;

	Board clone(this); // �ƻs�ثe�L��

	while (clone.hand_num < M_Sequence_Limit) // ��ƶW�L����h����
	{
		m = clone.select_Move(clone.hand_num); // ���I

		// �s��Pass�h�פ�
		if (m == 0)
		{
			if (Pass)
			{
				break;
			}
			else
			{
				Pass = true;
			}
		}
		else
		{
			Pass = false;
		}

		clone.update_Board(m); // �N��X���I�U��ѽL��
	}

	return clone.decide_Winner(); // �M�w�֬OĹ�a
}

int Board::select_Move(int hn)
{
	// �Y�L�X�k�B�A�h�^��PASS
 	if (num_LegalMoves == 0)
	{
		return PASS;
	}

 	int total_urgency = 0, pattern_code3 = 0;
 	string pattern_code5;
	int Urgency_Moves[BoardArraySize] = {0}; 	// �C�@�I�����ȳ����w�]��0
	int is_Legal_Moves[BoardArraySize] = {0};
	int pattern_score, extension_score, capture_score, distance_score;

	pattern_score = Pattern_Urgency;
	extension_score = Extension_Urgency;
	capture_score = Capture_Urgency;
	distance_score = Distance_Urgency;

    for(int i=0; i<num_LegalMoves; i++)
    {
    	is_Legal_Moves[LegalMoves[i]] = 1;
    }

    // �Ѧ�Q�s�Y�ɡA�W�[����I�����ȡA�n���δ��Y
	for (int i = 0; i < max_strings_id; i++)
	{
		if (strings[i].stone_num != 0) // �u�Ҽ{�D�ŴѦ�
		{
			if (strings[i].liberty == 1) // �Ѧ�u��1��
			{
				if (is_Legal_Moves[strings[i].liberty_point] == 1) // �Ӯ��I���X�k�B
				{
					if (strings[i].color == color) // �ڤ�Ѧ�(�h��)
					{
						Urgency_Moves[strings[i].liberty_point] += (strings[i].stone_num * extension_score);
						total_urgency += (strings[i].stone_num * extension_score);
					}
					else // ���Ѧ�(�h��)
					{
						Urgency_Moves[strings[i].liberty_point] += (strings[i].stone_num * capture_score);
						total_urgency += (strings[i].stone_num * capture_score);
					}
				}
			}
		}
	}

    for(int i=0; i<num_LegalMoves; i++)
    {
    	if (Urgency_Moves[LegalMoves[i]] == 0)
    	{
    		// Pattern 3 match
    		pattern_code3 = hashcode_pattern3(LegalMoves[i]);

    		if (Patterns_3[pattern_code3])
    		{
    			Urgency_Moves[LegalMoves[i]] += pattern_score;
    			total_urgency += pattern_score;
    		}

    		// Pattern 5 match
/*    		hashcode_pattern5(LegalMoves[i], &pattern_code5);
    		set<string>::iterator it;
    		if (color == BLACK)
    		{
    			it = Pattern5b.find(pattern_code5);
    			if (it != Pattern5b.end())
    			{
    				Urgency_Moves[LegalMoves[i]] += pattern_score;
    				total_urgency += pattern_score;
    			}
    		}
    		else
    		{
    			it = Pattern5w.find(pattern_code5);
    			if (it != Pattern5w.end())
    			{
    				Urgency_Moves[LegalMoves[i]] += pattern_score;
    				total_urgency += pattern_score;
    			}
    		}*/

        	if ((hn > 1) && (last_move != 0) && (distance(LegalMoves[i], last_move) <= 4))
        	{
        		Urgency_Moves[LegalMoves[i]] += distance_score;
        		total_urgency += distance_score;
        	}
    	}
    }

	// �H����X����I
	if (total_urgency == 0)
	{
		return LegalMoves[rand() % num_LegalMoves];	// �������A�h�H����@���I
	}
	else
	{
		int threshhold_urgency = rand() % total_urgency;
		for (int i=0; i<num_LegalMoves; i++)
		{
			total_urgency = total_urgency - Urgency_Moves[LegalMoves[i]];
			if (total_urgency < threshhold_urgency)
			{
				return LegalMoves[i];
			}
		}
	}

	return LegalMoves[0];
}

int Board::select_urgency_moves(int *moves)
{
 	if (num_LegalMoves == 0)
	{
		return -1;
	}

 	int pattern_code3 = 0;
 	string pattern_code5;
	int Urgency_Moves[BoardArraySize] = {0};
	int is_Legal_Moves[BoardArraySize] = {0};

    for(int i=0; i<num_LegalMoves; i++) // During open stage, central points are urgent
    {
    	is_Legal_Moves[LegalMoves[i]] = 1;
    }

	for (int i=0; i<max_strings_id; i++)
	{
		if (strings[i].stone_num != 0) // �u�Ҽ{�D�ŴѦ�
		{
			if (strings[i].liberty == 1) // �Ѧ�u��1��
			{
				if (is_Legal_Moves[strings[i].liberty_point] == 1) // �Ӯ��I���X�k�B
				{
					Urgency_Moves[strings[i].liberty_point] = 1;
				}
			}
		}
	}

    for(int i=0; i<num_LegalMoves; i++)
    {
    	if (Urgency_Moves[LegalMoves[i]] == 0)
    	{
    		// Pattern 3 match
    		pattern_code3 = hashcode_pattern3(LegalMoves[i]);

    		if (Patterns_3[pattern_code3])
    		{
    			Urgency_Moves[LegalMoves[i]] = 1;
    			continue;
    		}

    		// Pattern 5 match
    		hashcode_pattern5(LegalMoves[i], &pattern_code5);
    		set<string>::iterator it;
    		if (color == BLACK)
    		{
    			it = Pattern5b.find(pattern_code5);
    			if (it != Pattern5b.end())
    			{
    				Urgency_Moves[LegalMoves[i]] = 1;
    				continue;
    			}
    		}
    		else
    		{
    			it = Pattern5w.find(pattern_code5);
    			if (it != Pattern5w.end())
    			{
    				Urgency_Moves[LegalMoves[i]] = 1;
    				continue;
    			}
    		}

        	if ((hand_num > 1) && (last_move != PASS) && (distance(LegalMoves[i], last_move) <= 4))
        	{
        		Urgency_Moves[LegalMoves[i]] = 1;
        	}
    	}
    }

	int num_moves = 0;
	for (int i=0; i<num_LegalMoves; i++)
	{
		if (Urgency_Moves[LegalMoves[i]] == 1)
		{
			moves[num_moves] = LegalMoves[i];
			num_moves++;
		}
	}

	if (num_moves == 0)
	{
		num_moves = num_LegalMoves;
		moves = LegalMoves;
	}

	return num_moves;
}

bool Board::update_Board(int m)
{
	int num_captured = 0, total_num_captured = 0, dead_point = 0, i= 0, nb = 0;
	KO_point = 0;
	updated_strings_id.head = NULL;

	if (hand_num >= M_Sequence_Limit)
	{
		return false;
	}

	last_move = m;
	Move_Sequence[hand_num] = last_move;

	if (m != PASS) // �Y���h���ݰ����l�ΧT�I���P�_
	{
		board[m] = color; // �ѽL���l

		// ���l���B�z
		for (i=0; i<4; i++)
		{
			nb = m + adj_dir[i];
			if (board[nb] == -color) // �F�I���Ĥl
			{
				int nb_string = string_id[nb]; // �Ĥl�Ҧb�Ѧ�
				if (strings[nb_string].liberty == 1) // �Ħꤧ��1�A�h�i����
				{
					num_captured = strings[nb_string].stone_num;
					total_num_captured += num_captured;
					capture_String(nb_string); //�N���Ѧ�q�ѽL�W�����A�]�Asn�Bnext

					if (nb_string == max_strings_id)
					{
						max_strings_id--;
					}

					strings[nb_string].del_all(); //�çR�����Ѧꤧ�@�����
					dead_point = nb; //���l���I
				}
			}
		}

		bool has_adj_friend = false;
		// �Ѧ�X��
		for (i=0; i<4; i++)
		{
			nb = m + adj_dir[i];
			if (board[nb] == color) // �F�I�O�ڤ�Ѥl
			{
				strings[string_id[nb]].add_stone(m, string_id, next_string_stone); // �[�J�Ѧ�
				has_adj_friend = true;
				break;
			}
		}

		if (i < 3)
		{
			for (int j = i+1; j < 4; j++)
			{
				int nb = m + adj_dir[j];
				if (board[nb] == color) // �ڤ�Ѥl
				{
					if (string_id[m] != string_id[nb]) // �Ѧ�s�����P
					{
						int temp = string_id[nb];
						if (temp == max_strings_id)
						{
							max_strings_id--;
						}
						strings[string_id[m]].merge(&strings[temp], string_id, next_string_stone);
					}
				}
			}
		}

		if (!has_adj_friend) // �L�۾F�ڤ�Ѧ�A�h���I�ۦ��@�Ѧ�
		{
			int k;
			for (k=1; k<MAXSTRNUM; k++)
			{
				if (strings[k].stone_num == 0)
				{
					break;
				}
			}
			strings[k].create_new_string(m, board[m], k, string_id);

			if (k > max_strings_id)
			{
				max_strings_id = k;
			}
		}

		// �����F��Q�v�T�쪺�Ѧ�
		updated_strings_id.insert(string_id[m]);
		for (i=0; i<4; i++)
		{
			nb = m + adj_dir[i];
			if (string_id[nb] != 0 && string_id[nb] != string_id[m])
			{
				updated_strings_id.insert(string_id[nb]);
			}
		}

		// �p��Ѧ���
		INT_Node* n = updated_strings_id.head;
		while (n != NULL)
		{
			if (strings[n->value].stone_num != 0)
			{
				int num_liberty = 0, liberty_point = 0;
				memset(detected, 0, sizeof(detected));
				decide_Liberty(strings[n->value].start,strings[n->value].color, &num_liberty, detected, &liberty_point);
				strings[n->value].set_liberty(num_liberty, liberty_point);
			}
			n = n->next;
		}

		if (total_num_captured == 1) //�Y�u���@���l
		{
			if ((num_adj_stones(m) - num_adj_friends(m)) == (max_adj_stones[m] - 1)) //�N��ثe�o���Ѥl�Q���l�s�Y
			{
				KO_point = dead_point; //�h���I���T�I
			}
		}
	}

	color = -color;
	hand_num++;

	// �̷ӷs�L���M�s�C��M�w�X�k�B

	if (total_num_captured == 0 && num_adj_stones(m) == 0)
	{
		if (renew_legal)
		{
			remove_Legal(num_bLegalMoves, bLegalMoves, m);
			remove_Legal(num_wLegalMoves, wLegalMoves, m);
			num_bLegalMoves--;
			num_wLegalMoves--;

			int adj_empty_points[4];
			int num_adj_empty_points = get_adj_empty_points(m, adj_empty_points);

			for (int i=0; i<num_adj_empty_points; i++)
			{
				if (!is_Legal(adj_empty_points[i], BLACK))
				{
					remove_Legal(num_bLegalMoves, bLegalMoves, adj_empty_points[i]);
					num_bLegalMoves--;
				}

				if (!is_Legal(adj_empty_points[i], WHITE))
				{
					remove_Legal(num_wLegalMoves, wLegalMoves, adj_empty_points[i]);
					num_wLegalMoves--;
				}
			}
		}
		else
		{
			decide_Legal();
			renew_legal = true;
		}
	}
	else
	{
		decide_Legal(); // according black or white
		renew_legal = false;
	}

	return true;
}

void Board::capture_String(int s) //����string s���Ҧ��Ѥl�ç��sn�Bnext�B�����Q�v�T�Ѧ�
{
	int stone1 = strings[s].start, stone2 = strings[s].start, c = strings[s].color, nb;

	while (stone1 != 0) // string s �D��
	{
		for (int i=0; i < 4; i++)
		{
			nb = stone1 + adj_dir[i];
			if (board[nb] != c)
			{
				updated_strings_id.insert(string_id[nb]);
			}
		}

		stone2 = next_string_stone[stone1];
		board[stone1] = EMPTY;
		string_id[stone1] = 0;
		next_string_stone[stone1] = 0;
		stone1 = stone2;
	}
}

void Board::decide_Liberty(int p, int c, int* num, int* detected, int* lp) //�p���Ip�Ѧ���(0,1,2)
{
	int nb;
	detected[p] = true; // p�I�w�g�Ҽ{

	for (int i = 0; i < 4; i++)
	{
		nb = p + adj_dir[i];
		if (!detected[nb]) // �F�~�I�|���Ҽ{
		{
			if (board[nb] == EMPTY) // �F�~�I�����I
			{
				detected[nb] = true;
				(*num) = (*num) + 1; // ���I�ƥ[1
				(*lp) = nb; // nb�����I
				if (*num == 2) // �Y���I�w�F2�A�h���A�p��U�h
				{
					return;
				}
			}
			else if (board[nb] == c) // �p���P�C��F�~�I
			{
				decide_Liberty(nb, c, num, detected, lp); // ���j�p����
				if (*num == 2) // �Y���I�w�F2�A�h���A�p��U�h
				{
					return;
				}
			}
		}
	}
}

void Board::decide_Legal()
{
	if (color == BLACK)
	{
		LegalMoves = bLegalMoves;
	}
	else
	{
		LegalMoves = wLegalMoves;
	}

	int k = 0;

	for (int i=BoardSize+1; i<BoardArraySize-BoardSize; i++)
	{
		if (is_Legal(i))
		{
			LegalMoves[k] = i;
			k++;
		}
	}

	if (color == BLACK)
	{
		num_bLegalMoves = k;
	}
	else
	{
		num_wLegalMoves = k;
	}

	num_LegalMoves = k;
}

bool Board::is_Legal(int i)
{
	if ((board[i] == EMPTY) && (i != KO_point)) // ���I�B�D�T�I
	{
		if (num_adj_stones(i) == max_adj_stones[i]) // �P��L���I
		{
			if (!is_Suicide(i) && !is_TrueEye(i)) // ���|�y���۱��B���O����
			{
				return true;
			}
		}
		else
		{
			return true; // �P�򦳪��I
		}
	}

	return false;
}

bool Board::is_Legal(int i, int c)
{
	if ((board[i] == EMPTY) && (i != KO_point)) // ���I�B�D�T�I
	{
		if (num_adj_stones(i) == max_adj_stones[i]) // �P��L���I
		{
			if (!is_Suicide(i, c) && !is_TrueEye(i,c)) // ���|�y���۱��B���O����
			{
				return true;
			}
		}
		else
		{
			return true; // �P�򦳪��I
		}
	}

	return false;
}

bool Board::is_Suicide(int p) //�P�_���l���I�O�_���۱�:1.�Ǧ��@�𤧼Ħ�A�h�i�U. 2.�Y�L�A�h�Ǧ�2�𤧧ڦ�A��i�U
{
	int nb = 0;
	for (int i=0; i<4; i++)
	{
		nb = p + adj_dir[i];
		if (board[nb] == EMPTY) // ���䦳���I�A�D�۱�
		{
			return false;
		}
		else if (board[nb] == -color) //�Ǭ��Ĥ�Ѧ�
		{
			if (strings[string_id[nb]].liberty == 1)
			{
				return false; // �Ǧ�1�𤧼Ħ�i���l�A�D�۱�
			}
		}
		else if (board[nb] == color) //�Ǭ��ڤ�Ѧ�
		{
			if (strings[string_id[nb]].liberty > 1)
			{
				return false; // �Ǧ�2�𤧧ڤ�Ѧ�A�D�۱�
			}
		}
	}

	//3.�Y�Ǥ��Ħ�Ҧ��G��A�B�ڤ�Ѧ�ҶȤ@��A�h���i�U
	return true;
}

bool Board::is_Suicide(int p, int c) //�P�_���l���I�O�_���۱�:1.�Ǧ��@�𤧼Ħ�A�h�i�U. 2.�Y�L�A�h�Ǧ�2�𤧧ڦ�A��i�U
{
	int nb = 0;
	for (int i=0; i<4; i++)
	{
		nb = p + adj_dir[i];
		if (board[nb] == EMPTY) // ���䦳���I�A�D�۱�
		{
			return false;
		}
		else if (board[nb] == -c) //�Ǭ��Ĥ�Ѧ�
		{
			if (strings[string_id[nb]].liberty == 1)
			{
				return false; // �Ǧ�1�𤧼Ħ�i���l�A�D�۱�
			}
		}
		else if (board[nb] == c) //�Ǭ��ڤ�Ѧ�
		{
			if (strings[string_id[nb]].liberty > 1)
			{
				return false; // �Ǧ�2�𤧧ڤ�Ѧ�A�D�۱�
			}
		}
	}

	//3.�Y�Ǥ��Ħ�Ҧ��G��A�B�ڤ�Ѧ�ҶȤ@��A�h���i�U
	return true;
}

bool Board::is_Eye(int p)	// �Ip�O�_��(�Ȯ�)��
{
	int nb = 0, num_friends = 0;
	for (int i = 0; i < 4; i++)
	{
		nb = p + adj_dir[i];
		if (board[nb] == color)
		{
			if (strings[string_id[nb]].liberty == 1)
			{
				return false;
			}
			num_friends++;
		}
	}

	int num_edges = num_adj_edge(p);	//���o��ɼƶq
	if (num_edges == 0 && num_friends == 4)	//����(�Ȯ�)��:�ܤ�4��
	{
		return true;
	}
	else if (num_edges == 1 && num_friends == 3)	//��(�Ȯ�)��:�ܤ�3��
	{
		return true;
	}
	else if (num_edges == 2 && num_friends == 2)	//��(�Ȯ�)��:�ܤ�2��
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Board::is_Eye(int p, int c)	// �Ip�O�_��(�Ȯ�)��
{
	int nb = 0, num_friends = 0;
	for (int i = 0; i < 4; i++)
	{
		nb = p + adj_dir[i];
		if (board[nb] == c)
		{
			if (strings[string_id[nb]].liberty == 1)
			{
				return false;
			}
			num_friends++;
		}
	}

	int num_edges = num_adj_edge(p);	//���o��ɼƶq
	if (num_edges == 0 && num_friends == 4)	//����(�Ȯ�)��:�ܤ�4��
	{
		return true;
	}
	else if (num_edges == 1 && num_friends == 3)	//��(�Ȯ�)��:�ܤ�3��
	{
		return true;
	}
	else if (num_edges == 2 && num_friends == 2)	//��(�Ȯ�)��:�ܤ�2��
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Board::is_TrueEye(int p) //�Ip�O�_��(�u)��:�F�~���P�C��B���I���j��1
{
	int nb = 0, num_friends = 0, temp_eyes= 0;
	for(int i = 0; i<8; i++)
	{
		nb = p + all_dir[i];
		if (board[nb] == color)
		{
			if (strings[string_id[nb]].liberty == 1)
			{
				return false;
			}

			num_friends++;
		}
		else if(board[nb] == EMPTY)	//���I�i��O����
		{
			if (is_Eye(nb))
			{
				temp_eyes++;
			}
		}
	}

	int num_edges = num_adj_edge(p);	//���o��ɼƶq�A�P�_���I�����B���άO����
	if (num_edges == 0 && (num_friends + temp_eyes) >= 7)//�����u��:�ܤ�7��
	{
		return true;
	}
	else if (num_edges == 1 && (num_friends + temp_eyes) == 5)//��u��:�ܤ�5��
	{
		return true;
	}
	else if (num_edges == 2 && (num_friends + temp_eyes) == 3)//���u��:�ܤ�3��
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Board::is_TrueEye(int p, int c) //�Ip�O�_��(�u)��:�F�~���P�C��B���I���j��1
{
	int nb = 0, num_friends = 0, temp_eyes= 0;
	for(int i = 0; i<8; i++)
	{
		nb = p + all_dir[i];
		if (board[nb] == c)
		{
			if (strings[string_id[nb]].liberty == 1)
			{
				return false;
			}

			num_friends++;
		}
		else if(board[nb] == EMPTY)	//���I�i��O����
		{
			if (is_Eye(nb, c))
			{
				temp_eyes++;
			}
		}
	}

	int num_edges = num_adj_edge(p);	//���o��ɼƶq�A�P�_���I�����B���άO����
	if (num_edges == 0 && (num_friends + temp_eyes) >= 7)//�����u��:�ܤ�7��
	{
		return true;
	}
	else if (num_edges == 1 && (num_friends + temp_eyes) == 5)//��u��:�ܤ�5��
	{
		return true;
	}
	else if (num_edges == 2 && (num_friends + temp_eyes) == 3)//���u��:�ܤ�3��
	{
		return true;
	}
	else
	{
		return false;
	}
}

int Board::decide_Winner()
{
	float b = 0, w = 0;
	int i = 0, k = 0;

	for (i=BoardSize; i<BoardArraySize-BoardSize; i++)
	{
		if (board[i] == BLACK)
		{
			b++;
		}
		else if (board[i] == WHITE)
		{
			w++;
		}
		else if (board[i] == EMPTY)
		{
			for (k=0; k<4; k++)
			{
				if (board[i + adj_dir[k]] == BLACK)
				{
					b++;
					break;
				} else if (board[i + adj_dir[k]] == WHITE)
				{
					w++;
					break;
				}
			}
		}
	}

	if ((b - komi) > w)
	{
		return BLACK;
	}
	else
	{
		return WHITE;
	}

}

void Board::set_PointColor(int p, int c)
{
	board[p] = c;
}

void Board::set_Komi(float k)
{
	komi = k;
}

void Board::flip_Color()
{
	color = -color;
}

int Board::get_PointColor(int p) //�o��NowBoard���Y�@�I���C��
{
	return board[p];
}


int Board::get_LegalNum()
{
	return num_LegalMoves;
}

int Board::get_color()
{
	return color;
}

void Board::get_String(GO_String s[MAXSTRNUM])
{
	memcpy(s, strings, sizeof(strings));
}

int Board::get_hand_num()
{
	return hand_num;
}

void Board::get_Board(int nb[BoardArraySize])
{
	memcpy(nb, board, sizeof(board));
}

int Board::get_last_move()
{
	return last_move;
}

void Board::get_Move_Sequence(int *ts)
{
	memcpy(ts, Move_Sequence, sizeof(Move_Sequence));
}

int Board::num_adj_stones(int p)
{
	int nb = 0, num_stones = 0;
	for (int i=0; i<4; i++)
	{
		nb = p + adj_dir[i];
		if ((board[nb] == BLACK) || (board[nb] == WHITE))
		{
			num_stones++;
		}
	}

	return num_stones;
}

int Board::num_adj_friends(int p)
{
	int nb = 0, num_friends = 0;
	for (int i=0; i<4; i++)
	{
		nb = p + adj_dir[i];
		if (board[nb] == color)
		{
			num_friends++;
		}
	}

	return num_friends;
}

int Board::num_all_neighbors(int p)
{
	int nb = 0, num_opp = 0;
	for (int i=0; i<4; i++)
	{
		nb = p + adj_dir[i];
		if (board[nb] == -color)
		{
			num_opp++;
		}
	}

	return num_opp;
}

int Board::num_all_friends(int p)
{
	int nb = 0, num_friends = 0;
	for (int i=0; i<8; i++)
	{
		nb = p + all_dir[i];
		if (board[nb] == color)
		{
			num_friends++;
		}
	}

	return num_friends;
}

int Board::num_adj_edge(int p)
{
	int nb = 0, num_edge = 0;
	for (int i=0; i<4; i++)
	{
		nb = p + adj_dir[i];
		if (board[nb] == EDGE)
		{
			num_edge++;
		}
	}

	return num_edge;
}

int Board::get_adj_empty_points(int p, int *points)
{
	int nb = 0, num_empty = 0;
	for (int i=0; i<4; i++)
	{
		nb = p + adj_dir[i];
		if (board[nb] == EMPTY)
		{
			points[num_empty] = nb;
			num_empty++;
		}
	}

	return num_empty;
}

void Board::remove_Legal(int num_moves, int *moves, int p)
{
	bool found = false;

	for (int i=0; i<num_moves; i++)
	{
		if (p == moves[i])
		{
			found = true;
			continue;
		}

		if (found)
		{
			moves[i-1] = moves[i];
		}
	}
}

void Board::show_Board()
{
	fout.open("commands_log.txt",ios::app);
	fout << endl << "   ";
	for (int i = 0; i < RealBoardSize; i++)
	{
		if ((char) ('A' + i) < 'I')
		{
			fout << (char) ('A' + i) << " ";
		}
		else
		{
			fout << (char) ('A' + i + 1) << " ";
		}
	}
	fout << endl;

	string s1;
	for (int i = BoardSize; i < BoardArraySize - BoardSize; i++)
	{
		int x = i % BoardSize;
		if (x >= 0 && x <= BoardSize)
		{
			if (x == 0)
			{
				if (BoardSize - i / BoardSize - 1 < 10)
				{
					fout << BoardSize - i / BoardSize - 1 << "  ";
				}
				else
				{
					fout << BoardSize - i / BoardSize - 1 << " ";
				}
			}
			else if (x == BoardSize - 1)
			{
				fout << endl;
			}
			else
			{
				if (board[i] == EMPTY)
				{
					fout << ". ";
				}
				else if (board[i] == BLACK)
				{
					fout << "X ";
				}
				else if (board[i] == WHITE)
				{
					fout << "O ";
				}
			}
		}
	}

	fout << endl;
	fout.close();
}

int Board::hashcode_pattern3(int p)
{
	int pattern_code = 0;

	for (int j = 0; j < 8; j++) // �ھکP��K�Ӧ�m���C��i��s�X
	{
		pattern_code = pattern_code << 2;
		switch (board[p + all_dir[j]])
		{
		case BLACK:
			pattern_code += 1;
			break;
		case WHITE:
			pattern_code += 2;
			break;
		case EDGE:
			pattern_code += 3;
			break;
		}
	}

	return pattern_code;
}

int Board::hashcode_pattern5(int p, string *pattern_code)
{
	int code[4] = {0,0,0,0};

	int x = p % BoardSize;
	int y = p / BoardSize;

	code[0] = hashcode_pattern3(p);

	if ((x > 1)&&(x < 9) && (y > 1) && (y < 9))
	{
		for (int j=0; j < 8; j++)
		{
			code[1] = code[1] << 2;
			switch (board[p + extend_dir[j]])
			{
			case BLACK:
				code[1] += 1;
				break;
			case WHITE:
				code[1] += 2;
				break;
			case EDGE:
				code[1] += 3;
				break;
			}
		}

		for (int j=8; j < 16; j++)
		{
			code[2] = code[2] << 2;
			switch (board[p + extend_dir[j]])
			{
			case BLACK:
				code[2] += 1;
				break;
			case WHITE:
				code[2] += 2;
				break;
			case EDGE:
				code[2] += 3;
				break;
			}
		}

		code[3] = (code[0] + code[1]*16 + code[2]*256) % 65536;

		stringstream ss;

		ss << code[0];
		ss << " " + code[1];
		ss << " " + code[2];
		ss << " " + code[3];

		*pattern_code = ss.str();

		return 1;
	}

	return 0;
}

int Board::distance(int p1, int p2)
{
	int x1 = p1 % BoardSize;
	int x2 = p2 % BoardSize;
	int y1 = p1 / BoardSize;
	int y2 = p2 / BoardSize;
	int dx = (x1 > x2) ? (x1 - x2) : (x2 - x1);
	int dy = (y1 > y2) ? (y1 - y2) : (y2 - y1);

	return dx + dy;
}

} //namespace CJCU
