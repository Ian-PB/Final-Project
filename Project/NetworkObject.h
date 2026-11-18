#pragma once

#include <vector>
#include <memory>


class NetworkObject
{
public:
	NetworkObject();

	static void DeserialiseBase(const char* t_data, size_t t_size);

	virtual std::vector<char> Serialise(const std::shared_ptr<NetworkObject> t_packet) {}
	virtual void Deserialise(const char* t_data, size_t t_size) {}

protected:
	int index = -1;
};

