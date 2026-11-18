#include "NetworkObject.h"
#include <iostream>

#include "NetworkManager.h"


NetworkObject::NetworkObject()
{
	index = NetworkManager::AddObject(this);
}

void NetworkObject::DeserialiseBase(const char* t_data, size_t t_size)
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

	// Get the object
	NetworkObject* obj = NetworkManager::GetObjectAt(objIndex);
	// Deserialize object
	obj->Deserialise(t_data, t_size);
}
