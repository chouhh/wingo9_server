/*
 * go_string.cpp
 *
 * Created on: 2011/7/13
 * Author: chouhh
 */

#include "config.h"
#include "go_string.h"

namespace CJCU
{

void GO_String::create_new_string(int s , int c , int id , int *sn )
{
  stone_num = 1;
  liberty = 4;
  color = c;
  start = s;
  end = s;
  str_id = id;
  sn[s] = id;
}

void GO_String::add_stone(int s , int *sn , int *next)
{
  stone_num++;
  next[end] = s;
  end = s;
  sn[s] = str_id;
}

void GO_String::set_liberty(int l, int lp)
{
  liberty = l;
  liberty_point = lp;
}

void GO_String::merge(GO_String *s , int *sn , int *next)
{
  stone_num += s->stone_num;
  next[end] = s->start;
  end = s->end;

  //將所有s的str_id均改成s的str_id
  int temp = s->start;
  while(temp) //temp != 0
  {
    sn[temp] = str_id;
    temp = next[temp] ;
  }

  s->del_all();
}

void GO_String::capture(int *sn , int *next)
{
  int temp = start , temp2 = start;
  while(temp != 0)
  {
    temp2 = next[temp];
    sn[temp] = 0;
    next[temp] = 0;
    temp = temp2;
  }
}

void GO_String::del_all()
{
	this->str_id = 0;
	this->stone_num = 0 ;
	this->liberty = 0;
	this->color = EMPTY;
	this->start = 0;
	this->end = 0 ;
	this->liberty_point = 0;
	this->next_str = 0 ;
	this->prev_str = 0;
}

void GO_String::clone(GO_String *s)
{
	this->str_id = s->str_id;
	this->stone_num = s->stone_num ;
	this->liberty = s->liberty;
	this->color = s->color ;
	this->start = s->start;
	this->end = s->end;
	this->liberty_point = s->liberty_point;
	this->next_str = s->next_str;
	this->prev_str = s->prev_str;
}

} //namespace CJCU
