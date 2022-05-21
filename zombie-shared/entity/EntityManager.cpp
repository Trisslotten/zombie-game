#include "EntityManager.hpp"

#include <cassert>
#include <iostream>
#include "util/Underlying.hpp"
#include "util/Log.h"

Entity EntityManager::Iterator::operator*() const {
	return Entity(m_index, m_versions[m_index]);
}

EntityManager::Iterator& EntityManager::Iterator::operator++() {
	do {
		++m_index;
	} while (m_index < m_existance_map.GetSize() && !m_existance_map.Get(m_index));
	return *this;
}

EntityManager::Iterator EntityManager::begin() {
	auto index = 0;
	while (index < m_existance_map.GetSize() && !m_existance_map.Get(index)) {
		++index;
	}
	return Iterator(index, m_existance_map, m_versions);
}

EntityManager::Iterator EntityManager::end() {
	return Iterator(m_existance_map.GetSize(), m_existance_map, m_versions);
}

std::optional<u64> EntityBitField::FindFreeIndex() {
	auto size = m_bitfield.size();
	for (u64 i = 0; i < size; ++i) {
		const auto slot_data = m_bitfield[m_search_index];
		if (slot_data != std::numeric_limits<BlockType>::max()) {
			auto bit = _tzcnt_u64(~slot_data);
			return bit + m_search_index * GetNumBitsPerBlock();
		}
		m_search_index = (m_search_index + 1) % size;
	}
	return std::nullopt;
}

EntityManager::EntityManager() {
	Resize(512);
}

EntityManager::~EntityManager() {
}

Entity EntityManager::Create() {
	u64 index;

	if (auto free_index = m_existance_map.FindFreeIndex()) {
		index = free_index.value();
	} else {
		auto current_size = m_versions.size();
		auto next_size = 2 * current_size;
		Resize(next_size);
		index = m_existance_map.FindFreeIndex().value();

		Log::Debug() << "Out of free entities, resizing from " << current_size << " to " << next_size;
	}

	auto version = m_versions[index];
	auto entity = Entity(index, version);

	m_existance_map.Set(index, true);

	m_on_entity_created.Invoke(entity);

	Log::Debug() << "Created entity " << version << ":" << index;

	return entity;
}

void EntityManager::Add(Entity entity) {
	auto index = entity.GetIndexPart();
	auto version = entity.GetVersionPart();
	m_existance_map.Set(index, true);
	m_versions[index] = static_cast<u16>(version);

	Log::Debug() << "Added entity " << version << ":" << index;
}

void EntityManager::Destroy(Entity entity) {
	auto index = entity.GetIndexPart();
	auto version = entity.GetVersionPart();

	if (!m_existance_map.Get(index)) {
		Log::Error() << "Tried to delete non-existant entity " << version << ":" << index;
		return;
	}
	auto& real_version = m_versions[index];
	if (real_version != version) {
		Log::Error() << "Tried to delete non-existant entity " << version << ":" << index;
		return;
	}

	auto& bits = m_components_map[index];

	constexpr auto bits_per_block = ComponentExistanceMap::GetBitsPerBlock();

	auto num_blocks = DivRoundUp(m_num_component_types, bits.GetBitsPerBlock());
	for (std::size_t block_index = 0; block_index < num_blocks; ++block_index) {
		auto block = bits.GetBlock(block_index);
		if (block != 0) {
			for (std::size_t bit = 0; bit < bits.GetBitsPerBlock(); ++bit) {
				if (block & (1 << bit)) {
					auto index = bit + block_index * bits.GetBitsPerBlock();
					if (m_component_destroy_callbacks[index]) {
						m_component_destroy_callbacks[index](entity);
					}
				}
			}
		}
	}


	//for (u64 i = 0; i < m_num_component_types; ++i) {
	//	if (bits.Get(i) && m_component_destroy_callbacks[i]) {
	//		m_component_destroy_callbacks[i](entity);
	//	}
	//}

	Log::Debug() << "Destroyed entity " << version << ":" << index;

	m_on_entity_destroyed.Invoke(entity);

	m_existance_map.Set(index, false);
	++real_version;
}

Event<void(Entity entity)>& EntityManager::OnEntityCreated() {
	return m_on_entity_created;
}

bool EntityManager::IsDirty(Entity entity, EntityDirtyFlag flag) const {
	auto index = entity.GetIndexPart();
	const auto& dirty_flags = m_dirty_flags[index];
	return dirty_flags.Get(Underlying(flag));
}

Event<void(Entity entity)>& EntityManager::OnEntityDestroyed() {
	return m_on_entity_destroyed;
}

