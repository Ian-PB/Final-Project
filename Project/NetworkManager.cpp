#include "NetworkManager.h"

std::vector<NetworkObject*> NetworkManager::networkObjects;

int NetworkManager::AddObject(NetworkObject* t_newObj)
{
	networkObjects.push_back(t_newObj);
	return networkObjects.size()-1;
}
