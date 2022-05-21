#pragma once

#include <SFML/Network/Packet.hpp>
#include <vector>
#include <functional>
#include <glm/glm.hpp>

template <class C>
class ComponentNetworking {
public:
	template <class T>
	void RegisterField(T C::*member);

	void Serialize(sf::Packet& packet, const C& component) const;
	void Deserialize(sf::Packet& packet, C& component) const;

private:
	std::vector<std::function<void(sf::Packet&, const C&)>> m_serializers;
	std::vector<std::function<void(sf::Packet&, C&)>> m_deserializers;
};

template<glm::length_t n, typename T, glm::qualifier Q>
sf::Packet& operator<<(sf::Packet& packet, const glm::vec<n, T, Q>& vec) {
	packet.append(&vec, sizeof(vec));
	return packet;
}
template<glm::length_t n, typename T, glm::qualifier Q>
sf::Packet& operator>>(sf::Packet& packet, glm::vec<n, T, Q>& vec) {
	for (int i = 0; i < n; ++i) {
		packet >> vec[i];
	}
	return packet;
}

template<class C> template<class T>
inline void ComponentNetworking<C>::RegisterField(T C::* member) {
	m_serializers.push_back([member](sf::Packet& packet, const C& component) {
		auto& ref = component.*member;
		packet << ref;
	});
	m_deserializers.push_back([member](sf::Packet& packet, C& component) {
		auto& ref = component.*member;
		packet >> ref;
	});
}

template<class C>
inline void ComponentNetworking<C>::Serialize(sf::Packet& packet, const C& component) const  {
	for (const auto& serializer : m_serializers) {
		serializer(packet, component);
	}
}

template<class C>
inline void ComponentNetworking<C>::Deserialize(sf::Packet& packet, C& component) const  {
	for (const auto& deserializer : m_deserializers) {
		deserializer(packet, component);
	}
}
