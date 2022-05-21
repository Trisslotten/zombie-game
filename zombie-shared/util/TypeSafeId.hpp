#pragma once

#include <functional>

template <class Id>
class TypeSafeIdGenerator;

template <class T, class U, U c_invalid_value>
class TypeSafeId {
public:
	TypeSafeId() : m_value{c_invalid_value} {}
	explicit TypeSafeId(U value) : m_value{ value } {}
	bool IsValid() const {
		return m_value != c_invalid_value;
	}
	friend bool operator==(TypeSafeId left, TypeSafeId right) {
		return left.m_value == right.m_value;
	}
	friend bool operator!=(TypeSafeId left, TypeSafeId right) {
		return left.m_value != right.m_value;
	}
	U GetInternalValue() const {
		return m_value;
	}
	explicit operator bool() const {
		return IsValid();
	}
protected:
	U m_value;
};

template <class T, class U, U c_invalid_value>
class TypeSafeIdGenerator<TypeSafeId<T, U, c_invalid_value>> {
public:
	using ID = TypeSafeId<T, U, c_invalid_value>;

	TypeSafeIdGenerator() {}

	ID Generate() {
		return ID(++m_id);
	}
private:
	U m_id = 0;
};

namespace std {
	template <class T, class U, U c_invalid_value>
	struct hash<TypeSafeId<T, U, c_invalid_value>> {
		using ID = TypeSafeId<T, U, c_invalid_value>;
		size_t operator()(ID id) const {
			return hash<U>{}(id.GetInternalValue());
		}
	};
}
