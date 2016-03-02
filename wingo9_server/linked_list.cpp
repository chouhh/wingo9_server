/*
 * linked_list.cpp
 *
 * Created on: 2011/7/14
 * Author: chouhh
 */

#include <cstdlib>

#include "config.h"
#include "linked_list.h"

namespace CJCU {

INT_Node::INT_Node()
{
	value = 0;
	next = NULL;
}

INT_LinkedList::INT_LinkedList()
{
	head = NULL;

	for (int i = 0; i < MAXSTRNUM; ++i)
	{
		node_array[i].value = i;
	}
}

INT_LinkedList::~INT_LinkedList()
{
}

void INT_LinkedList::insert(int v)
{
	INT_Node *temp = head;

	if (temp == NULL) //�Yhead == NULL�A�N���ꬰ��
	{
		head = &node_array[v];
		head->next = NULL;
		return;
	}

	if (v < temp->value) //�Y�����J���Ȥp��ثe�}�Y(�̤p)���ȡA�N���ȭn���bLinked List���}�Y����
	{
		INT_Node *n = &node_array[v];
		n->next = head;
		head = n;
		return;
	}

	//�Y���ݩ�W�z�G�ت��p�A�N��n�M��A���m���J
	//�Y�۵��A�h���δ��J�A���Y�J���ثe���Ȥj�B���@�Ӧ�m���Ȥp�A�h���J�󤤶��A
	//�Y����ݪ��Ȥj�A�h���J�����

	int k = 0;
	while (temp != NULL && k < 121)
	{
		k++;
		if (v > temp->value)
		{
			if (temp->next != NULL)
			{
				if (v < temp->next->value) //���J����
				{
					INT_Node *n = &node_array[v];
					n->next = temp->next;
					temp->next = n;
					return;
				}
				else //���ŦX���󱡪p�h�~�򩹫ᨫ
				{
					temp = temp->next;
				}
			}
			else //���J����
			{
				INT_Node *n = &node_array[v];
				n->next = NULL;
				temp->next = n;
				return;
			}
		}
		else if (v == temp->value) //�۵��h��������B�z
		{
			return;
		}
	}
}

INT_Node* INT_LinkedList::get_head()
{
	return head;
}

void INT_LinkedList::clear()
{
	head = NULL;
}

} //namespace CJCU
