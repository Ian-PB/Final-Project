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

int NetworkManager::GetSize(int t_owner)
{
	auto it = networkObjects.find(t_owner);

	// Make sure no new ones created from this function
	if (it == networkObjects.end())
		return 0;

	return static_cast<int>(it->second.size());
}
