#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<stdio.h>
#include<winsock2.h>
#include <string>
#include <vector>
#include <memory>

#define BUFLEN 512	// Max length of buffer
#define PORT 8888	// The port on which to listen for incoming data

class NetworkObject
{
public:
	virtual std::vector<char> Serialise(const std::shared_ptr<NetworkObject> p) = 0;
	virtual std::shared_ptr<NetworkObject> Deserialise(const char* t_data, size_t t_size) = 0;

	virtual void SendData() = 0;

	// Only 1 server ever
	static void SetServerIP(std::string t_serverIP) { serverIP = t_serverIP; }
	static std::string GetServerIP() { return serverIP; }

protected:
	addrinfo hints;
	// IP address of UDP server
	static std::string serverIP;
};

