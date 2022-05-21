#pragma once

#include <robin_hood.h>

#include "util/Event.hpp"
#include "util/IntTypes.hpp"
#include "util/TypeId.hpp"
#include "util/Log.h"

#include <iostream>

class SystemManager {
public:
	SystemManager();
	~SystemManager();

	Event<void()>& OnInitialize();
	Event<void(float)>& OnUpdate();

	template <class TSystem, class...Args>
	TSystem& Add(Args&&...args) {
		std::size_t id = TypeId<TSystem>();
		auto [iter, addedNew] = m_systems.emplace(id, System{});
		if (!addedNew) {
			Log::Error() << "added duplicate system";
			__debugbreak();
		}
		auto& system = iter->second;
		auto new_system = new TSystem(std::forward<Args>(args)...);
		system.system = reinterpret_cast<void*>(new_system);
		system.destroy = [](void* generic_system) {
			auto system = reinterpret_cast<TSystem*>(generic_system);
			delete system;
		};
		return *new_system;
	}

	template <class TSystem>
	bool Has() {
		std::size_t id = TypeId<TSystem>();
		auto iter = m_systems.find(id);
		return iter != m_systems.end();
	}

	template <class TSystem>
	TSystem& Get() const {
		TSystem* result = nullptr;
		std::size_t id = TypeId<TSystem>();
		auto iter = m_systems.find(id);
		if (iter != m_systems.end()) {
			result = static_cast<TSystem*>(iter->second.system);
		} else {
			Log::Error() << "tried to get non existant system";
			__debugbreak();
		}
		return *result;
	}

	void Initialize();
	void Update(float dt);

private:
	struct System {
		void* system = nullptr;
		std::function<void(void*)> destroy = {};
	};

	robin_hood::unordered_map<std::size_t, System> m_systems;

	Event<void()> m_on_initialize;
	Event<void(float)> m_on_update;
};
