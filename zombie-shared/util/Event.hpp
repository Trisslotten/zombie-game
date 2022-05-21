#pragma once

#include <functional>
#include <vector>

template <typename T>
class Event;

template <typename Ret, typename ... Args>
class Event<Ret(Args...)> {
public:
	void Listen(std::function<Ret(Args...)> func) {
		m_listeners.push_back({ func });
	}

	void Invoke(Args... args) {
		for (const auto& listener : m_listeners) {
			listener.function(args...);
		}
	}
private:
	struct Listener {
		std::function<Ret(Args...)> function;
	};

	std::vector<Listener> m_listeners;
};
