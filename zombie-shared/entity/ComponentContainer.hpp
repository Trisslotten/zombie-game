#pragma once

#include <robin_hood.h>

#include "entity/EntityManager.hpp"

template <typename T>
class ComponentContainer {
public:
	ComponentContainer(EntityManager& entity_manager) : m_entity_manager{ entity_manager } {
		m_component_type_index = m_entity_manager.GetFreeComponentTypeIndex();

		m_entity_manager.SetDestroyComponentCallback(
			m_component_type_index,
			[this](Entity entity) { Destroy(entity); }
		);
	}

	~ComponentContainer() {
		m_entity_manager.SetDestroyComponentCallback(m_component_type_index, {});
	}

	T& Create(Entity entity) {
		auto [iter, createdNew] = m_components.try_emplace(entity, std::move(T{}));

		m_entity_manager.SetHasComponent(entity, m_component_type_index, true);

		return iter->second;
	}

	bool Has(Entity entity) {
		return m_entity_manager.HasComponent(entity, m_component_type_index);
	}

	void Destroy(Entity entity) {
		m_components.erase(entity);
		m_entity_manager.SetHasComponent(entity, m_component_type_index, false);
	}
private:
	EntityManager& m_entity_manager;

	robin_hood::unordered_map<Entity, T> m_components;
	u64 m_component_type_index;
};
