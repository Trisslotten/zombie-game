#pragma once

namespace Host {
	bool IsServer();
	bool IsClient();
}

namespace HostSetters {
	void SetToServer();
	void SetToClient();
}
