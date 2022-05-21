#include "NetworkSystem.hpp"

#include <SFML/Network/Packet.hpp>
#include <iostream>

#include "util/HostType.hpp"
#include "engine/SystemManager.hpp"
#include "engine/ClientManager.hpp"

NetworkSystem::NetworkSystem(SystemManager& system_manager)
	: m_client_manager{ system_manager.Get<ClientManager>() }
	, m_networking{ system_manager.Get<Networking>() }
{
	system_manager.OnInitialize().Listen([this]() { Initialize(); });
	system_manager.OnUpdate().Listen([this](float dt) { Update(); });
}

NetworkSystem::~NetworkSystem() {}

void NetworkSystem::Initialize() {
	if (Host::IsServer()) {
		m_networking.Bind(13337);
	}
	
	if (Host::IsClient()) {
		m_networking.Bind();
		m_networking.Connect({ "localhost", 13337 });
	}
}

void NetworkSystem::Update() {
	m_networking.Update();
}
