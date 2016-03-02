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

	if (temp == NULL) //若head == NULL，代表此串為空
	{
		head = &node_array[v];
		head->next = NULL;
		return;
	}

	if (v < temp->value) //若欲插入的值小於目前開頭(最小)的值，代表此值要插在Linked List的開頭部份
	{
		INT_Node *n = &node_array[v];
		n->next = head;
		head = n;
		return;
	}

	//若不屬於上述二種狀況，代表要尋找適當位置插入
	//若相等，則不用插入，但若遇到比目前的值大且比後一個位置的值小，則插入於中間，
	//若比尾端的值大，則插入於尾端

	int k = 0;
	while (temp != NULL && k < 121)
	{
		k++;
		if (v > temp->value)
		{
			if (temp->next != NULL)
			{
				if (v < temp->next->value) //插入中間
				{
					INT_Node *n = &node_array[v];
					n->next = temp->next;
					temp->next = n;
					return;
				}
				else //不符合任何情況則繼續往後走
				{
					temp = temp->next;
				}
			}
			else //插入尾端
			{
				INT_Node *n = &node_array[v];
				n->next = NULL;
				temp->next = n;
				return;
			}
		}
		else if (v == temp->value) //相等則不做任何處理
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
