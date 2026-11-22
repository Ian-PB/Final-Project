#pragma once

#ifndef NOGDI
#define NOGDI
#endif

#ifndef NOUSER
#define NOUSER
#endif

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")
#include<winsock2.h>
#include <string>
#include <vector>
#include <memory>
#include <ws2def.h>

#undef PlaySound
#undef CloseWindow
#undef Rectangle
#undef ShowCursor

class NetworkObject;
class World;

#define BUFLEN 512	// Max length of buffer
#define PORT 8888	// The port on which to listen for incoming data

enum class ConnectionType
{
	Waiting,
	Connected,
	TookTooLong
};

class Client
{
public:
	Client(World& t_world);
	~Client();
	void JoinServer(std::string t_serverIP);
	void SendJoinMsg();

	void Update();
	void SendObject(NetworkObject* t_object);

	// Only 1 server ever
	static void SetServerIP(std::string t_serverIP) { serverIP = t_serverIP; }
	static std::string GetServerIP() { return serverIP; }

	int GetClientsObjs() const { return clientID + 1; }
	int GetClientID() const { return clientID; }

	int GetNewPlayersSize() { return newPlayers.size(); }
	std::vector<int> GetNewPlayers() { return newPlayers; }
	void ClearNewPlayers() { newPlayers.clear(); }

	bool HasJoined() const { return joined; }
	ConnectionType JoiningTimer();

private:
	void AcceptedJoin(int recv_len, char buf[]);

	// Join waiting
	float joinTimer = 0.0f;
	static const int JOINING_WAIT_DUR = 30;

	World& world;
	std::vector<int> newPlayers;

	int clientID = -1;
	bool joined = false;

	// IP address of UDP server
	static std::string serverIP;

	addrinfo hints;
	struct sockaddr_in si_other;
	int s;
	int slen;
	char buffer[BUFLEN];

	WSADATA wsa;
};

#include "NetworkObject.h"
#include "World.h"