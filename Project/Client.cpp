#include "Client.h"
#include "NetworkManager.h"
#include <iostream>

std::string Client::serverIP = "192.168.0.100";

Client::~Client()
{
	closesocket(s);
	WSACleanup();
}

void Client::Init()
{
	slen = sizeof(si_other);

	// Initialise winsock
	printf("\nInitialising Winsock...");

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	// Create socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	// Setup address structure
	memset((char*)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	si_other.sin_addr.S_un.S_addr = inet_addr(serverIP.c_str());

	// Set listening socket to NON-BLOCKING
	u_long mode = 1;
	ioctlsocket(s, FIONBIO, &mode);
}

void Client::Update()
{
	// Send all objects to server
	for (int i = 0; i < NetworkManager::GetSize(); i++)
	{
		SendObject(NetworkManager::GetObjectAt(i));
	}

	char buf[BUFLEN];
	memset(buf, '\0', BUFLEN);

	int recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&si_other, &slen);

	// Got error
	if (recv_len == SOCKET_ERROR)
		return;

	NetworkObject* obj = NetworkObject::Deserialise(buf, recv_len);
	if (obj == nullptr)
	{
		std::cout << "Object == nullptr" << std::endl;
		return;
	}
}

void Client::SendObject(NetworkObject* t_object)
{
	// Serialise
	auto packet = t_object->Serialise();
	uint32_t packetSize = (uint32_t)packet.size();

	// Send the packet
	if (sendto(s, packet.data(), packetSize, 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
}
