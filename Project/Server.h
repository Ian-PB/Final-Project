#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<stdio.h>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 512	// Max length of buffer
#define PORT 8888	// The port on which to listen for incoming data


class Server
{
public:
	~Server();
	void Init();

	void ListenForData();

private:
	SOCKET s;
	struct sockaddr_in server;
	struct sockaddr_in si_other;
	int recv_len;
	int slen;
	char buffer[BUFLEN];
	WSADATA wsa;
};