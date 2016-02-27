/* ----------------------
 * client.cpp
 *
 * Created on: 2014/11/1
 * Author: sjean
 * ----------------------
 */
#include <iostream>
#include <fstream>

#include <stdio.h>
#include <winsock2.h>

#include "client.h"
#include "config.h"

using namespace std;

ofstream client_log;
bool connectSuccess = false;

int initClient(const char* ip,int tid)
{
	cout << "Initial client socket:" << endl;

	cout << "1. Get my socket~" << endl;
	int mySocket;
	mySocket = StartClient(5000, ip);

	cout << "2. Hand shaking with server~~" << endl;
	char buffer[6];
	int bytes = recv(mySocket, buffer, 6, 0);

	if (strcmp(buffer, "check") == 0)
	{
		cout << "Connect success!" << std::endl;
	}

	if (bytes == SOCKET_ERROR || mySocket == -1)
	{
		EndClient(mySocket);
		cout << "Close client socket!" << endl;
		client_log.close();

		return -1;
	}
	else
	{
		string log_name="client_log";
		log_name.append(buffer);
		log_name.append(".txt");
		client_log.open(log_name.c_str(),ios::out);

		tid = atoi(&buffer[0]);
		client_log << "tid = " << tid << endl;
		connectSuccess = true;
	}

	return mySocket;
}

int recvBoard(int socket, int * sn, int* hn, int* c, int MSQ[M_Sequence_Limit])
{
	int nBytes;
	char buffer[MAX_MESSAGE_SIZE];

	client_log << "Receiving... " << endl;

	/* 接收simulation number */
	memset(buffer, 0, sizeof(buffer));
	if ((nBytes = recv(socket, buffer, 6, 0)) == SOCKET_ERROR)
	{
		client_log << "Simulation Number Receive Failed !" << endl;
		return -1;
	}

	*sn = atoi(&buffer[0]);
	client_log << "Simulation Number =" << *sn << endl;

	/* 接收hand number */
	memset(buffer, 0, sizeof(buffer));
	if ((nBytes = recv(socket, buffer, 6, 0)) == SOCKET_ERROR)
	{
		client_log << "Hand Number Receive Failed !" << endl;
		return -1;
	}

	*hn = atoi(&buffer[0]);
	client_log << "Hand Number =" << *hn << endl;


	/* 接收COLOR */
	memset(buffer, 0, sizeof(buffer));
	if ((nBytes = recv(socket, buffer, 6, 0)) == SOCKET_ERROR)
	{
		client_log << "Color Receive Failed!" << endl;
		return -1;
	}

	*c = atoi(&buffer[0]);
	client_log << "Color =" << *c << endl;

	/* 接收MSQ */
	memset(buffer, 0, sizeof(buffer));
	if ((nBytes = recv(socket, buffer, MAX_BUF, 0)) == SOCKET_ERROR)
	{
		client_log << "MSQ Receive Failed !\n";
		return -1;
	}

	/* buffer to MSQ */
	int index=0, tIndex=0, mIndex=0;
	char temp[6]="";

	while(buffer[index]!='\0')
	{
		if(buffer[index]==',')
		{
			MSQ[mIndex]=atoi(temp);
			memset(temp, 0, sizeof(temp));
			mIndex++;
			tIndex=0;
		}
		else
		{
			temp[tIndex]=buffer[index];
			tIndex++;
		}

		index++;
	}

	return 0;
}

int sendResult(int socket,int* wins,int* visits)
{
	int nBytes;
	char buffer[MAX_MESSAGE_SIZE] = "", temp[10] = "";

	client_log << "\nWINS:" << endl;
	for (int index=0; index<BoardArraySize; index++)
	{
		client_log << wins[index] << ",";
	}
	client_log<< endl;

	client_log << "VISITS:" << endl;
	for (int index=0; index<BoardArraySize; index++)
	{
		client_log << visits[index] << ",";
	}
	client_log << endl;
	client_log << "OK" << endl;

	/* send wins */
	for (int index=0; index<BoardArraySize; index++)
	{
		sprintf(temp, "%d", wins[index]);
		strcat(buffer, temp);
		strcat(buffer, ",");
		memset(temp, 0, sizeof(temp));
	}

	while(true)
	{
		if ((nBytes = send(socket, buffer, MAX_BUF, 0)) == SOCKET_ERROR)
		{
			client_log << " Wins Receive Failed !" << endl;
			return -1;
		}

		char tempBuff[10] = "";

		if ((nBytes = recv(socket, tempBuff, 10, 0)) == SOCKET_ERROR)
		{
			client_log << " Wins Receive Checker is NO Answer !" << endl;
			return -1;
		}

		client_log<<tempBuff<<endl;

		if (strcmp(tempBuff,"WinsOK") == 0)
		{
			client_log << " Wins Receive Success !" << endl << endl;
			break;
		}
		else
		{
			client_log << "In Wins phase got the following message : " << tempBuff << endl;
		}
	}

	/* send visits */
	memset(buffer, 0, sizeof(buffer));
	memset(temp, 0, sizeof(temp));

	for (int index=0; index<BoardArraySize; index++)
	{
		sprintf(temp, "%d", visits[index]);
		strcat(buffer, temp);
		strcat(buffer, ",");
		memset(temp, 0, sizeof(temp));
	}

	while (true)
	{
		if ((nBytes = send(socket, buffer, MAX_BUF, 0)) == SOCKET_ERROR)
		{
			client_log << " Visits Receive Failed !\n";
			return -1;
		}

		char tempBuff[10]="";

		if ((nBytes = recv(socket, tempBuff, 10, 0)) == SOCKET_ERROR)
		{
			client_log << " Visits Receive Checker is NO Answer !\n";
			return -1;
		}

		client_log<<tempBuff<<endl;

		if (strcmp(tempBuff,"VisitsOK") == 0)
		{
			client_log << " Visits Receive Success !\n\n";
			break;
		}
		else
		{
			client_log << "In Visits phase got the following message : "<< tempBuff << endl;
		}
	}

	client_log << "Results have sent!" << endl << endl;
	memset(buffer, 0, sizeof(buffer));
	memset(temp, 0, sizeof(temp));

	return 0;
}

int StartClient(unsigned short port, const char* serverName)
{
	int error;
	WSAData wsaData;

	if ((error = WSAStartup(MAKEWORD(2, 2), &wsaData)) == SOCKET_ERROR)
	{
		cout << "Client: socket error!\n";
		return -1;
	}

	int mySocket = socket(AF_INET, SOCK_STREAM, 0);

	if (mySocket == SOCKET_ERROR)
	{
		cout << "Client: socket cannot open!" << endl;
		return -1;
	}

	struct hostent *host_entry;
	if ((host_entry = gethostbyname(serverName)) == NULL)
	{
		cout << "Client: I'm lost!" << endl;
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = *(unsigned long*) host_entry->h_addr;

	if (connect(mySocket, (sockaddr*) &server, sizeof(server)) == SOCKET_ERROR)
	{
		cout << "Client: ****ing server wont let me connect dude.\n";
	}

	cout << "Client: I-AM-RUNNING!" << endl;

	return mySocket;
}

void EndClient(int clientSocket)
{
	closesocket(clientSocket);
	WSACleanup();
	cout<< "Client:  shutting down!" << endl;
	if(connectSuccess)
	{
		client_log.close();
	}
}
