#include "NetworkObject.h"
#include <iostream>

#include "NetworkManager.h"


NetworkObject::NetworkObject()
{
	index = NetworkManager::AddObject(this);
}

NetworkObject* NetworkObject::DeserialiseBase(const char* t_data, size_t t_size)
{
	size_t offset = 0;

	auto read = [&](void* out, size_t s) {
		memcpy(out, t_data + offset, s);
		offset += s;
		};

	uint32_t len;
	read(&len, sizeof(len));

	int objIndex;
	read(&objIndex, sizeof(objIndex));

	// Check if in range
	if (objIndex < 0 || objIndex >= NetworkManager::GetSize())
	{
		std::cout << "Object not in NetworkObjects vector - " << objIndex << std::endl;
		return nullptr;
	}

	// Get the object
	NetworkObject* obj = NetworkManager::GetObjectAt(objIndex);
	// Deserialize object
	return obj->UpdateFromDeserialisation(t_data);
}
