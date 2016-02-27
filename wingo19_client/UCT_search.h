/*
 * UCT_search.h
 *
 * Created on: 2011/7/13
 * Author: chouhh
 */

#ifndef UCTCLIENT_0_UCT_SEARCH_H_
#define UCTCLIENT_0_UCT_SEARCH_H_

#include "config.h"
#include "board.h"
#include "UCT_tree.h"

namespace CJCU
{
int UCTSearch(int MSQ[M_Sequence_Limit], int hn,int *wins,int *visits);
int get_BestMove(Node *r);

void create_Tree(Node *r, Board *b, int hn);
void build_Tree(Node *r, Board *b);
void update_Tree(Node** Node_Sequence, int Max_Level, int win_color);
void play_Simulation(Board *b, Node *P_root);
void get_NodeSequence(Node *G_root, Board *b, Node** Node_Sequence, int *Max_Level);

extern Node* root;

} //namespace CJCU

#endif /* UCTCLIENT_0_UCT_SEARCH_H_ */
