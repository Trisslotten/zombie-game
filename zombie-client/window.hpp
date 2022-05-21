#pragma once

#include <glad/glad.h>
#include <functional>
#include <glm/vec2.hpp>

#include "util/IntTypes.hpp"
#include "util/Event.hpp"

struct GLFWwindow;
class Input;
class SystemManager;

class Window final {
public:
	Window(SystemManager& system_manager, glm::ivec2 size);
	~Window();
	void Update(float dt);
	bool ShouldClose() const;

	Event<void(glm::ivec2)>& OnResized();
	Event<void(bool)>& OnFocusChanged();
	glm::ivec2 GetSize() const;
private:
	GLFWwindow* m_glfw_window = nullptr;
	Input* m_input = nullptr;

	Event<void(glm::ivec2)> m_resized_callbacks;
	Event<void(bool)> m_focus_changed_callbacks;

	friend class Input;
	friend class WindowCallbacks;
	friend class InputCallbacks;
};
