#include "engine/Engine.hpp"
#include "engine/DebugCamera.hpp"
#include "graphics/renderer.hpp"
#include "util/HostType.hpp"
#include "util/LogLifetime.hpp"
#include "Window.hpp"
#include "Input.hpp"

int main() {
	Log::Create();

	HostSetters::SetToClient();

	Engine engine(
		[](SystemManager& system_manager) {
			// Also adds Input
			system_manager.Add<Window>(system_manager, glm::ivec2{ 1280, 720 });
			system_manager.Add<Graphics::Renderer>(system_manager);
			system_manager.Add<DebugCamera>(system_manager);
		}
	);

	auto& system_manager = engine.GetSystemManager();
	auto& renderer = system_manager.Get<Graphics::Renderer>();
	auto& debug_camera = system_manager.Get<DebugCamera>();
	engine.GetSystemManager().OnUpdate().Listen([&](float dt) {
		renderer.SetCamera(debug_camera.CalculateCamera());
	});


	engine.Initialize();

	auto& window = engine.GetSystemManager().Get<Window>();

	constexpr auto c_first_frame_dt = std::chrono::milliseconds(16);
	auto previous_time = std::chrono::steady_clock::now() - c_first_frame_dt;

	while (engine.IsRunning() && !window.ShouldClose()) {
		auto now = std::chrono::steady_clock::now();
		auto nano_diff = std::chrono::duration_cast<std::chrono::nanoseconds>(now - previous_time).count();
		float deltatime = static_cast<float>(nano_diff) / static_cast<float>(std::nano::den);

		engine.Update(deltatime);

		previous_time = now;
	}

	Log::Destroy();
	return 0;
}
