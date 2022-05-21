#pragma once

#include <SFML/Network/UdpSocket.hpp>

#include "network/Networking.hpp"

class ClientManager;
class SystemManager;

class NetworkSystem {
public:
	NetworkSystem(SystemManager& system_manager);
	~NetworkSystem();
private:
	void Initialize();
	void Update();

	Networking& m_networking;
	ClientManager& m_client_manager;
};
