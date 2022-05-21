#pragma once

#include "SystemManager.hpp"
#include "util/FrameLimiter.hpp"

class Engine {
public:
	Engine(std::function<void(SystemManager&)> add_host_systems = {});

	bool IsRunning() const;
	void StopRunning();

	void Initialize();
	void Update(float deltatime);

	SystemManager& GetSystemManager();

private:
	SystemManager m_system_manager;
	std::atomic_bool m_running = true;
};