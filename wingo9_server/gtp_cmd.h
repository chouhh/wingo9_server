/* ----------------------
 * gtp_cmd.h
 *
 * Created on: 2014/10/18
 * Author: chouhh
 * -----------------------
 */

#ifndef PARALLELWINGO_GTP_CMD_H_
#define PARALLELWINGO_GTP_CMD_H_

#include <string>

using namespace std;

typedef int (*gtp_fn_ptr)(string arg);

struct gtp_command
{
	const char *name;
	gtp_fn_ptr function;
};

void start_main_loop();

int GetCoordinate(string str);
void LowerCase(string& str);
int StrToInt(string& str);
void IntToStr(int num , string& str);
int GTP_Response(string str, int flag);
int find_nearest_joseki(int num_joseki, int *joseki_moves, int lastmove);

#endif /* PARALLELWINGO_GTP_CMD_H_ */
