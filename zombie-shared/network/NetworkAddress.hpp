#pragma once

#include <SFML/Network/IpAddress.hpp>

#include "util/IntTypes.hpp"

constexpr u64 NETWORK_ADDRESS_ID_NONE = 0;

struct NetworkAddress {
	sf::IpAddress address = sf::IpAddress("0.0.0.0");
	u16 port = 0;

	NetworkAddress& operator=(u64 id) {
		address = sf::IpAddress(static_cast<u32>(id & 0xFFFFFFFF));
		port = static_cast<u16>((id >> 32) & 0xFFFF);
		return *this;
	}

	u64 GetId() const {
		return static_cast<u64>(address.toInteger()) | (static_cast<u64>(port) << 32llu);
	}
};
inline bool operator==(const NetworkAddress& left, const NetworkAddress& right) {
	return left.address == right.address && left.port == right.port;
}
inline bool operator!=(const NetworkAddress& left, const NetworkAddress& right) {
	return !(left == right);
}
namespace std {
	template <> struct hash<NetworkAddress>   {
		size_t operator()(const NetworkAddress& address) const {
			return address.GetId();
		}
	};
}
