#include "SystemManager.hpp"

SystemManager::SystemManager() {}

SystemManager::~SystemManager() {
	for (auto& [id, system]: m_systems) {
		system.destroy(system.system);
	}
}

Event<void()>& SystemManager::OnInitialize() {
	return m_on_initialize;
}

Event<void(float)>& SystemManager::OnUpdate() {
	return m_on_update;
}

void SystemManager::Initialize() {
	m_on_initialize.Invoke();
}

void SystemManager::Update(float dt) {
	m_on_update.Invoke(dt);
}
