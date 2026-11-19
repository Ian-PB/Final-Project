#include "NetworkObject.h"
#include <iostream>

#include "NetworkManager.h"


NetworkObject::NetworkObject(int t_owner) : owner(t_owner)
{
	index = NetworkManager::AddObject(owner, this);
}

NetworkObject* NetworkObject::Deserialise(const char* t_data, size_t t_size)
{
	size_t offset = 0;

	auto read = [&](void* out, size_t s) {
		memcpy(out, t_data + offset, s);
		offset += s;
		};

	uint32_t len;
	read(&len, sizeof(len));

	int objOwner;
	read(&objOwner, sizeof(objOwner));


	int objIndex;
	read(&objIndex, sizeof(objIndex));

	std::cout << "Received Owner: " << objOwner
		<< " Index: " << objIndex
		<< " Vector size: " << NetworkManager::GetSize(objOwner)
		<< std::endl;

	// Check if in range
	if (objIndex < 0 || objIndex >= NetworkManager::GetSize(objOwner))
	{
		std::cout << "Object not in NetworkObjects vector - " << objIndex << std::endl;
		return nullptr;
	}

	// Get the object
	NetworkObject* obj = NetworkManager::GetObjectAt(objOwner, objIndex);
	// Deserialize object
	return obj->UpdateFromDeserialisation(t_data + offset);
}