EntityTransform EntityManager::GetTransform(Entity entity) {
	auto index = entity.GetIndexPart();
	auto& transform = m_transforms[index];
	auto& dirty_flags = m_dirty_flags[index];
	return EntityTransform(transform, dirty_flags);
}

bool EntityManager::Has(Entity entity) const {
    auto index = entity.GetIndexPart();
    auto version = entity.GetVersionPart();
	 if (index < m_existance_map.GetSize()) {
		 bool index_exists = m_existance_map.Get(index);
		 bool correct_version = m_versions[index] == version;
		 return index_exists && correct_version;
	 }
    return false;
}

u64 EntityManager::GetFreeComponentTypeIndex() {
	auto new_index = m_num_component_types;

	assert(new_index < ComponentExistanceMap::GetNumBits());

	++m_num_component_types;
	return new_index;
}

void EntityManager::SetDestroyComponentCallback(u64 component, std::function<void(Entity entity)> func) {
	if (component >= m_component_destroy_callbacks.size()) {
		m_component_destroy_callbacks.resize(component + 1);
	}
	m_component_destroy_callbacks[component] = func;
}

void EntityManager::SetHasComponent(Entity entity, u64 component, bool has_component) {
	auto index = entity.GetIndexPart();
	m_components_map[index].Set(component, has_component);
}

bool EntityManager::HasComponent(Entity entity, u64 component) const {
	auto index = entity.GetIndexPart();
	return m_components_map[index].Get(component);
}

void EntityManager::ClearDirtyFlags() {
	for (auto& dirty_flags : m_dirty_flags) {
		dirty_flags.Clear();
	}
}

void EntityManager::Resize(u64 size) {
	auto previous_size = m_versions.size();

	m_existance_map.Resize(size);
	m_versions.resize(size);
	m_transforms.resize(size);
	m_dirty_flags.resize(size);
	m_components_map.resize(size);
	for (std::size_t i = previous_size; i < m_versions.size(); ++i) {
		m_versions[i] = 0;
	}
}

EntityTransform::EntityTransform(
	Transform& transform,
	EntityManager::DirtyFlagBits& dirty_flags
) 
	: m_transform{ transform }
	, m_dirty_flags{ dirty_flags }
{
}

void EntityTransform::SetTransform(const Transform& transform) {
	SetPosition(transform.position);
	SetOrientation(transform.orientation);
	SetScale(transform.scale);
}

void EntityTransform::SetPosition(const glm::vec3& position) {
	if (m_transform.position != position) {
		m_transform.position = position;
		m_dirty_flags.Set(Underlying(EntityDirtyFlag::Position), true);
	}
}

void EntityTransform::SetOrientation(const glm::quat& orientation) {
	if (m_transform.orientation != orientation) {
		m_transform.orientation = orientation;
		m_dirty_flags.Set(Underlying(EntityDirtyFlag::Orientation), true);
	}
}

void EntityTransform::SetScale(const glm::vec3& scale) {
	if (m_transform.scale != scale) {
		m_transform.scale = scale;
		m_dirty_flags.Set(Underlying(EntityDirtyFlag::Scale), true);
	}
}

void EntityTransform::SetTransformInterpolated(const Transform& transform) {
	SetPositionInterpolated(transform.position);
	SetOrientationInterpolated(transform.orientation);
	SetScaleInterpolated(transform.scale);
}

void EntityTransform::SetPositionInterpolated(const glm::vec3& position) {
	if (m_transform.position != position) {
		m_transform.position = position;
		m_dirty_flags.Set(Underlying(EntityDirtyFlag::PositionInterpolate), true);
	}
}

void EntityTransform::SetOrientationInterpolated(const glm::quat& orientation) {
	if (m_transform.orientation != orientation) {
		m_transform.orientation = orientation;
		m_dirty_flags.Set(Underlying(EntityDirtyFlag::OrientationInterpolate), true);
	}
}

void EntityTransform::SetScaleInterpolated(const glm::vec3& scale) {
	if (m_transform.scale != scale) {
		m_transform.scale = scale;
		m_dirty_flags.Set(Underlying(EntityDirtyFlag::ScaleInterpolate), true);
	}
}

const Transform& EntityTransform::GetTransform() const {
	return m_transform;
}

const glm::vec3& EntityTransform::GetPosition() const {
	return m_transform.position;
}

const glm::quat& EntityTransform::GetOrientation() const {
	return m_transform.orientation;
}

const glm::vec3& EntityTransform::GetScale() const {
	return m_transform.scale;
}
