#include "engine/Engine.hpp"

#include "util/HostType.hpp"
#include "util/LogLifetime.hpp"

#include "tests/tests.hpp"
#include "tests/benchmarks.hpp"

#include "entity/EntityManager.hpp"

int main() {
	Log::Create();

	Log::LogDebug(false);
	Log::LogError(false);
	Log::LogInfo(false);
	//RunTests();
	//RunBenchmarks();
	Log::LogDebug(true);
	Log::LogError(true);
	Log::LogInfo(true);

	HostSetters::SetToServer();

	Engine engine;

	constexpr float c_update_rate = 30.f;
	constexpr float c_update_time = 1.f / c_update_rate;
	
	FrameLimiter frame_limiter;
	frame_limiter.SetFramerate(c_update_rate);

	engine.Initialize();

	while (engine.IsRunning()) {
		engine.Update(c_update_time);
		frame_limiter.Limit();
	}

	Log::Destroy();
	return 0;
}
