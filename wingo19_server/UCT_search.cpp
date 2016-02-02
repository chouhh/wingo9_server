/*
 * UCT_search.cpp
 *
 * Created on: 2011/7/14
 * Author: chouhh
 */

#include "UCT_search.h"
#include "config.h"

namespace CJCU
{
Node* root = NULL;

int UCTSearch(int MSQ[M_Sequence_Limit], int hn, float *winrate)
{
	if (hn > 40)
	{
		if (MSQ[hn-1] == PASS && MSQ[hn-3] == PASS)
		{
			return PASS;
		}
	}

	Board new_board(MSQ, hn, UCT_Komi); // 取得當前盤面資訊

	if (new_board.get_LegalNum() == 0) //若沒有著手可下，則虛手
	{
		return PASS;
	}

	root = new Node(-new_board.get_color(), 0, MSQ[hn]); // color, level, move
	create_Tree(root, &new_board, hn);
	build_Tree(root, &new_board);

	return get_BestMove(root);
}

void create_Tree(Node *r, Board *b, int hn)
{
	r->add_Visits();

	if (hn < 120)
	{
		int moves[BoardArraySize] = {0};
		int num_moves = b->select_urgency_moves(moves);
		if (num_moves > 0)
		{
			r->create_Children(moves, num_moves, b->get_color(), hn);
		}
		else
		{
			r->create_Children(b->LegalMoves , b->get_LegalNum() , b->get_color(), hn);
		}
	}
	else
	{
		r->create_Children(b->LegalMoves , b->get_LegalNum() , b->get_color(), hn);
	}
}

void build_Tree(Node *r, Board *b)
{
	int nsim = 0;

	while (nsim < MaxSimulation)
	{
		Board new_board(b);
		play_Simulation(&new_board, r);
		nsim++;
	}
}

//根據UCT的結果，傳回勝率最高的著手
int get_BestMove(Node *r)
{
	int result = PASS;

	if (r->get_Child() != NULL)
	{
		r->set_BestNode();
		Node *best = r->get_BestNode();

		if (best != NULL)
		{
			if ((float) best->get_Wins() / best->get_Visits() <= 0.15)
			{
				result = RESIGN;
			}
			else
			{
				result = best->get_Move();
			}
		}
	}
	delete r;

	return result;
}

void play_Simulation(Board *b, Node *P_root)
{
	Node* Node_Sequence[M_Sequence_Limit] = {0};
	int Max_Level = 0, win_color = 0;

	get_NodeSequence(P_root, b, Node_Sequence, &Max_Level);
	win_color = b->play_RandomGame();
	update_Tree(Node_Sequence, Max_Level, win_color);
}

void update_Tree(Node** Node_Sequence, int Max_Level, int win_color)
{
	for (int i=0; i<=Max_Level; i++)
	{
		Node_Sequence[i]->add_Visits();
		Node_Sequence[i]->update_Wins(win_color);
	}
}

//1.由root開始往下搜尋，直到遇到(1)leaf node (2)end game
//2.在此過程中，更新Node_Sequence、Max_Level、Board的值
//3.遇到leaf node，則結束
void get_NodeSequence(Node *G_root, Board *b, Node** Node_Sequence, int *Max_Level)
{
	Node *temp = G_root;

	for (;;)
	{
		//temp加入NodeSequence、更改Max_Level
		*Max_Level = temp->get_Level();
		Node_Sequence[temp->get_Level()] = temp;

		if (temp->get_Visits() == 0) //代表找到leaf
		{
			return;
		}
		else //代表這點並非leaf，但不排除是 end game
		{
			if (!temp->has_Child()) //若temp沒有子點
			{
				if (b->get_LegalNum() != 0)
				{
					if (b->get_hand_num() < 120)
					{
						int moves[BoardArraySize] = {0};
						int num_moves = b->select_urgency_moves(moves);
						if (num_moves > 0)
						{
							temp->create_Children(moves, num_moves, b->get_color(), b->get_hand_num());
						}
						else
						{
							temp->create_Children(b->LegalMoves , b->get_LegalNum() , b->get_color(), b->get_hand_num());
						}
					}
					else
					{
						temp->create_Children(b->LegalMoves , b->get_LegalNum() , b->get_color(), b->get_hand_num());
					}
				}
				else
				{
					return;
				}
			}

			Node *next = temp->select_BestChild(); //依照UCTValue選擇適當子點 "next"
			b->update_Board(next->get_Move()); //落子並更新盤面
			temp = next;
		}
	}
}

} //namespace CJCU
