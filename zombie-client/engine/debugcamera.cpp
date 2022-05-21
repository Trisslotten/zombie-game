#include "debugcamera.hpp"

#include <iostream>

#include "Input.hpp"
#include "engine/SystemManager.hpp"

DebugCamera::DebugCamera(SystemManager& system_manager)
	: m_input{ system_manager.Get<Input>() }
{
	system_manager.OnUpdate().Listen([this](float dt) { Update(dt); });
}

void DebugCamera::Update(float dt) {
	bool currently_looking = m_input.Has(InputAction::DebugLooking);
	if (currently_looking && !m_looking) {
		m_input.SetMouseLocked(true);
		m_looking = true;
	} else if (!currently_looking && m_looking) {
		m_input.SetMouseLocked(false);
		m_looking = false;
	}

	if (m_looking) {
		auto mouse_movement = m_input.GetMouseMovement();
		m_yaw -= mouse_movement.x * m_sensitivity;
		m_pitch += mouse_movement.y * m_sensitivity;

		m_pitch = glm::clamp(m_pitch, -0.49f * glm::pi<float>(), 0.49f * glm::pi<float>());
		m_yaw = glm::mod(m_yaw, 2.0f * glm::pi<float>());
	}

	glm::quat orientation{ glm::vec3(0, m_pitch,m_yaw) };
	glm::vec3 forward = orientation * glm::vec3(1, 0, 0);
	glm::vec3 left = orientation * glm::vec3(0, 1, 0);
	glm::vec3 up = orientation * glm::vec3(0, 0, 1);

	float speed = 5.0f;

	glm::vec3 direction{ 0,0,0 };
	if (m_input.Has(InputAction::MoveForward)) {
		direction += forward;
	}
	if (m_input.Has(InputAction::MoveBackward)) {
		direction -= forward;
	}
	if (m_input.Has(InputAction::MoveLeft)) {
		direction += left;
	}
	if (m_input.Has(InputAction::MoveRight)) {
		direction -= left;
	}
	if (m_input.Has(InputAction::MoveUp)) {
		direction += up;
	}
	if (m_input.Has(InputAction::MoveDown)) {
		direction -= up;
	}

	if (direction != glm::vec3(0)) {
		m_move_time += dt;
		speed += 5.f * m_move_time;

		if (m_input.Has(InputAction::MoveFaster)) {
			speed *= 5.f;
		}

		m_position += glm::normalize(direction) * speed * dt;
	} else {
		m_move_time /= 1.f + 5.f * dt;
	}
}

Graphics::Camera DebugCamera::CalculateCamera() const {
	return Graphics::Camera{ m_position, glm::vec3(0, m_pitch, m_yaw) };
}
