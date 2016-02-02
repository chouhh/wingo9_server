/* ----------------------
 * wingo.cpp
 *
 * Created on: 2014/10/18
 * Modified on: 2015/11/30
 * Author: sjean
 * ----------------------
 */
#include <string>
#include <iostream>
#include <fstream>

#include "config.h"
#include "gtp_cmd.h"

using namespace CJCU;
using namespace std;

int initConfig()
{
	ifstream fin("setup.txt", ios::in);
	ofstream cmd_log("commands_log.txt",ios::app);
	string par_name;
	float par_val;

	//Åª¨ú°Ñ¼Æ
	if (!fin.is_open())
	{
		return false;
	}

	while (!fin.eof())
	{
		fin >> par_name;

		if (par_name == "MaxSimulation")
		{
			fin>> par_val;
			MaxSimulation = (int) par_val;
			Simulation_Limit = MaxSimulation;
			cmd_log << "MaxSimulation = " << MaxSimulation << endl;
		}
		else if (par_name == "Extension_Urgency")
		{
			fin>> par_val;
			Extension_Urgency = (int) par_val;
			cmd_log << "Extension_Urgency = " << Extension_Urgency << endl;
		}
		else if (par_name == "Capture_Urgency")
		{
			fin>> par_val;
			Capture_Urgency = (int) par_val;
			cmd_log << "Capture_Urgency = " << Capture_Urgency << endl;
		}
		else if (par_name == "Pattern_Urgency")
		{
			fin>> par_val;
			Pattern_Urgency = (int) par_val;
			cmd_log << "Pattern_Urgency=" << Pattern_Urgency << endl;
		}
		else if (par_name == "Distance_Urgency")
		{
			fin>> par_val;
			Distance_Urgency = (int) par_val;
			cmd_log << "Distance_Urgency = " << Distance_Urgency << endl;
		}
		else if (par_name == "ExEValue")
		{
			fin>> par_val;
			ExEValue = (int) par_val;
			cmd_log << "ExEValue = " << ExEValue << endl;
		}
		else if (par_name == "UCT_Komi")
		{
			fin>> par_val;
			UCT_Komi = par_val;
			cmd_log << "UCT_Komi = " << UCT_Komi << endl;
		}
		else if (par_name == "MiddleGame_Step_Limit")
		{
			fin>> par_val;
			MiddleGame_Step_Limit = (int)par_val;
			cmd_log << "MiddleGame_Step_Limit = " << MiddleGame_Step_Limit << endl;
		}
		else if (par_name == "EndGame_Step_Limit")
		{
			fin>> par_val;
			EndGame_Step_Limit = (int)par_val;
			cmd_log << "EndGame_Step_Limit = " << EndGame_Step_Limit << endl;
		}
		else if (par_name == "Open_Step_Limit")
		{
			fin>> par_val;
			Open_Step_Limit =(int)par_val;
			cmd_log << "Open_Step_Limit = " << Open_Step_Limit << endl;
		}
		else if (par_name == "Mode")
		{
			fin>> par_val;
			Mode =(int)par_val;
			cmd_log << "Mode=" << Mode << endl;
		}
	}

	fin.close();
	cmd_log.close();

	return true;
}

int main()
{
	initConfig();
	start_main_loop();

	return 0;
}
