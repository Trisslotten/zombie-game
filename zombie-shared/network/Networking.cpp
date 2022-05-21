#include "Networking.hpp"

#include "util/HostType.hpp"
#include "util/Log.h"

Networking::Networking() {
	m_socket.setBlocking(false);

	if (Host::IsServer()) {
		RegisterOnReceiveCallback(
			MessageId::EstablishConnection,
			[this](sf::Packet& packet, NetworkAddress client) {
				auto connection = std::make_unique<Connection>(client);
				auto ptr = connection.get();
				m_connections.emplace(client, std::move(connection));

				Log::Info() << "Client " << client.address << ":" << client.port << " connected";

				ptr->QueueSendReliable(MessageId::EstablishConnection);

				m_on_connected.Invoke(*ptr);
			}
		);
	}

	if (Host::IsClient()) {
		RegisterOnReceiveCallback(
			MessageId::EstablishConnection,
			[this](sf::Packet& packet, NetworkAddress server) {
				auto connection = std::make_unique<Connection>(server);
				auto ptr = connection.get();

				m_connections.emplace(server, std::move(connection));

				Log::Info() << "Connected to server " << server.address << ":" << server.port;

				m_on_connected.Invoke(*ptr);
			}
		);
	}
}

void Networking::Bind(std::optional<unsigned short> port) {
	unsigned short used_port = sf::UdpSocket::AnyPort;
	if (port.has_value()) {
		used_port = port.value();
	}
	auto status = m_socket.bind(used_port);
	if (status == sf::Socket::Status::Done) {
		m_socket_bound = true;
		Log::Info() << "Started on port " << m_socket.getLocalPort();
	} else {
		Log::Error() << "Could not bind socket to port " << used_port;
	}
}

void Networking::Connect(const NetworkAddress& address) {
	Connection connection(address);
	connection.QueueSendReliable(
		MessageId::EstablishConnection,
		[](sf::Packet& packet) {}
	);
	connection.SendAllQueued(m_socket);
}

void Networking::RegisterOnReceiveCallback(
	MessageId id, 
	std::function<void(sf::Packet&, NetworkAddress)> receiveFunc
) {
	m_receive_functions[id] = receiveFunc;
}

void Networking::Update() {
	for (auto& [id, connection] : m_connections) {
		connection->SendAllQueued(m_socket);
	}

	Receive();
}

Event<void(Connection&)>& Networking::OnConnected() {
	return m_on_connected;
}

void Networking::Receive() {
	if (!m_socket_bound) {
		return;
	}

	while (true) {
		m_receive_packet.clear();

		NetworkAddress sender;
		auto status = m_socket.receive(m_receive_packet, sender.address, sender.port);

		if (status != sf::Socket::Status::Done) {
			break;
		}

		while (!m_receive_packet.endOfPacket()) {
			MessageId type;
			m_receive_packet >> type;
			if (type >= MessageId::NumMessageTypes) {
				Log::Error() << "Unknown MessageType " << static_cast<u32>(type) << ", skipping packet";
				return;
			}

			auto iter = m_receive_functions.find(type);
			if (iter == m_receive_functions.end()) {
				Log::Error() << "Receive function not registered for MessageType " << static_cast<u32>(type) << ", skipping packet";
				return;
			}

			iter->second(m_receive_packet, sender);
		}
	}
}

robin_hood::unordered_flat_map<NetworkAddress, std::unique_ptr<Connection>>& Networking::GetConnections() {
	return m_connections;
}
