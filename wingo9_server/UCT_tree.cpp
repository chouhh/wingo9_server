/*
 * UCT_tree.cpp
 *
 * Created on: 2011/7/14
 * Author: chouhh
 */

#include <cmath>
#include <cstdlib>

#include "UCT_tree.h"
#include "config.h"

namespace CJCU
{

Node::Node()
{
  wins = 0;
  visits = 0;
  level = 0;
  move = 0;
  color = EMPTY;
  BestNode = NULL;
  Child = NULL;
  Sibling = NULL;
}

Node::Node(int c , int l , int m)
{
  color = c;
  level = l;
  move = m;
  wins = 0;
  visits = 0;
  BestNode = NULL;
  Child = NULL;
  Sibling = NULL;
}

Node::~Node()
{
}

//�ھ�LegalCandidate�H��LegalNum�ӨM�wChildren
//�l�I���Ͷ��Ǩ䭫�n�צb�����֪������ӧ�֦a���n�I
//�M�w�n�[�J���Ǥl�I�A�䭫�n�צb�󤣮��OPlayRandomGame�������b�L����n���l�I�W
//�Y�[�J���l�I�Ƥ�����L���W�i�U���I�ƮɡA�n�`�NPlaySimulation function���u�׽L�P�_����v
void Node::create_Children(int *LegalMoves , int num , int c , int hn )
{
    Node* next_node = NULL;

      for(int i = 0 ; i < num ; i++ )
      {
    	  // �@�u���Ҽ{
          if(hn <= Open_Step_Limit )
          {
            int j = LegalMoves[i]/BoardSize , k = LegalMoves[i]%BoardSize;
            if( j == 1 || j==RealBoardSize || k == 1 || k == RealBoardSize )
            {
                continue;
            }
          }

          // ���leaf���U�i
          if(Child == NULL)
          {
            Node* temp = new Node(c,level+1,LegalMoves[i]);
            Child = temp;
          }
          else
          {
        	next_node = Child;
            while( next_node->Sibling != NULL )
            {
            	next_node = next_node->Sibling;
            }

            Node* temp = new Node(c,level+1,LegalMoves[i]);

            next_node->Sibling = temp;
          }
      }
}

Node* Node::select_BestChild()
{
	float bestuct = 0, uct = 0, uctvalue = 0, winrate = 0;

	Node* result = NULL;
	Node* next = Child;

	while (next != NULL)
	{
		if (next->visits != 0) //�Y�w���X�L�A�h�ھ�UCB�����p��UCTValue
		{
			winrate = (float) next->wins / next->visits;
			uct = sqrt( (log(visits)) / ((float) ExEValue * next->visits) );

			uctvalue = winrate + uct;
		}
		else //�Y�������X�L�A�h�H����UCTValue
		{
			uctvalue = 1000 + rand() % 1000;
		}

		if (uctvalue > bestuct)
		{
			bestuct = uctvalue;
			result = next;
		}
		next = next->Sibling;
	}

	return result;
}

void Node::set_BestNode()
{
	float best_winrate = 0, temp = 0, winrate = 0;
	Node* next = Child;

	if (next != NULL)
	{
		while (next != NULL)
		{
			if (next->visits != 0)
			{
				winrate = (float) next->wins / next->visits;
				temp = winrate;
				if (temp >= best_winrate)
				{
					best_winrate = temp;
					BestNode = next;
				}
			}
			next = next->Sibling;
		}
	}
	else
	{
		BestNode = NULL;
	}
}

void Node::delete_Tree()
{
	if (Child != NULL)
	{
		Child->delete_Tree();
	}
	if (Sibling != NULL)
	{
		Sibling->delete_Tree();
	}

	delete this;
}

void Node::update_Wins(int win_result)
{
	if (color == win_result)
	{
		wins++;
	}
}

bool Node::has_Child()  //�Ǧ^�O�_���l�I
{
	return Child == NULL ? false : true;
}

bool Node::has_Sibling()
{
	return Sibling == NULL ? false : true;
}

bool Node::has_Visited() //�Ǧ^�O�_���X�L
{
	return visits == 0 ? false : true;
}

void Node::add_Visits() //���X���ƥ[�@
{
	visits++;
}

int Node::get_Move()
{
	return move;
}

int Node::get_Level()
{
	return level;
}

int Node::get_Color()
{
	return color;
}

int Node::get_Visits()
{
	return visits;
}

int Node::get_Wins()
{
	return wins;
}

Node* Node::get_Child()
{
	return Child;
}

Node* Node::get_Sibling()
{
	return Sibling;
}

Node* Node::get_BestNode()
{
	return BestNode;
}

} //namespace CJCU
