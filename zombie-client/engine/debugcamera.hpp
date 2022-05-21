#pragma once

#include "graphics/camera.hpp"

class Input;
class SystemManager;

class DebugCamera {
public:
	DebugCamera(SystemManager& system_manager);

	Graphics::Camera CalculateCamera() const;
private:
	void Update(float dt);

	Input& m_input;

	bool m_looking = false;

	glm::vec3 m_position = glm::vec3(5,0,100);
	float m_move_time = 0.f;
	float m_sensitivity = 0.0025f;
	float m_yaw = 0.f;
	float m_pitch = 0.f;
};
