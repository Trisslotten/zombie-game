#pragma once

#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/Packet.hpp>
#include <type_traits>
#include <iostream>
#include <functional>

#include "PacketTypes.hpp"
#include "NetworkAddress.hpp"

class PacketPool {
public:
	sf::Packet* GetFreePacket();
	void ReturnPacket(sf::Packet*);
private:
	std::vector<sf::Packet*> m_free_packets;
	std::vector<std::unique_ptr<sf::Packet>> m_packet_pool;
};

class Connection {
public:
	Connection(const NetworkAddress& address);

	using SendFunc = std::function<void(sf::Packet&)>;
	void QueueSendUnreliable(MessageId id, SendFunc sendFunc = {});
	
	void QueueSendReliable(MessageId id, SendFunc sendFunc = {});

	void SendAllQueued(sf::UdpSocket& socket);

	NetworkAddress GetAddress() const;

private:
	PacketPool m_packet_pool;

	NetworkAddress m_address;
	std::vector<sf::Packet*> m_packets_to_send;
	sf::Packet* m_current_packet;
	sf::Packet* m_size_packet;
};
