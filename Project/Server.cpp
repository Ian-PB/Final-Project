#include "Server.h"
#include <WinSock2.h>
#include <ws2def.h>
#include <iostream>

#include "NetworkObject.h"
#include "NetworkManager.h"
#include "World.h"

#include "Health.h"
#include <raylib.h>
#include "PlayersManager.h"

Server::Server(World& t_world) : world(t_world)
{
}

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
	RecievingUpdate();

	// Allows the removal of multiple clients without worries
	for (int i = clients.size() - 1; i >= 0; i--)
	{
		if (clients[i].AliveCheck())
		{
			// Client is gone
			RemoveClient(i);
		}
	}

	ServerSending();
}

void Server::RemoveClient(int t_i)
{
	std::cout << "Client disconnected: " << clients[t_i].id << std::endl;

	int owner = clients[t_i].id + 1;
	NetworkManager::RemoveOwner(owner);
	PlayersManager::RemovePlayer(owner);

	// Remove from clients array
	clients.erase(clients.begin() + t_i);

	SendPlayerRemoval(owner);
}

void Server::SendPlayerRemoval(int t_owner)
{
	// Send client removal request
	char packet[5];
	packet[0] = static_cast<char>(PacketType::RemoveClient);

	uint32_t networkID = htonl(static_cast<uint32_t>(t_owner));

	memcpy(packet + 1, &networkID, sizeof(networkID));

	for (const ConnectedClient& client : clients)
	{
		if (sendto(s, packet, sizeof(packet), 0, (struct sockaddr*)&client.address, sizeof(client.address)) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
	}

	std::cout << "Sent client removal request to: " << t_owner << std::endl;
}

void Server::SendAddPlayer(const sockaddr_in& t_address, int t_clientID)
{
	// Send new client request
	char packet[5];
	packet[0] = static_cast<char>(PacketType::AddNewClient);

	uint32_t networkID = htonl(static_cast<uint32_t>(t_clientID));
	memcpy(packet + 1, &networkID, sizeof(networkID));

	if (sendto(s, packet, sizeof(packet), 0, (struct sockaddr*)&t_address, sizeof(t_address)) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	std::cout << "Sent new client request to: " << t_clientID << std::endl;
}

void Server::RecievingUpdate()
{
	char buf[BUFLEN];

	struct sockaddr_in sender;
	memset(buf, '\0', BUFLEN);

	int recv_len = SOCKET_ERROR;

	// Check for data
	while (true)
	{
		int temp_slen = sizeof(sender);
		recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&sender, &temp_slen);

		if (recv_len == SOCKET_ERROR)
		{
			int error = WSAGetLastError();

			// No more packets to process
			if (error == WSAEWOULDBLOCK)
				break;

			if (error == WSAECONNRESET)
				continue;

			break;
		}

		if (recv_len < 1)
			continue;

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
				continue;
			}
			else if (obj->GetIndex() < 0 || obj->GetIndex() >= NetworkManager::GetSize(obj->GetOwner()))
			{
				std::cout << "SERVER -- Object not in NetworkObjects vector - " << obj->GetIndex() << std::endl;
				continue;
			}

			// Send object directly after deserializing
			SendObjectToClients(obj);
		}
		else if (type == PacketType::ClientAlive)
		{
			int clientIndex = FindClient(sender);

			if (clientIndex == -1)
				continue;

			// Reset alive timer
			clients[clientIndex].ResetAliveTimer();
		}
	}
}

void Server::ServerSending()
{
	// Send host's objects
	for (int i = 0; i < NetworkManager::GetSize(0); i++)
	{
		NetworkObject* obj = NetworkManager::GetObjectAt(0, i);
		SendObjectToClients(obj);
	}
}

void Server::SendJoinAccepted(const sockaddr_in& t_address, int t_clientID)
{
	std::vector<char> buffer;

	auto write = [&](const void* data, size_t size) {
		const char* bytes = reinterpret_cast<const char*>(data);
		buffer.insert(buffer.end(), bytes, bytes + size);
		};

	// Packet type
	uint8_t type = static_cast<uint8_t>(PacketType::JoinAccepted);
	write(&type, sizeof(type));

	// Client ID
	write(&t_clientID, sizeof(t_clientID));

	// World info
	int seed = World::GetSeed();
	write(&seed, sizeof(seed));
	float surfaceLevel = world.GetSurfaceLevel();
	write(&surfaceLevel, sizeof(surfaceLevel));

	if (sendto(s, buffer.data(), static_cast<int>(buffer.size()), 0, (struct sockaddr*)&t_address, sizeof(t_address)) == SOCKET_ERROR)
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

	int owner = newClient.id + 1;
	newPlayers.push_back(owner);

	// Tell the new client about all existing clients (including itself)
	for (const ConnectedClient& client : clients)
	{
		SendAddPlayer(t_sender, client.id + 1);
	}

	// Tell all other clients about the new player.
	for (const ConnectedClient& client : clients)
	{
		if (client.id == newClient.id)
			continue;

		SendAddPlayer(client.address, owner);
	}
}

void Server::SendObjectToClients(NetworkObject* t_object)
{
	if (t_object == nullptr)
		return;

	auto data = t_object->Serialise();

	// Check for no data
	if (data.empty())
		return;

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
		int clientOwner = client.id + 1;

		// Dont send an object back to its owner.
		if (t_object->GetOwner() == clientOwner)
			continue;

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

bool ConnectedClient::AliveCheck()
{
	aliveTimer += GetFrameTime();

	if (aliveTimer >= TIMER_DUR)
	{
		aliveTimer = 0.0f;
		return true;
	}

	if (aliveTimer >= TIMER_DUR / 2.0f)
	{
		alive = false;
	}

	return false;
}

void ConnectedClient::ResetAliveTimer()
{
	aliveTimer = 0.0f;
	alive = true;
}
