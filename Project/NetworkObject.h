#pragma once

#include <vector>
#include <memory>


class NetworkObject
{
public:
	NetworkObject();

	static NetworkObject* DeserialiseBase(const char* t_data, size_t t_size);

	virtual std::vector<char> Serialise() { return std::vector<char>(); }
	virtual NetworkObject* UpdateFromDeserialisation(const char* t_data) { return nullptr; }

	int GetIndex() const { return index; }
protected:
	int index = -1;
};

