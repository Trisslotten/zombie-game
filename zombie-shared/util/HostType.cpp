#include "HostType.hpp"

namespace {
	enum class HostType {
		SERVER = 0,
		CLIENT = 1,
	};

	HostType host_type;
}

bool Host::IsServer() {
	return host_type == HostType::SERVER;
}

bool Host::IsClient() {
	return host_type == HostType::CLIENT;
}

void HostSetters::SetToServer() {
	host_type = HostType::SERVER;
}

void HostSetters::SetToClient() {
	host_type = HostType::CLIENT;
}
