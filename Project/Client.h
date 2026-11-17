#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")
#include<winsock2.h>
#include <string>
#include <vector>
#include <memory>
#include <ws2def.h>

class NetworkObject;

#define BUFLEN 512	// Max length of buffer
#define PORT 8888	// The port on which to listen for incoming data

class Client
{
public:
	~Client();
	void Init();

	void SendObject(std::shared_ptr<NetworkObject> t_object);

	// Only 1 server ever
	static void SetServerIP(std::string t_serverIP) { serverIP = t_serverIP; }
	static std::string GetServerIP() { return serverIP; }

private:
	addrinfo hints;

	// IP address of UDP server
	static std::string serverIP;

	static struct sockaddr_in si_other;
	static int s;
	static int slen;
	static char buffer[BUFLEN];

	WSADATA wsa;
};

#include "NetworkObject.h"