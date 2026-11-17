#pragma once

#include <vector>
#include <memory>


class NetworkObject
{
public:
	NetworkObject(int t_id);

	virtual std::vector<char> Serialise(const std::shared_ptr<NetworkObject> t_packet) = 0;
	virtual std::shared_ptr<NetworkObject> Deserialise(const char* t_data, size_t t_size) = 0;

protected:
	int id = -1; // Index for vector
};

