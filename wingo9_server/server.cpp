/* ----------------------
 * server.cpp
 *
 * Created on: 2014/11/1
 * Author: sjean
 * ----------------------
 */
#include <winsock2.h>
#include <iostream>
#include <fstream>
#include <string>

#include "server.h"
#include "stdio.h"

using namespace std;

HANDLE Hthread; // Used to handle our threads
HANDLE Hmutex; // A mutex is basically a memory spot we can lock so nothing writes to it as we read form it..
FD_SET clientSet; // This basically is our list of sockets that we will lock and unlock without mutexHandle

ofstream serout("ParallelResult.txt", ios::app);

void initServer()
{
	int serverSocket = StartServerListening(5000);

	if (serverSocket == -1)
	{
		return;
	}

	Hmutex = CreateMutex(NULL, false, NULL); // Create mutex

	if (Hmutex == NULL) // Could not make mutex
	{
		EndServer(serverSocket);
		return;
	}

	int thread; // Create thread
	Hthread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) AcceptThread, &serverSocket, 0, (LPDWORD) &thread);

	if (Hthread == NULL) //Could not been accepted.
	{
		EndServer(serverSocket);
		return;
	}

	Sleep(100);

    serout << "����client�ݳs�u��..." << endl;

	for (;;)
	{
		WaitForSingleObject(Hmutex, INFINITE);  // Lock the mutex
		FD_SET TempSet = clientSet; // Make a temp location so we can unlock our mutex and let our accept thread run ASAP
		ReleaseMutex(Hmutex);        // Unlock the mutex

		if (TempSet.fd_count == 0)
		{
			continue;
		}

		timeval waitTime; // time interval
		waitTime.tv_sec = 0;
		waitTime.tv_usec = 0;

		//�ˬd�O�_��client�ݥ��b����
		int result = select(TempSet.fd_count, &TempSet, NULL, NULL, &waitTime); // Select a socket from TempSet

		if (result == 0) // Empty
		{
			continue;
		}

		if (result == SOCKET_ERROR)
		{
			serout << "Server: There was an error in the select() ..." << endl;
			continue;
		}
	}

	EndServer(serverSocket);
}

