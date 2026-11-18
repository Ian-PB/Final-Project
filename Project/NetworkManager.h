#pragma once
#include "NetworkObject.h"
#include <vector>

class NetworkManager
{
public:
	static int AddObject(NetworkObject* t_newObj);
	static NetworkObject* GetObjectAt(int t_i) { return networkObjects[t_i]; }

private:
	static std::vector<NetworkObject*> networkObjects;
};

