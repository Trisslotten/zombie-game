#pragma once

#include "util/TypeSafeId.hpp"
#include "util/IntTypes.hpp"

class Entity : public TypeSafeId<Entity, u64, std::numeric_limits<u64>::max()> {
public:
	Entity() { m_value = std::numeric_limits<u64>::max(); }
	Entity(u64 index, u16 version) {
		m_value = index | (static_cast<u64>(version) << 48llu);
	}
	u64 GetIndexPart() const {
		return m_value & 0xFFFFFFFFFFFFllu;
	}
	u64 GetVersionPart() const {
		return (m_value >> 48llu) & 0xFFFFllu;
	}
};

namespace std {
	template <>
	struct hash<Entity> {
		size_t operator()(Entity id) const {
			return hash<u64>{}(id.GetInternalValue());
		}
	};
}
