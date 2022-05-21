#pragma once

#include <SFML/Network/Packet.hpp>

#include "util/IntTypes.hpp"

using MessageIdType = u8;
enum class MessageId : MessageIdType {
	EstablishConnection = 0,
	ClientActions,

	EntityExistingData,
	EntityCreated,
	EntityDestroyed,
	EntityDebugTransform,

	ClientConnected,
	PlayerEntityCreated,

	NumMessageTypes,
};

inline sf::Packet& operator <<(sf::Packet& packet, const MessageId& type) {
	return packet << (MessageIdType)type;
}

inline sf::Packet& operator >>(sf::Packet& packet, MessageId& type) {
	MessageIdType size_type;
	auto& result = packet >> size_type;
	type = (MessageId)size_type;
	return result;
}
