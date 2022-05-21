#include "EntityNetworking.hpp"

#include "engine/SystemManager.hpp"
#include "EntityManager.hpp"
#include "network/Networking.hpp"
#include "util/HostType.hpp"

EntityNetworking::EntityNetworking(SystemManager& system_manager)
	: m_networking(system_manager.Get<Networking>())
	, m_entity_manager(system_manager.Get<EntityManager>())
{
	if (Host::IsClient()) {
		m_networking.RegisterOnReceiveCallback(
			MessageId::EntityCreated,
			[this](auto& packet, auto server) {
				Entity entity;
				packet >> entity;

				m_entity_manager.Add(entity);
			}
		);

		m_networking.RegisterOnReceiveCallback(
			MessageId::EntityDestroyed,
			[this](auto& packet, auto server) {
				Entity entity;
				packet >> entity;

				m_entity_manager.Destroy(entity);
			}
		);
	}


	if (Host::IsServer()) {
		m_entity_manager.OnEntityCreated().Listen([this](Entity entity) {
			for (auto& [id, connection] : m_networking.GetConnections()) {
				connection->QueueSendReliable(
					MessageId::EntityCreated,
					[entity](auto& packet) {
						packet << entity;
					}
				);
			}
		});

		m_entity_manager.OnEntityDestroyed().Listen([this](Entity entity) {
			for (auto& [id, connection] : m_networking.GetConnections()) {
				connection->QueueSendReliable(
					MessageId::EntityDestroyed,
					[entity](auto& packet) {
						packet << entity;
					}
				);
			}
		});

		m_networking.OnConnected().Listen([this](Connection& connection) {
			for (auto entity : m_entity_manager) {
				connection.QueueSendReliable(
					MessageId::EntityCreated,
					[entity](auto& packet) {
						packet << entity;
					}
				);
			}
		});
	}
}

void EntityNetworking::Update(float dt) {
	if (Host::IsClient()) {

	}
}
