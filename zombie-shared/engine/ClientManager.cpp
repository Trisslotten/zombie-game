#include "ClientManager.hpp"

#include "SystemManager.hpp"
#include "network/Networking.hpp"
#include "util/HostType.hpp"

ClientManager::ClientManager(SystemManager& system_manager)
	: m_networking{ system_manager.Get<Networking>() } 
{
	if (Host::IsClient()) {
		m_networking.RegisterOnReceiveCallback(
			MessageId::ClientConnected,
			[this](auto& packet, auto address) {
				bool me;
				Client client;
				packet >> me;
				packet >> client;
				AddClient(client);

				if (me) {
					m_my_id = client.id;
				}
			}
		);
	}

	if (Host::IsServer()) {
		m_networking.OnConnected().Listen([this](Connection& connection) {
			auto new_client = CreateClient(connection.GetAddress());

			for (auto& [id, client] : m_clients) {
				connection.QueueSendReliable(
					MessageId::ClientConnected,
					[&](sf::Packet& packet) {
						packet << (new_client.id == id);
						packet << client;
					}
				);
			}
		});
	}
}

std::optional<Client> ClientManager::GetClient(ClientId id) const {
	if (auto iter = m_clients.find(id); iter != m_clients.end()) {
		return iter->second;
	} else {
		return std::nullopt;
	}
}

std::optional<Client> ClientManager::GetClient(NetworkAddress network_address) const {
	if (auto iter = m_network_address_lookup.find(network_address); iter != m_network_address_lookup.end()) {
		return *iter->second;
	} else {
		return std::nullopt;
	}
}

Client ClientManager::CreateClient(NetworkAddress network_address) {
	Client client;
	client.id = m_id_generator.Generate();
	client.network_address = network_address;

	AddClient(client);

	return client;
}

void ClientManager::AddClient(Client client) {
	auto [iter, _] = m_clients.emplace(client.id, client);
	m_network_address_lookup.emplace(client.network_address, &iter->second);

	if (Host::IsServer()) {
		for (auto& [address, connection] : m_networking.GetConnections()) {
			connection->QueueSendReliable(
				MessageId::ClientConnected,
				[client = iter->second, &address](auto& packet) {
					packet << (client.network_address == address);
					packet << client;
				}
			);
		}
	}

	m_on_player_connected.Invoke(iter->second);
}

const robin_hood::unordered_node_map<ClientId, Client>& ClientManager::GetClients() const {
	return m_clients;
}

Event<void(const Client& player)>& ClientManager::OnClientConnected() {
	return m_on_player_connected;
}

bool ClientManager::IsMe(ClientId id) const {
	return m_my_id == id;
}

ClientId ClientManager::GetMyId() const {
	return m_my_id;
}
