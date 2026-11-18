#pragma once
#ifndef NOGDI
#define NOGDI
#endif

#ifndef NOUSER
#define NOUSER
#endif

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<stdio.h>
#include<winsock2.h>
#include <string>
#include <vector>
#include "NetworkObject.h"
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#undef PlaySound
#undef CloseWindow
#undef Rectangle
#undef ShowCursor

#define BUFLEN 512	// Max length of buffer
#define PORT 8888	// The port on which to listen for incoming data

struct ConnectedClient
{
	sockaddr_in address;
	int id;
};

class Server
{
public:
	~Server();
	void Init();

	void Update();
	std::string GetServerIp() { return serverIpAddress; }

private:
	void SendJoinAccepted(const sockaddr_in& t_address, int t_clientID);
	int FindClient(const sockaddr_in& t_address);
	void CreateNewClient(struct sockaddr_in t_sender, int& t_index);
	std::vector<ConnectedClient> clients;
	int nextClientID = 0;

	void SendObjectToClients(NetworkObject* t_object);

	SOCKET s;
	struct sockaddr_in si_other;
	int recv_len;
	int slen;
	char buffer[BUFLEN];

	void SetServerIP();
	std::string serverIpAddress = "NOT HERE YET";
};