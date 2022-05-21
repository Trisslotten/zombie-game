#include "DebugCameraSystem.hpp"

#include "engine/SystemManager.hpp"
//#include "entity/EntityManager.hpp"

DebugCameraSystem::DebugCameraSystem(SystemManager& system_manager)
	: m_components{ system_manager.Get<EntityManager>() }
{
	system_manager.OnUpdate().Listen([this](float dt) {
		Update(dt);
	});
}

void DebugCameraSystem::Update(float dt) {

}