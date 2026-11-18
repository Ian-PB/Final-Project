#include "Server.h"
#include <WinSock2.h>
#include <ws2def.h>
#include <iostream>

#include "NetworkObject.h"
#include "NetworkManager.h"

Server::~Server()
{
	closesocket(s);
	WSACleanup();
}

void Server::Init()
{
	struct sockaddr_in server;
	WSADATA wsa;

	slen = sizeof(si_other);

	// Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	// Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	// Set listening socket to NON-BLOCKING
	u_long mode = 1;
	ioctlsocket(s, FIONBIO, &mode);

	// Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	// Bind
	if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	SetServerIP();
}

void Server::Update()
{
	char buf[BUFLEN];
	struct sockaddr_in temp_si_other;
	int temp_slen = sizeof(temp_si_other);
	memset(buf, '\0', BUFLEN);

	int recv_len = SOCKET_ERROR;

	// Check for data
	while (true)
	{
		recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&temp_si_other, &slen);
		if (recv_len == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET)
			continue;

		break;
	}

	if (recv_len == SOCKET_ERROR)
		return;

	// Deserialise
	NetworkObject* obj = NetworkObject::Deserialise(buf, recv_len);
	if (obj == nullptr)
	{
		std::cout << "Object == nullptr" << std::endl;
		return;
	}
	else if (obj->GetIndex() < 0 || obj->GetIndex() >= NetworkManager::GetSize())
	{
		std::cout << "Object not in NetworkObjects vector - " << obj->GetIndex() << std::endl;
		return;
	}
}

void Server::SetServerIP()
{
	char hostname[256];
	gethostname(hostname, 256);
	struct hostent* hostInfo = gethostbyname(hostname);
	struct in_addr addr;
	memcpy(&addr, hostInfo->h_addr_list[0], sizeof(struct in_addr));
	serverIpAddress = inet_ntoa(addr);
}
