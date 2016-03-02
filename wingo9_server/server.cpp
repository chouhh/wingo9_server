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

    serout << "等待client端連線中..." << endl;

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

		//檢查是否有client端正在活動
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

	if (listen(mySocket, 63) == SOCKET_ERROR) // 最多64個連線
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

			if ((send(clientSocket, tid, sizeof(tid), 0)) != SOCKET_ERROR)//測試client是否活著
			{
				FD_SET(clientSocket, &clientSet); // Add this socket to the client set using FD_SET()
				clientNum = clientSet.fd_count; //目前連線的數量
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
	serout << "資料從 " << clientSocket << " 接收時失敗!\n " << endl;
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

//中途斷掉的機制還沒做

int sendBoard(int simnum, int handnum, int color, int *MSQ)
{
	serout << "第 " << handnum << " 手, 顏色為 " << color << " 。" << endl;
	WaitForSingleObject(Hmutex, INFINITE); // Lock the mutex
	FD_SET TempSet = clientSet;	// Get the masterSet of now
	ReleaseMutex(Hmutex); // Unlock the mutex

	unsigned int clientSocket;
	int nBytes;
	char buffer[MAX_MESSAGE_SIZE] = "";

	if(TempSet.fd_count == 0)// 若無 client，則回報 -1
	{
		return -1;
	}

	for (unsigned int i=0; i<TempSet.fd_count; i++) // 逐一傳送給每一個client，包含hand number, color, MSQ
	{
		clientSocket = TempSet.fd_array[i];
		nBytes = -1;

		/* 傳送simulation number */
		memset(buffer, 0, sizeof(buffer));	//清空buffer, 改成0或是拿掉，因為sprintf(buffer, "%d", simnum);可能已經是新的位置了
		sprintf(buffer, "%d", simnum);

		if ((nBytes = send(clientSocket, buffer, 6, 0)) == SOCKET_ERROR)// buffer長度要跟client對應好
		{
			int error = WSAGetLastError();
			if (error == WSAECONNRESET)
			{
				closeWrongClient(clientSocket);
				serout << "Send simulation number error!" << error << endl;
			}
			continue;
		}

		/* 傳送hand number */
		memset(buffer, 0, sizeof(buffer));	//清空buffer, 改成0或是拿掉，因為sprintf(buffer, "%d", handnum);可能已經是新的位置了
		sprintf(buffer, "%d", handnum);

		if ((nBytes = send(clientSocket, buffer, 6, 0)) == SOCKET_ERROR)// buffer長度要跟client對應好
		{
			int error = WSAGetLastError();
			if (error == WSAECONNRESET)
			{
				closeWrongClient(clientSocket);
				serout << "Send hand number error!" << error << endl;
			}
			continue;
		}

		/* 傳送color */
		memset(buffer, 0, sizeof(buffer));		//清空buffer
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

		/* 傳送MSQ */
		memset(buffer, 0, sizeof(buffer));		//清空buffer
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

	record << "第 " << handNum << " 手....." << endl;

	for (unsigned int i=0; i<TempSet.fd_count; i++) // 逐一接收每一個client回傳的結果，包含wins and visits
	{
		clientSocket = TempSet.fd_array[i];

		int tempWins[BoardArraySize] = { 0 };
		int tempVisits[BoardArraySize] = { 0 };
		//memset(tempWins, 0, sizeof(tempWins));
		//memset(tempVisits, 0, sizeof(tempVisits));
		memset(buffer,0,sizeof(buffer));

		serout << "接收第 " << i << " 個client的資料..." << endl;

		/* 接收勝利次數 */
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

		record << "第" << i << "個Client：" << endl;
		C_ArrToI_Arr(buffer, tempWins); //(char*)buffer to (int*)wins
		record << "Wins：" << endl;

		for (int index = 0; index < BoardArraySize; index++)
		{
			record << tempWins[index] << ";";
		}

		record << endl;

		/* 接收拜訪次數 */
		memset(buffer, 0, sizeof(buffer));		//清空buffer
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
		record << "Visits：\n";
		for (int index = 0; index < BoardArraySize; index++)
		{
			record << tempVisits[index] << ";";
		}
		record << endl;

		/* wins,visits 加總 */
		memset(buffer, 0, sizeof(buffer));		//清空buffer
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

	serout << "There are " << num_live_client << " clients return the results。" << endl;
	serout << "And " << TempSet.fd_count << " clients are still alive." << endl;

	return bestMove(wins, visits);
}

int bestMove(int* wins, int* visits)
{
	ofstream winratOut("data.txt",ios::trunc);
	float bestWinrate = 0.0;
	int bestMove = 0;
	char data[4096]="";//寫出勝率給greenfoot監控

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
		char tmpWinrate[10] = "";//temp勝率的char型態
		sprintf(tmpWinrate, "%f", tempWinrate);
		strcat(data,tmpWinrate);
		strcat(data,",");

		if(tempWinrate >= bestWinrate)
		{
			bestWinrate = tempWinrate;
			bestMove=i;
		}
	}

	serout << endl << "最佳位置為" << bestMove << endl << "拜訪次數為："
			<< visits[bestMove] << "\n 勝利次數為：" << wins[bestMove] << endl
			<< " 勝率：" << bestWinrate << endl << endl;
	winratOut << data <<endl;
	winratOut.close();//要關閉才會被下一筆資料覆蓋;

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
