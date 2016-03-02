/*
 * go_string.h
 *
 * Created on: 2011/7/13
 * Author: chouhh
 */

#ifndef PARALLELWINGO_GO_STRING_H_
#define PARALLELWINGO_GO_STRING_H_

namespace CJCU
{

class GO_String
{
public:
	int str_id ;        //�Ѧ�s��
	int stone_num ;     //�Ѥl�Ӽ�
	int liberty;        //���Ѧ��ơA�u��0 1 2 �T�ءA2�N��2��H�W
	int color ;         //���Ѧꪺ�C��
	int start;          //���Ѧꤧ�Ĥ@���Ѥl
	int end;            //���Ѧꤧ�̫�@���Ѥl
	int liberty_point;  //���Ѧꪺ�Y�Ӯ��I�A�Y��0��A�h���Ȭ��s�A�Y���@��A�h���ߤ@���I

	int next_str;       //�i�δѦ�list��@�ӴѦ�
	int prev_str;       //�i�δѦ�list�e�@�ӴѦ�

	void create_new_string(int s, int c, int id, int *sn); //�Ѥls�ۦ��@�s�Ѧ�A���Ѧ�s����id�Asn�����Ѥls���ݴѦ�s��
	void add_stone(int s, int *sn, int *next);  //�[�J���w�Ѥl��Ѧꤤ�A�Nend�]�����Ѥl�Anext���Ѥl��C
	void set_liberty(int l, int lp);    //��s�Ѧ��ơA�ë��wliberty_point
	void merge(GO_String *s, int *sn, int *next);  //�P���w���Ѧ�X��
	void capture(int *sn, int *next); //�NString_no�W�Ҧ��v���T�M��
	void del_all();     //�N�Ҧ����k�s
	void clone(GO_String *s); //�����ƻs���w���Ѧ�
};

} //namespace CJCU

#endif /* PARALLELWINGO_GO_STRING_H_ */
