/*
 * UCT_tree.h
 *
 * Created on: 2011/7/13
 * Author: chouhh
 */

#ifndef PARALLELWINGO_UCT_TREE_H_
#define PARALLELWINGO_UCT_TREE_H_

#include "config.h"

namespace CJCU {

class Node  // UCT樹中節點的結構
{
private:
	int wins; // 以此節點為root的子樹中，PlayRandomGame的總勝利次數
	int visits; // 此節點的總拜訪次數
	int level; // 此節點在UCT tree中的level，root為0
	int move; // 產生此節點盤面的著手
	int color; // move的顏色

	Node* BestNode; // 勝率最高的子節點
	Node* Child; //子節點的開頭，子節點間以Sibling連接
	Node* Sibling; //連接下一個兄弟節點的指標

public:
	Node();
	Node(int c, int l, int m); //由color、levels、move三項資料建立新node
	~Node();

	void delete_Tree(); //會遞迴呼叫此node下的所有node的DeleteNode function，以釋放子Tree的空間
	void create_Children(int *LegalMoves, int num, int c, int hn); //產生子點。第十手前不產生一線上子點，其餘皆產生全部可用點。
	void update_Wins(int win_result); //根據PlayRandomGame的結果以及此node的顏色來決定wins是否增加
	void set_BestNode(); //設定勝率最高的子點

	bool has_Child(); //傳回是否有子點
	bool has_Sibling(); //傳回是否有兄弟
	bool has_Visited(); //傳回是否拜訪過
	void add_Visits(); //拜訪次數加一

	int get_Move(); //回傳此點之move值
	int get_Level();
	int get_Color();
	int get_Visits();
	int get_Wins();

	Node* select_BestChild(); //根據UCB公式計算UCTValue，並回傳UCTValue最高之子點
	Node* get_Child(); //回傳子點指標
	Node* get_Sibling(); //回傳指向兄弟節點的指標
	Node* get_BestNode(); //回傳BestNode的指標
};

} //namespace CJCU

#endif /* PARALLELWINGO_UCT_TREE_H_ */
