#pragma once

#include <robin_hood.h>
#include "entity/Entity.hpp"
#include "engine/ClientManager.hpp"
#include "util/Event.hpp"

class SystemManager;
class EntityManager;
class Networking;

class PlayerEntitySystem {
public:
	PlayerEntitySystem(SystemManager& system_manager);
	~PlayerEntitySystem();

	Event<void(ClientId, Entity)>& OnPlayerEntityCreated();
private:
	void Update(float dt);

	EntityManager& m_entity_manager;
	ClientManager& m_client_manager;
	Networking& m_networking;

	Event<void(ClientId, Entity)> m_on_player_entity_created;

	robin_hood::unordered_flat_map<Entity, ClientId> m_entity_to_client;
	robin_hood::unordered_flat_map<ClientId, Entity> m_client_to_entity;
};