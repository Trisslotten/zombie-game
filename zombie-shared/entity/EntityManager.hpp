#pragma once

#include <optional>
#include <functional>
#include <array>

#include "engine/Transform.hpp"
#include "entity.hpp"
#include "util/IntTypes.hpp"
#include "util/Event.hpp"
#include "util/BitField.hpp"

class EntityTransform;

class EntityBitField : public BitField {
public:
	std::optional<u64> FindFreeIndex();
private:
	u64 m_search_index = 0;
};

enum class EntityDirtyFlag {
	Position,
	PositionInterpolate,
	Orientation,
	OrientationInterpolate,
	Scale,
	ScaleInterpolate,
};

class EntityManager {
public:
	using DirtyFlagBits = StaticBitfield<u8, 1>;

	EntityManager();
	~EntityManager();

	Entity Create();
	void Add(Entity entity);
	bool Has(Entity entity) const;

	void Destroy(Entity entity);

	EntityTransform GetTransform(Entity entity);
	bool IsDirty(Entity entity, EntityDirtyFlag flag) const;

	Event<void(Entity)>& OnEntityCreated();
	Event<void(Entity)>& OnEntityDestroyed();

	u64 GetFreeComponentTypeIndex();
	void SetDestroyComponentCallback(u64 component, std::function<void(Entity)> func);
	void SetHasComponent(Entity entity, u64 component, bool has_component);
	bool HasComponent(Entity entity, u64 component) const;

	void ClearDirtyFlags();

	class Iterator {
	public:
		Iterator(
			u64 index,
			const EntityBitField& existance_map, 
			const std::vector<u16>& versions
		)
			: m_index(index)
			, m_existance_map(existance_map)
			, m_versions(versions) 
		{ 
		}

		Entity operator*() const;

		// Prefix increment
		Iterator& operator++();

		// Postfix increment
		Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
		friend bool operator==(const Iterator& a, const Iterator& b) {
			return a.m_index == b.m_index; 
		};
		friend bool operator!=(const Iterator& a, const Iterator& b) {
			return !(a == b);
		};
	private:
		u64 m_index;
		const EntityBitField& m_existance_map;
		const std::vector<u16>& m_versions;
	};

	Iterator begin();
	Iterator end();

private:
	void Resize(u64 size);

	EntityBitField m_existance_map;
	std::vector<u16> m_versions;

	using ComponentExistanceMap = StaticBitfield<u8, 2>;
	std::vector<ComponentExistanceMap> m_components_map;

	u64 m_num_component_types = 0;
	std::vector<std::function<void(Entity)>> m_component_destroy_callbacks;

	std::vector<Transform> m_transforms;
	std::vector<DirtyFlagBits> m_dirty_flags;

	Event<void(Entity)> m_on_entity_created;
	Event<void(Entity)> m_on_entity_destroyed;
};

class EntityTransform {
public:
	EntityTransform(
		Transform& m_transform,
		EntityManager::DirtyFlagBits& m_dirty_flags
	);

	void SetTransform(const Transform& transform);
	void SetPosition(const glm::vec3& position);
	void SetOrientation(const glm::quat& orientation);
	void SetScale(const glm::vec3& scale);

	void SetTransformInterpolated(const Transform& transform);
	void SetPositionInterpolated(const glm::vec3& position);
	void SetOrientationInterpolated(const glm::quat& orientation);
	void SetScaleInterpolated(const glm::vec3& scale);

	const Transform& GetTransform() const;
	const glm::vec3& GetPosition() const;
	const glm::quat& GetOrientation() const;
	const glm::vec3& GetScale() const;
private:
	Transform& m_transform;
	EntityManager::DirtyFlagBits& m_dirty_flags;
};
