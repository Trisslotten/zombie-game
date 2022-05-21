#include "PlayerEntitySystem.hpp"

#include "engine/SystemManager.hpp"
#include "engine/ClientManager.hpp"
#include "util/HostType.hpp"
#include "entity/EntityManager.hpp"
#include "network/Networking.hpp"

PlayerEntitySystem::PlayerEntitySystem(SystemManager& system_manager)
	: m_entity_manager{ system_manager.Get<EntityManager>() }
	, m_client_manager{ system_manager.Get<ClientManager>() }
	, m_networking{ system_manager.Get<Networking>() }
{
	if (Host::IsServer()) {
		m_networking.OnConnected().Listen([this](Connection& connection) {
			auto client = m_client_manager.GetClient(connection.GetAddress());
			auto entity = m_entity_manager.Create();

			m_on_player_entity_created.Invoke(client->id, entity);

			for (auto& [address, connection] : m_networking.GetConnections()) {
				connection->QueueSendReliable(
					MessageId::PlayerEntityCreated,
					[entity, client_id = client->id](sf::Packet& packet) {
						packet << entity;
						packet << client_id;
					}
				);
			}
		});

		m_networking.RegisterOnReceiveCallback(
			MessageId::EntityDebugTransform,
			[this](auto& packet, auto client) {
				Entity entity;
				Transform transform;
				packet >> entity;
				packet >> transform;
				auto entity_transform = m_entity_manager.GetTransform(entity);
				entity_transform.SetTransform(transform);

				auto& p = transform.position;
				Log::Debug() << p.x;
			}
		);
	}

	if (Host::IsClient()) {
		m_networking.RegisterOnReceiveCallback(
			MessageId::PlayerEntityCreated,
			[this](sf::Packet& packet, auto address) {
				Entity entity;
				ClientId client_id;
				packet >> entity;
				packet >> client_id;

				m_entity_to_client[entity] = client_id;
				m_client_to_entity[client_id] = entity;

				m_on_player_entity_created.Invoke(client_id, entity);
			}
		);
	}

	system_manager.OnUpdate().Listen([this](float dt) {
		Update(dt);
	});
}

PlayerEntitySystem::~PlayerEntitySystem() {
}

Event<void(ClientId, Entity)>& PlayerEntitySystem::OnPlayerEntityCreated() {
	return m_on_player_entity_created;
}

void PlayerEntitySystem::Update(float dt) {
	if (Host::IsClient()) {
		for (auto& [id, connection] : m_networking.GetConnections()) {
			auto my_id = m_client_manager.GetMyId();
			//if (!my_id) {
			//	continue;
			//}
			auto entity_iter = m_client_to_entity.find(my_id);
			//if (entity_iter == m_client_to_entity.end()) {
			//	continue;
			//}
			auto entity = entity_iter->second;
			//if (!m_entity_manager.Has(entity)) {
			//	continue;
			//}

			auto transform = m_entity_manager.GetTransform(entity);

			if (m_entity_manager.IsDirty(entity, EntityDirtyFlag::Position)) {
				connection->QueueSendUnreliable(
					MessageId::EntityDebugTransform,
					[&](auto& packet) {
						packet << entity;
						packet << transform.GetTransform();
					}
				);

				m_entity_manager.ClearDirtyFlags();
			}
		}
	}
}
