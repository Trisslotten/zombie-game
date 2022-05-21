#pragma once

#include <optional>
#include <vector>
#include <array>
#include <functional>
#include <memory>
#include <robin_hood.h>
#include <SFML/Network/UdpSocket.hpp>

#include "PacketTypes.hpp"
#include "Connection.hpp"
#include "util/Event.hpp"

template <class T, class = typename std::enable_if<std::is_trivially_copyable<T>::value>::type>
sf::Packet& operator <<(sf::Packet& packet, const T& m) {
	packet.append(&m, sizeof(T));
	return packet;
}

template <class T, class = typename std::enable_if<std::is_trivially_copyable<T>::value>::type>
sf::Packet& operator >>(sf::Packet& packet, T& m) {
	std::array<u8, sizeof(T)> data;
	for (auto& element : data) {
		packet >> element;
	}
	m = *reinterpret_cast<T*>(data.data());
	return packet;
}


template <class T, class = typename std::enable_if<std::is_trivially_copyable<T>::value>::type>
sf::Packet& operator <<(sf::Packet& packet, const std::vector<T>& m) {
	packet << static_cast<u16>(m.size());
	packet.append(m.data(), sizeof(T) * m.size());
	return packet;
}

template <class T>
sf::Packet& operator <<(sf::Packet& packet, const std::vector<T>& vec) {
	packet << static_cast<u16>(vec.size());
	for (const auto& element : vec) {
		packet << element;
	}
	return packet;
}

template <class T>
sf::Packet& operator >>(sf::Packet& packet, std::vector<T>& vec) {
	u16 size;
	packet >> size;
	vec.resize(size);
	for (auto& element : vec) {
		packet >> element;
	}
	return packet;
}

class Networking {
public:
	Networking();

	void Bind(std::optional<unsigned short> port = std::nullopt);

	void Connect(const NetworkAddress& address);

	using ReceiveFunc = std::function<void(sf::Packet&, NetworkAddress)>;
	void RegisterOnReceiveCallback(
		MessageId id,
		ReceiveFunc receiveFunc
	);

	void Update();

	Event<void(Connection&)>& OnConnected();

	robin_hood::unordered_flat_map<NetworkAddress, std::unique_ptr<Connection>>& GetConnections();
private:
	void Receive();

	Event<void(Connection&)> m_on_connected;

	sf::Packet m_receive_packet;
	sf::UdpSocket m_socket;
	bool m_socket_bound = false;
	robin_hood::unordered_flat_map<NetworkAddress, std::unique_ptr<Connection>> m_connections;
	robin_hood::unordered_map<MessageId, ReceiveFunc> m_receive_functions;
};
