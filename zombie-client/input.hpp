#pragma once

#include <array>
#include <vector>
#include <glm/vec2.hpp>

#include "util/IntTypes.hpp"
#include "util/Underlying.hpp"

struct GLFWwindow;

using InputActionType = u32;
enum class InputAction : InputActionType {
	MoveForward,
	MoveBackward,
	MoveLeft,
	MoveRight,
	MoveUp,
	MoveDown,
	MoveFaster,

	DebugReloadShaders,

	DebugLooking,

	NumActions
};

enum class InputType {
	None,

	Down,
	Pressed,
	Released,
};

enum class InputDevice {
	Keyboard,
	Mouse,
};

struct Keybind {
	i32 key = 0;
	InputType type = InputType::None;
	InputDevice input_device = InputDevice::Keyboard;

	Keybind();
	Keybind(
		i32 key, 
		InputType type = InputType::Down, 
		InputDevice input_device = InputDevice::Keyboard
	) {
		this->key = key;
		this->type = type;
		this->input_device = input_device;
	}
};

class Input final {
public:
	Input(GLFWwindow* glfw_window);

	void Update();

	bool Has(InputAction) const;
	glm::vec2 GetMousePosition() const;
	glm::vec2 GetMouseMovement() const;

	void SetMouseLocked(bool locked);

private:
	GLFWwindow* m_glfw_window;

	struct KeyState {
		i32 current_state = 0;
		i32 num_state_changes = 0;
	};

	struct Keybinds {
		Keybind& operator[](InputAction action) {
			return keybinds[Underlying(action)];
		}
		const Keybind operator[](InputAction action) const {
			return keybinds[Underlying(action)];
		}
		std::array<Keybind, Underlying(InputAction::NumActions)> keybinds;
	};

	Keybinds m_keybinds;
	std::vector<KeyState> m_key_states;
	std::vector<KeyState> m_mouse_button_states;

	glm::dvec2 m_mouse_position{ 0, 0 };
	glm::vec2 m_last_mouse_position{ 0, 0 };
	bool m_mouse_locked = false;
	bool m_mouse_was_locked = false;

	friend class InputCallbacks;
};
