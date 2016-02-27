/*
 * go_string.h
 *
 * Created on: 2011/7/13
 * Author: chouhh
 */

#ifndef UCTCLIENT_0_GO_STRING_H_
#define UCTCLIENT_0_GO_STRING_H_

namespace CJCU
{

class GO_String
{
public:
	int str_id ;        //棋串編號
	int stone_num ;     //棋子個數
	int liberty;        //此棋串氣數，只有0 1 2 三種，2代表2氣以上
	int color ;         //此棋串的顏色
	int start;          //此棋串之第一顆棋子
	int end;            //此棋串之最後一顆棋子
	int liberty_point;  //此棋串的某個氣點，若為0氣，則此值為零，若為一氣，則為唯一氣點

	int next_str;       //可用棋串list後一個棋串
	int prev_str;       //可用棋串list前一個棋串

	void create_new_string(int s, int c, int id, int *sn); //棋子s自成一新棋串，此棋串編號為id，sn紀錄棋子s所屬棋串編號
	void add_stone(int s, int *sn, int *next);  //加入指定棋子於棋串中，將end設成此棋子，next為棋子串列
	void set_liberty(int l, int lp);    //更新棋串氣數，並指定liberty_point
	void merge(GO_String *s, int *sn, int *next);  //與指定的棋串合併
	void capture(int *sn, int *next); //將String_no上所有己串資訊清除
	void del_all();     //將所有值歸零
	void clone(GO_String *s); //完全複製指定的棋串
};

} //namespace CJCU

#endif /* UCTCLIENT_0_GO_STRING_H_ */
