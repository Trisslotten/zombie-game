#include "Input.hpp"

#include "Window.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

class InputCallbacks {
public:
	static void KeyCallback(GLFWwindow* glfw_window, i32 key, i32 scancode, i32 action, i32 mods) {
		Input* input = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window))->m_input;

		if (key != GLFW_KEY_UNKNOWN && action != GLFW_REPEAT) {
			auto& key_state = input->m_key_states[key];
			key_state.current_state = action;
			key_state.num_state_changes++;
		}
	}

	static void MouseCallback(GLFWwindow* glfw_window, i32 button, i32 action, i32 mods) {
		Input* input = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window))->m_input;

		auto& mouse_button_state = input->m_mouse_button_states[button];
		mouse_button_state.current_state = action;
		mouse_button_state.num_state_changes++;
	}
private:
	InputCallbacks() = default;
};

Keybind::Keybind() : key{ GLFW_KEY_UNKNOWN }, type{ InputType::Down } {}

Input::Input(GLFWwindow* glfw_window) : m_glfw_window{ glfw_window } {
	glfwSetKeyCallback(glfw_window, &InputCallbacks::KeyCallback);
	glfwSetMouseButtonCallback(glfw_window, &InputCallbacks::MouseCallback);

	m_keybinds[InputAction::MoveForward] = GLFW_KEY_W;
	m_keybinds[InputAction::MoveBackward] = GLFW_KEY_S;
	m_keybinds[InputAction::MoveLeft] = GLFW_KEY_A;
	m_keybinds[InputAction::MoveRight] = GLFW_KEY_D;
	m_keybinds[InputAction::MoveUp] = GLFW_KEY_SPACE;
	m_keybinds[InputAction::MoveDown] = GLFW_KEY_LEFT_CONTROL;
	m_keybinds[InputAction::MoveFaster] = GLFW_KEY_LEFT_SHIFT;

	m_keybinds[InputAction::DebugReloadShaders] = { GLFW_KEY_F5, InputType::Released };

	m_keybinds[InputAction::DebugLooking] = { GLFW_MOUSE_BUTTON_RIGHT, InputType::Down, InputDevice::Mouse };

	m_key_states.resize(GLFW_KEY_LAST);
	m_mouse_button_states.resize(GLFW_MOUSE_BUTTON_LAST);
}

glm::vec2 Input::GetMousePosition() const {
   return glm::vec2(m_mouse_position);
}

glm::vec2 Input::GetMouseMovement() const {
   return GetMousePosition() - m_last_mouse_position;
}

void Input::SetMouseLocked(bool locked) {
	m_mouse_locked = locked;
	if (m_glfw_window) {
		auto mode = locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
		glfwSetInputMode(m_glfw_window, GLFW_CURSOR, mode);

	}
}

void Input::Update() {
	m_last_mouse_position = m_mouse_position;
	glfwGetCursorPos(m_glfw_window, &m_mouse_position.x, &m_mouse_position.y);

	if (m_mouse_was_locked && !m_mouse_locked) {
		m_last_mouse_position = m_mouse_position;
	}
	m_mouse_was_locked = m_mouse_locked;

	for (auto& key_state : m_key_states) {
		key_state.num_state_changes = 0;
	}
	for (auto& mouse_button_state : m_mouse_button_states) {
		mouse_button_state.num_state_changes = 0;
	}
}

bool Input::Has(InputAction action) const {
	auto& keybind = m_keybinds[action];
	if (keybind.type != InputType::None && keybind.key != GLFW_KEY_UNKNOWN) {
		auto& states = keybind.input_device == InputDevice::Keyboard ? m_key_states : m_mouse_button_states;
		auto& state = states[keybind.key];

		switch (keybind.type) {
		case InputType::Down:
			return state.current_state == GLFW_PRESS || state.num_state_changes >= 2;
		case InputType::Pressed:
			return state.current_state == GLFW_PRESS && state.num_state_changes >= 1 || state.num_state_changes >= 2;
		case InputType::Released:
			return state.current_state == GLFW_RELEASE && state.num_state_changes >= 1;
		default:
			std::cerr << "ERROR: invalid input type" << std::endl;
			return false;
		}
	} else {
		return false;
	}
}
