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

class Node  // UCT�𤤸`�I�����c
{
private:
	int wins; // �H���`�I��root���l�𤤡APlayRandomGame���`�ӧQ����
	int visits; // ���`�I���`���X����
	int level; // ���`�I�bUCT tree����level�Aroot��0
	int move; // ���ͦ��`�I�L�����ۤ�
	int color; // move���C��

	Node* BestNode; // �Ӳv�̰����l�`�I
	Node* Child; //�l�`�I���}�Y�A�l�`�I���HSibling�s��
	Node* Sibling; //�s���U�@�ӥS�̸`�I������

public:
	Node();
	Node(int c, int l, int m); //��color�Blevels�Bmove�T����ƫإ߷snode
	~Node();

	void delete_Tree(); //�|���j�I�s��node�U���Ҧ�node��DeleteNode function�A�H����lTree���Ŷ�
	void create_Children(int *LegalMoves, int num, int c, int hn); //���ͤl�I�C�ĤQ��e�����ͤ@�u�W�l�I�A��l�Ҳ��ͥ����i���I�C
	void update_Wins(int win_result); //�ھ�PlayRandomGame�����G�H�Φ�node���C��ӨM�wwins�O�_�W�[
	void set_BestNode(); //�]�w�Ӳv�̰����l�I

	bool has_Child(); //�Ǧ^�O�_���l�I
	bool has_Sibling(); //�Ǧ^�O�_���S��
	bool has_Visited(); //�Ǧ^�O�_���X�L
	void add_Visits(); //���X���ƥ[�@

	int get_Move(); //�^�Ǧ��I��move��
	int get_Level();
	int get_Color();
	int get_Visits();
	int get_Wins();

	Node* select_BestChild(); //�ھ�UCB�����p��UCTValue�A�æ^��UCTValue�̰����l�I
	Node* get_Child(); //�^�Ǥl�I����
	Node* get_Sibling(); //�^�ǫ��V�S�̸`�I������
	Node* get_BestNode(); //�^��BestNode������
};

} //namespace CJCU

#endif /* PARALLELWINGO_UCT_TREE_H_ */
