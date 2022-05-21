#pragma once

#include <robin_hood.h>
#include <optional>

#include "util/IntTypes.hpp"
#include "util/TypeSafeId.hpp"
#include "network/NetworkAddress.hpp"
#include "entity/Entity.hpp"
#include "util/Event.hpp"

using ClientId = TypeSafeId<struct TagClientId, u32, 0>;

struct Client {
	ClientId id;
	NetworkAddress network_address;
};

class ClientManager {
public:
	ClientManager(class SystemManager& system_manager);

	std::optional<Client> GetClient(ClientId id) const;
	std::optional<Client> GetClient(NetworkAddress network_address) const;

	const robin_hood::unordered_node_map<ClientId, Client>& GetClients() const;

	Event<void(const Client& player)>& OnClientConnected();

	bool IsMe(ClientId id) const;
	ClientId GetMyId() const;

private:
	Client CreateClient(NetworkAddress network_address);
	void AddClient(Client client);

	class Networking& m_networking;

	ClientId m_my_id;

	TypeSafeIdGenerator<ClientId> m_id_generator;
	robin_hood::unordered_node_map<ClientId, Client> m_clients;
	robin_hood::unordered_flat_map<NetworkAddress, Client*> m_network_address_lookup;

	Event<void(const Client& old, const Client& newP)> m_on_player_updated;
	Event<void(const Client& player)> m_on_player_connected;
};
