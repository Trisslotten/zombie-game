#include "DebugPlayerController.hpp"

#include "engine/SystemManager.hpp"
#include "engine/ClientManager.hpp"
#include "entity/EntityManager.hpp"
#include "gameplay/management/PlayerEntitySystem.hpp"
#include "Input.hpp"

DebugPlayerController::DebugPlayerController(SystemManager& system_manager) 
	: m_client_manager{ system_manager.Get<ClientManager>() }
	, m_entity_manager{ system_manager.Get<EntityManager>() }
	, m_player_entity_system{ system_manager.Get<PlayerEntitySystem>() }
	, m_input{ system_manager.Get<Input>() }
{
	m_player_entity_system.OnPlayerEntityCreated().Listen([this](auto client, auto entity) {
		if (m_client_manager.IsMe(client)) {
			m_player_entity = entity;
		}
	});

	system_manager.OnUpdate().Listen([this](float dt) {
		if (!m_player_entity) {
			return;
		}

		auto transform = m_entity_manager.GetTransform(m_player_entity);

		if (m_input.Has(InputAction::MoveUp)) {
			transform.GetPosition();
		}
	});
}
