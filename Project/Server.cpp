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
	// Only send host player
	NetworkObject* obj = NetworkManager::GetObjectAt(0);
	SendObjectToClients(obj);

	char buf[BUFLEN];

	struct sockaddr_in sender;
	int temp_slen = sizeof(sender);
	memset(buf, '\0', BUFLEN);

	int recv_len = SOCKET_ERROR;

	// Check for data
	while (true)
	{
		recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&sender, &temp_slen);
		if (recv_len == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET)
			continue;

		break;
	}

	if (recv_len == SOCKET_ERROR)
		return;

	if (recv_len < 1)
		return;

	PacketType type = static_cast<PacketType>(static_cast<uint8_t>(buf[0]));

	if (type == PacketType::JoinRequest)
	{
		int clientIndex = FindClient(sender);

		if (clientIndex == -1)
		{
			CreateNewClient(sender, clientIndex);
		}

		int id = clients[clientIndex].id;

		SendJoinAccepted(sender, id);
	}
	else if (type == PacketType::UpdateNetworkObject)
	{
		// Deserialise
		NetworkObject* obj = NetworkObject::Deserialise(buf + 1, recv_len - 1);
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
	// When an object needs to be spawned this is recieved
	else if (type == PacketType::SpawnNetworkObject)
	{
		// Add object to networkObjects
		
		// Send to all clients besides owner
	}
}

void Server::SendJoinAccepted(const sockaddr_in& t_address, int t_clientID)
{
	char packet[5];

	packet[0] = static_cast<char>(PacketType::JoinAccepted);

	uint32_t networkID = htonl(static_cast<uint32_t>(t_clientID));

	memcpy(packet + 1, &networkID, sizeof(networkID));

	if (sendto(s, packet, sizeof(packet), 0, (struct sockaddr*)&t_address, sizeof(t_address)) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	std::cout << "Sent client ID: " << t_clientID << std::endl;
}

int Server::FindClient(const sockaddr_in& t_address)
{
	for (int i = 0; i < clients.size(); i++)
	{
		const sockaddr_in& existingAddr = clients[i].address;

		// Check if same client
		if (existingAddr.sin_addr.s_addr == t_address.sin_addr.s_addr && existingAddr.sin_port == t_address.sin_port)
		{
			return i;
		}
	}

	// Client not found
	return -1;
}

void Server::CreateNewClient(struct sockaddr_in t_sender, int& t_index)
{
	ConnectedClient newClient;
	newClient.address = t_sender;
	newClient.id = nextClientID++;

	clients.push_back(newClient);

	t_index = static_cast<int>(clients.size()) - 1;

	std::cout << "New client joined - ID: " << newClient.id << std::endl;
}

void Server::SendObjectToClients(NetworkObject* t_object)
{
	if (t_object == nullptr)
		return;

	auto data = t_object->Serialise();

	if (data.size() + 1 > BUFLEN)
		return;

	// Serialise
	std::vector<char> packet;
	packet.reserve(data.size() + 1);
	packet.push_back(static_cast<char>(PacketType::UpdateNetworkObject));

	// Inset the packet data after the type
	packet.insert(packet.end(), data.begin(), data.end());

	uint32_t packetSize = (uint32_t)packet.size();

	for (const ConnectedClient& client : clients)
	{
		// Send the packet
		if (sendto(s, packet.data(), packetSize, 0, (struct sockaddr*)&client.address, sizeof(client.address)) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
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
