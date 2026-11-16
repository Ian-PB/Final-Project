#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")
#include<winsock2.h>
#include <string>
#include <vector>
#include <memory>
#include <ws2def.h>

#define BUFLEN 512	// Max length of buffer
#define PORT 8888	// The port on which to listen for incoming data

class NetworkObject
{
public:
	NetworkObject();
	~NetworkObject();

	void Init();

	virtual std::vector<char> Serialise(const NetworkObject* t_packet) = 0;
	virtual std::shared_ptr<NetworkObject> Deserialise(const char* t_data, size_t t_size) = 0;

	void SendObject();

	// Only 1 server ever
	static void SetServerIP(std::string t_serverIP) { serverIP = t_serverIP; }
	static std::string GetServerIP() { return serverIP; }

protected:
	addrinfo hints;

	// IP address of UDP server
	static std::string serverIP;

	struct sockaddr_in si_other;
	int s;
	int slen;
	char buffer[BUFLEN];

	WSADATA wsa;
};

