#include "Client.h"
#include "NetworkManager.h"
#include <iostream>
#include <raylib.h>
#include "PlayersManager.h"

std::string Client::serverIP = "";

Client::Client(World& t_world) : world(t_world)
{
}

Client::~Client()
{
	closesocket(s);
	WSACleanup();
}

void Client::JoinServer(std::string t_serverIP)
{
	serverIP = t_serverIP;
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

	SendJoinMsg();
}

void Client::SendJoinMsg()
{
	uint8_t packet = static_cast<uint8_t>(PacketType::JoinRequest);

	// Send the packet
	if (sendto(s, reinterpret_cast<const char*>(&packet), sizeof(packet), 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	std::cout << "JOIN sent to server" << std::endl;
}

void Client::Update()
{
	// Only send objects when joined
	if (joined)
	{
		// Send all owned objects
		int clientObjs = GetClientsObjs();

		for (int i = 0; i < NetworkManager::GetSize(clientObjs); i++)
		{
			SendObject(NetworkManager::GetObjectAt(clientObjs, i));
		}

		if (AliveTimer())
		{
			SendAliveMessage();
		}
	}

	char buf[BUFLEN];
	memset(buf, '\0', BUFLEN);

	while (true)
	{
		int recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&si_other, &slen);

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

		if (type == PacketType::JoinAccepted)
		{
			AcceptedJoin(recv_len, buf);
		}
		else if (type == PacketType::AddNewClient)
		{
			if (recv_len < 5)
				continue;

			uint32_t networkID;
			memcpy(&networkID, buf + 1, sizeof(networkID));

			int ownerID = static_cast<int>(ntohl(networkID));

			newPlayers.push_back(ownerID);
			std::cout << "New client vector added" << std::endl;
		}
		else if (type == PacketType::UpdateNetworkObject)
		{
			NetworkObject* obj = NetworkObject::Deserialise(buf + 1, recv_len - 1);

			if (obj == nullptr)
			{
				std::cout << "Object == nullptr" << std::endl;
				continue;
			}
		}
		else if (type == PacketType::RemoveClient)
		{
			if (recv_len < 5)
				continue;

			uint32_t networkID;
			memcpy(&networkID, buf + 1, sizeof(networkID));

			int owner = static_cast<int>(ntohl(networkID));
			NetworkManager::RemoveOwner(owner);
			PlayersManager::RemovePlayer(owner);

			std::cout << "Player removed: " << owner << std::endl;
		}
	}
}

void Client::SendObject(NetworkObject* t_object)
{
	if (t_object == nullptr)
		return;

	auto data = t_object->Serialise();

	if (data.empty())
		return;

	// Check that adding the packet type wont go over the limit
	if (data.size() + 1 > BUFLEN)
		return;

	// Serialise
	std::vector<char> packet;
	packet.reserve(data.size() + 1);
	packet.push_back(static_cast<char>(PacketType::UpdateNetworkObject));

	// Inset the packet data after the type
	packet.insert(packet.end(), data.begin(), data.end());

	uint32_t packetSize = (uint32_t)packet.size();

	// Send the packet
	if (sendto(s, packet.data(), packetSize, 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
}

void Client::AcceptedJoin(int recv_len, char buf[])
{
	size_t offset = 1;

	auto read = [&](void* out, size_t s) {
		memcpy(out, buf + offset, s);
		offset += s;
		};

	read(&clientID, sizeof(clientID));

	// World Seed
	int seed;
	read(&seed, sizeof(seed));

	// World SurfaceLevel
	float surfaceLevel;
	read(&surfaceLevel, sizeof(surfaceLevel));

	// Set them to world
	world.SetSurfaceLevel(surfaceLevel);
	world.Init(seed);

	joined = true;

	std::cout << "Joined server - Client ID: " << clientID << std::endl;
}

void Client::SendAliveMessage()
{
	std::vector<char> buffer;

	auto write = [&](const void* data, size_t size) {
		const char* bytes = reinterpret_cast<const char*>(data);
		buffer.insert(buffer.end(), bytes, bytes + size);
		};

	// Packet type
	uint8_t type = static_cast<uint8_t>(PacketType::ClientAlive);
	write(&type, sizeof(type));

	// Send the packet
	if (sendto(s, buffer.data(), static_cast<int>(buffer.size()), 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	std::cout << "Client Alive sent to server" << std::endl;
}

bool Client::AliveTimer()
{
	if (aliveTimer >= TIME_BETWEEN_ALIVE_MSGS)
	{
		aliveTimer = 0.0f;
		return true;
	}

	aliveTimer += GetFrameTime();

	return false;
}

ConnectionType Client::JoiningTimer()
{
	if (!joined)
	{
		if (joinTimer < JOINING_WAIT_DUR)
		{
			joinTimer += GetFrameTime();
			return ConnectionType::Waiting;
		}
		else
		{
			return ConnectionType::TookTooLong;
		}
	}
	else // When joining success
		return ConnectionType::Connected;
}
