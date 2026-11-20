#include "NetworkManager.h"

std::map<int, std::vector<NetworkObject*>> NetworkManager::networkObjects{

	{0, std::vector<NetworkObject*>()}, // Default Server
};

int NetworkManager::AddObject(int t_owner, NetworkObject* t_newObj)
{
	networkObjects[t_owner].push_back(t_newObj);
	return networkObjects[t_owner].size()-1;
}

NetworkObject* NetworkManager::GetObjectAt(int t_owner, int t_i)
{
	auto it = networkObjects.find(t_owner);
	
	// Check if exists
	if (it == networkObjects.end())
		return nullptr;

	// Outside of range
	if (t_i < 0 || static_cast<size_t>(t_i) >= it->second.size())
		return nullptr;

	return it->second[t_i];
}

int NetworkManager::AddNewVector()
{
	int next = NetworkManager::networkObjects.rbegin()->first + 1;
	NetworkManager::networkObjects[next] = std::vector<NetworkObject*>();

	// Return the new vectors key
	return next;
}
