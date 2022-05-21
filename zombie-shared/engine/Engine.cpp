#include "Engine.hpp"

#include "engine/SystemManager.hpp"
#include "ClientManager.hpp"
#include "network/Networking.hpp"
#include "network/NetworkSystem.hpp"
#include "entity/EntityManager.hpp"
#include "entity/EntityNetworking.hpp"
#include "gameplay/management/PlayerEntitySystem.hpp"

Engine::Engine(std::function<void(SystemManager&)> add_host_systems) {
	m_system_manager.Add<Networking>();
	m_system_manager.Add<EntityManager>();
	m_system_manager.Add<ClientManager>(m_system_manager);
	m_system_manager.Add<NetworkSystem>(m_system_manager);
	m_system_manager.Add<EntityNetworking>(m_system_manager);
	m_system_manager.Add<PlayerEntitySystem>(m_system_manager);

	if (add_host_systems) {
		add_host_systems(m_system_manager);
	}
}

bool Engine::IsRunning() const {
	return m_running;
}

void Engine::StopRunning() {
	m_running = false;
}

void Engine::Initialize() {
	m_system_manager.Initialize();
}

void Engine::Update(float deltatime) {
	m_system_manager.Update(deltatime);
}

SystemManager& Engine::GetSystemManager() {
	return m_system_manager;
}
