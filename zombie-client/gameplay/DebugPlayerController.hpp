#pragma once

#include "entity/Entity.hpp"

class DebugPlayerController {
public:
	DebugPlayerController(class SystemManager& system_manager);

private:
	class ClientManager& m_client_manager;
	class EntityManager& m_entity_manager;
	class PlayerEntitySystem& m_player_entity_system;
	class Input& m_input;

	Entity m_player_entity;
};