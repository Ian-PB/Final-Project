#pragma once
#include "NetworkObject.h"
#include <vector>
#include <cstdint>
#include <map>

enum class PacketType : uint8_t
{
	JoinRequest = 1,
	JoinAccepted = 2,
	UpdateNetworkObject = 3,
	SpawnNetworkObject = 4,
	AddNewClient = 5
};

class NetworkManager
{
public:
	static int AddObject(int t_owner, NetworkObject* t_newObj);
	static NetworkObject* GetObjectAt(int t_owner, int t_i);

	static int GetSize(int t_owner);
private:
	// 0 is objects owned by the host
	static std::map<int, std::vector<NetworkObject*>> networkObjects;
};