int StartServerListening(unsigned short port)
{
	int error;
	WSAData wsaData;

	if ((error = WSAStartup(MAKEWORD(2, 2), &wsaData)) == SOCKET_ERROR)
	{
		return -1;
	}

	int mySocket = socket(AF_INET, SOCK_STREAM, 0);

	if (mySocket == SOCKET_ERROR)
	{
		return -1;
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;

	if (bind(mySocket, (sockaddr*) &server, sizeof(server)) == SOCKET_ERROR)
	{
		closesocket(mySocket);
		return -1;
	}

	if (listen(mySocket, 63) == SOCKET_ERROR) // �̦h64�ӳs�u
	{
		closesocket(mySocket);
		return -1;
	}

	serout << "Server socket listening ready: " << mySocket << endl;

	return mySocket;
}

void AcceptThread(int* serverSocket)
{
	int mySocket = *serverSocket; // copy my socket
	int clientNum = 0;

	for (;;)
	{
		unsigned int clientSocket = accept(mySocket, 0, 0); // Do a normal accept

		if (clientSocket == SOCKET_ERROR)
		{
			return;
		}
		else
		{
			WaitForSingleObject(Hmutex, INFINITE); // Lock the mutex and wait for it

			char tid[6];
			sprintf(tid, "%d", clientNum);

			if ((send(clientSocket, tid, sizeof(tid), 0)) != SOCKET_ERROR)//����client�O�_����
			{
				FD_SET(clientSocket, &clientSet); // Add this socket to the client set using FD_SET()
				clientNum = clientSet.fd_count; //�ثe�s�u���ƶq
				serout << "There are " << clientNum << " clients into Server " << endl;
				serout << "id[" << clientNum - 1 << "]=" << clientSet.fd_array[clientNum - 1] << " connected." << endl;
			}
			else
			{
				serout <<"Connection Error!"<<endl;
			}

			ReleaseMutex(Hmutex); // Unlock the mutex
		}
	}
}

void closeWrongClient(unsigned int clientSocket)
{
	WaitForSingleObject(Hmutex, INFINITE);
	FD_CLR(clientSocket, &clientSet);
	ReleaseMutex(Hmutex);
	closesocket (clientSocket);
	serout << "Server: Client on " << clientSocket << " has disconnected." << endl; // Tell everyone the person left
	serout << "��Ʊq " << clientSocket << " �����ɥ���!\n " << endl;
}

void EndServer(int socket)
{
	serout << "Shut down the server..." << endl;
	WaitForSingleObject(Hthread, INFINITE);		// Kill thread and handle
	CloseHandle(Hthread);
	CloseHandle(Hmutex);
	closesocket(socket);
	WSACleanup();
}

//���~�_���������٨S��

int sendBoard(int simnum, int handnum, int color, int *MSQ)
{
	serout << "�� " << handnum << " ��, �C�⬰ " << color << " �C" << endl;
	WaitForSingleObject(Hmutex, INFINITE); // Lock the mutex
	FD_SET TempSet = clientSet;	// Get the masterSet of now
	ReleaseMutex(Hmutex); // Unlock the mutex

	unsigned int clientSocket;
	int nBytes;
	char buffer[MAX_MESSAGE_SIZE] = "";

	if(TempSet.fd_count == 0)// �Y�L client�A�h�^�� -1
	{
		return -1;
	}

	for (unsigned int i=0; i<TempSet.fd_count; i++) // �v�@�ǰe���C�@��client�A�]�thand number, color, MSQ
	{
		clientSocket = TempSet.fd_array[i];
		nBytes = -1;

		/* �ǰesimulation number */
		memset(buffer, 0, sizeof(buffer));	//�M��buffer, �令0�άO�����A�]��sprintf(buffer, "%d", simnum);�i��w�g�O�s����m�F
		sprintf(buffer, "%d", simnum);

		if ((nBytes = send(clientSocket, buffer, 6, 0)) == SOCKET_ERROR)// buffer���׭n��client�����n
		{
			int error = WSAGetLastError();
			if (error == WSAECONNRESET)
			{
				closeWrongClient(clientSocket);
				serout << "Send simulation number error!" << error << endl;
			}
			continue;
		}

		/* �ǰehand number */
		memset(buffer, 0, sizeof(buffer));	//�M��buffer, �令0�άO�����A�]��sprintf(buffer, "%d", handnum);�i��w�g�O�s����m�F
		sprintf(buffer, "%d", handnum);

		if ((nBytes = send(clientSocket, buffer, 6, 0)) == SOCKET_ERROR)// buffer���׭n��client�����n
		{
			int error = WSAGetLastError();
			if (error == WSAECONNRESET)
			{
				closeWrongClient(clientSocket);
				serout << "Send hand number error!" << error << endl;
			}
			continue;
		}

		/* �ǰecolor */
		memset(buffer, 0, sizeof(buffer));		//�M��buffer
		sprintf(buffer, "%d", color);
		nBytes = -1;

		if ((nBytes = send(clientSocket, buffer, 6, 0)) == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			if (error == WSAECONNRESET)
			{
				closeWrongClient(clientSocket);
				serout << "Send color error!" << error << endl;
			}
			continue;
		}

		/* �ǰeMSQ */
		memset(buffer, 0, sizeof(buffer));		//�M��buffer
		char temp[6] = "";

		for (int index = 0; index < M_Sequence_Limit; index++)
		{
			sprintf(temp, "%d", MSQ[index]);
			strcat(buffer, temp);
			strcat(buffer, ",");
			memset(temp, 0, sizeof(temp));
		}

		nBytes = -1;
		if ((nBytes = send(clientSocket, buffer, MAX_BUF, 0)) == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			if (error == WSAECONNRESET)
			{
				closeWrongClient(clientSocket);
				serout << "Send MSQ error!" << error << endl;
			}
			continue;
		}
	}

	serout << "Send to Client Finished!" << endl;

	return 1;
}

int receiveResult(int handNum)
{
	ofstream record("record.txt", ios::app);

	WaitForSingleObject(Hmutex, INFINITE); // Lock the mutex
	FD_SET TempSet = clientSet;
	ReleaseMutex(Hmutex); // Unlock the mutex

	unsigned int clientSocket;
	char buffer[MAX_MESSAGE_SIZE] = "";
	int wins[BoardArraySize] = { 0 };
	int visits[BoardArraySize] = { 0 };
	//memset(wins, 0, sizeof(wins));
	//memset(visits, 0, sizeof(visits));

	int nBytes = -1;
	int num_live_client = 0;

	record << "�� " << handNum << " ��....." << endl;

	for (unsigned int i=0; i<TempSet.fd_count; i++) // �v�@�����C�@��client�^�Ǫ����G�A�]�twins and visits
	{
		clientSocket = TempSet.fd_array[i];

		int tempWins[BoardArraySize] = { 0 };
		int tempVisits[BoardArraySize] = { 0 };
		//memset(tempWins, 0, sizeof(tempWins));
		//memset(tempVisits, 0, sizeof(tempVisits));
		memset(buffer,0,sizeof(buffer));

		serout << "������ " << i << " ��client�����..." << endl;

		/* �����ӧQ���� */
		while (buffer[0] == '\0')
		{
			if ((nBytes = recv(clientSocket, buffer, MAX_BUF, 0)) == SOCKET_ERROR)
			{
				int error = WSAGetLastError();
				serout << "Receive wins error!" << error << endl;
				closeWrongClient(clientSocket);

				break;
			}

			if (buffer[0] != '\0')
			{
				send(clientSocket, "WinsOK", 10, 0);
			}
			else
			{
				send(clientSocket, "AGAIN", 10, 0);
			}
		}

		record << "��" << i << "��Client�G" << endl;
		C_ArrToI_Arr(buffer, tempWins); //(char*)buffer to (int*)wins
		record << "Wins�G" << endl;

		for (int index = 0; index < BoardArraySize; index++)
		{
			record << tempWins[index] << ";";
		}

		record << endl;

		/* �������X���� */
		memset(buffer, 0, sizeof(buffer));		//�M��buffer
		while (buffer[0] == '\0')
		{
			if ((nBytes = recv(clientSocket, buffer, MAX_BUF, 0)) == SOCKET_ERROR)
			{
				int error = WSAGetLastError();
				serout << "Receive visits error!" << error << endl;
				closeWrongClient(clientSocket);

				break;
			}

			if (buffer[0] != '\0')
			{
				nBytes = send(clientSocket, "VisitsOK", 10, 0);
			}
			else
			{
				send(clientSocket, "AGAIN", 10, 0);
			}
		}

		C_ArrToI_Arr(buffer, tempVisits);		//(char*)buffer to (int*)visits
		record << "Visits�G\n";
		for (int index = 0; index < BoardArraySize; index++)
		{
			record << tempVisits[index] << ";";
		}
		record << endl;

		/* wins,visits �[�` */
		memset(buffer, 0, sizeof(buffer));		//�M��buffer
		for (int index = 0; index < BoardArraySize; index++)
		{
			wins[index] += tempWins[index];
			visits[index] += tempVisits[index];
		}

		num_live_client++;
	}

	WaitForSingleObject(Hmutex, INFINITE);        // Lock the mutex
	TempSet = clientSet;
	ReleaseMutex(Hmutex);        // Unlock the mutex

	serout << "There are " << num_live_client << " clients return the results�C" << endl;
	serout << "And " << TempSet.fd_count << " clients are still alive." << endl;

	return bestMove(wins, visits);
}

int bestMove(int* wins, int* visits)
{
	ofstream winratOut("data.txt",ios::trunc);
	float bestWinrate = 0.0;
	int bestMove = 0;
	char data[4096]="";//�g�X�Ӳv��greenfoot�ʱ�

	winratOut << "decide best move" << endl;

	for (int i=0; i<BoardArraySize; i++)
	{
		if(visits[i] <= 0)
		{
			strcat(data,"0,");
			continue;
		}

		winratOut << "i:" << i << "; visits[i]:" << visits[i] << endl;

		float tempWinrate = (float) wins[i] / visits[i];
		char tmpWinrate[10] = "";//temp�Ӳv��char���A
		sprintf(tmpWinrate, "%f", tempWinrate);
		strcat(data,tmpWinrate);
		strcat(data,",");

		if(tempWinrate >= bestWinrate)
		{
			bestWinrate = tempWinrate;
			bestMove=i;
		}
	}

	serout << endl << "�̨Φ�m��" << bestMove << endl << "���X���Ƭ��G"
			<< visits[bestMove] << "\n �ӧQ���Ƭ��G" << wins[bestMove] << endl
			<< " �Ӳv�G" << bestWinrate << endl << endl;
	winratOut << data <<endl;
	winratOut.close();//�n�����~�|�Q�U�@������л\;

	if (bestWinrate < 0.2)
	{
		return -1;
	}
	else
	{
		return bestMove;
	}
}

void C_ArrToI_Arr(char* buffer,int* goal)
{
	int index = 0, tIndex = 0, mIndex = 0;
	char temp[14] = "";

	while (buffer[index] !='\0')
	{
		if (buffer[index] == ',')
		{
			goal[mIndex] = atoi(temp);
			memset(temp, 0, sizeof(temp));
			mIndex++;
			tIndex = 0;
		}
		else
		{
			temp[tIndex] = buffer[index];
			tIndex++;
		}

		index++;
	}
}
