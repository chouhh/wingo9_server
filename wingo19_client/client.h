/* ------------------------
 * client.h
 *
 *  Created on: 2014/11/1
 *  Author: sjean
 *  -----------------------
 */

#ifndef UCTCLIENT_0_CLIENT_H_
#define UCTCLIENT_0_CLIENT_H_

#include "config.h"
#define MAX_MESSAGE_SIZE 4096

int StartClient(unsigned short port, const char* serverName);
void EndClient(int clientSocket);
int initClient(const char* ip,int tid);
int recvBoard(int socket,int * sn, int* hn,int* c,int MSQ[M_Sequence_Limit]);
void clearArray(char* a,int len);
int sendResult(int socket,int* wins,int* visits);

#endif /* UCTCLIENT_0_CLIENT_H_ */
