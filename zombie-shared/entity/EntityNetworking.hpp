#pragma once

#include "util/HostType.hpp"

class Networking;
class SystemManager;
class EntityManager;

class EntityNetworking {
public:
	EntityNetworking(SystemManager& system_manager);
	~EntityNetworking() = default;
private:
	void Update(float dt);

	Networking& m_networking;
	EntityManager& m_entity_manager;
};
