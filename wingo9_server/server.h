/* ------------------------
 * server.h
 *
 * Created on: 2014/11/1
 * Author: sjean
 * ------------------------
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <winsock.h>
#include "config.h"

#define MAX_MESSAGE_SIZE 4096

int StartServerListening(unsigned short port);
void AcceptThread(int* serverSocket);
void initServer();
int sendBoard(int simnum, int handnum,int color,int* MSQ);
void closeWrongClient(unsigned int clientSocket);
void C_ArrToI_Arr(char* buffer,int* goal);
int bestMove(int* wins,int* visits);
int receiveResult(int handNum);
void EndServer(int socket);

#endif /* SERVER_H_ */
