#pragma once

#include "entity/ComponentContainer.hpp"

class DebugCameraSystem {
public:
	DebugCameraSystem(class SystemManager& system_manager);
private:
	void Update(float dt);

	struct DebugCameraComponent {
	};

	ComponentContainer<DebugCameraComponent> m_components;
};