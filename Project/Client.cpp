#include "Client.h"

std::string Client::serverIP = "127.0.0.1";

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
}

void Client::SendObject(std::shared_ptr<NetworkObject> t_object)
{
	// Serialise
	auto packet = t_object->Serialise(t_object);
	uint32_t packetSize = (uint32_t)packet.size();

	// Send the packet
	if (sendto(s, packet.data(), packetSize, 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
}
