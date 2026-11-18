#pragma once
#include "NetworkObject.h"
#include <vector>
#include <cstdint>

enum class PacketType : uint8_t
{
	JoinRequest = 1,
	JoinAccepted = 2,
	UpdateNetworkObject = 3,
	SpawnNetworkObject = 4
};

class NetworkManager
{
public:
	static int AddObject(NetworkObject* t_newObj);
	static NetworkObject* GetObjectAt(int t_i) { return networkObjects[t_i]; }

	static int GetSize() { return networkObjects.size(); }
private:
	static std::vector<NetworkObject*> networkObjects;
};

