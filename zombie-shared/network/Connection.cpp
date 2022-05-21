#include "Connection.hpp"

#include "util/Log.h"

sf::Packet* PacketPool::GetFreePacket() {
	if (m_free_packets.empty()) {
		m_packet_pool.push_back(std::make_unique<sf::Packet>());
		return m_packet_pool.back().get();
	} else {
		auto result = m_free_packets.back();
		m_free_packets.pop_back();
		return result;
	}
}

void PacketPool::ReturnPacket(sf::Packet* packet) {
	packet->clear();
	m_free_packets.push_back(packet);
}

Connection::Connection(const NetworkAddress& address) : m_address{ address } {
	m_size_packet = m_packet_pool.GetFreePacket();
	m_current_packet = m_packet_pool.GetFreePacket();
}

void Connection::QueueSendUnreliable(MessageId id, SendFunc sendFunc) {
	auto& size_packet = *m_size_packet;
	size_packet.clear();
	size_packet << id;

	if (sendFunc) {
		sendFunc(size_packet);
	}

	if (size_packet.getDataSize() >= sf::UdpSocket::MaxDatagramSize) {
		// TODO: maybe make special message that splits up packet.
		Log::Error() << "Packet too big, could not send";
		return;
	} else if (size_packet.getDataSize() + m_current_packet->getDataSize() >= sf::UdpSocket::MaxDatagramSize) {
		// TODO: change max size to fit in MTU
		m_packets_to_send.push_back(m_current_packet);
		m_current_packet = m_packet_pool.GetFreePacket();
	}

	*m_current_packet << id;
	if (sendFunc) {
		sendFunc(*m_current_packet);
	}
}

void Connection::QueueSendReliable(MessageId id, SendFunc sendFunc) {
	QueueSendUnreliable(id, std::move(sendFunc));
}

void Connection::SendAllQueued(sf::UdpSocket& socket) {
	for (auto* packet : m_packets_to_send) {
		socket.send(*packet, m_address.address, m_address.port);
		m_packet_pool.ReturnPacket(packet);
	}
	m_packets_to_send.clear();

	if (m_current_packet->getDataSize() != 0) {
		socket.send(*m_current_packet, m_address.address, m_address.port);
	}
	m_current_packet->clear();
}

NetworkAddress Connection::GetAddress() const {
	return m_address;
}
