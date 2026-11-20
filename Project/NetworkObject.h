#pragma once

#include <vector>
#include <memory>


class NetworkObject
{
public:
	NetworkObject(int t_owner);

	static NetworkObject* Deserialise(const char* t_data, size_t t_size);

	virtual std::vector<char> Serialise() { return std::vector<char>(); }
	virtual NetworkObject* UpdateFromDeserialisation(const char* t_data) { return nullptr; }

	int GetIndex() const { return index; }
	int GetOwner() const { return owner; }
protected:
	void SerialiseBase(std::vector<char>& t_buffer);

	int index = -1;
	int owner = -1; // 0 = Host, Anything above is a client
};

