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

	// Check if in range
	if (objIndex < 0 || objIndex >= NetworkManager::GetSize(objOwner))
	{
		std::cout << "OBJ -- Object not in NetworkObjects vector - " << objIndex << std::endl;
		return nullptr;
	}

	// Get the object
	NetworkObject* obj = NetworkManager::GetObjectAt(objOwner, objIndex);
	// Deserialize object
	return obj->UpdateFromDeserialisation(t_data + offset);
}

void NetworkObject::SerialiseBase(std::vector<char>& t_buffer)
{
	auto write = [&](const void* data, size_t size) {
		const char* bytes = reinterpret_cast<const char*>(data);
		t_buffer.insert(t_buffer.end(), bytes, bytes + size);
		};

	uint32_t len = 0;
	write(&len, sizeof(len));

	write(&owner, sizeof(owner));
	write(&index, sizeof(index));
}
