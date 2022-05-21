#include "Window.hpp"

#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "Input.hpp"
#include "util/Log.h"
#include "engine/SystemManager.hpp"

class WindowCallbacks {
public:
   static void ErrorCallback(i32 error, const char* description) {
      std::cerr << "GLFW Error: " << description << std::endl;
   }
   static void FrameBufferSizeCallback(GLFWwindow* glfw_window, i32 width, i32 height) {
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
		window->OnResized().Invoke({ width, height });
   }
   static void WindowFocusCallback(GLFWwindow* glfw_window, int focused) {
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
		window->OnFocusChanged().Invoke(focused);
   }
private:
   WindowCallbacks() = default;
};


Window::Window(SystemManager& system_manager, glm::ivec2 size) {
	system_manager.OnUpdate().Listen([this](float dt) { Update(dt); });

   glfwSetErrorCallback(&WindowCallbacks::ErrorCallback);

   if (!glfwInit()) {
      // error
      return;
   }

   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   m_glfw_window = glfwCreateWindow(size.x, size.y, "zombie", NULL, NULL);

   if (!m_glfw_window) {
      glfwTerminate();
      // error
      return;
   }
   glfwSetWindowUserPointer(m_glfw_window, this);
   glfwMakeContextCurrent(m_glfw_window);

	m_input = &system_manager.Add<Input>(m_glfw_window);

   if (!gladLoadGL()) {
      glfwTerminate();
      // error
      return;
   }

	Log::Info() << "Using OpenGL " << glGetString(GL_VERSION);

   //glfwSetInputMode(m_glfw_window, GLFW_STICKY_KEYS, GLFW_TRUE);
   glfwSetFramebufferSizeCallback(m_glfw_window, &WindowCallbacks::FrameBufferSizeCallback);
   glfwSetWindowFocusCallback(m_glfw_window, &WindowCallbacks::WindowFocusCallback);
   glfwSwapInterval(1);
}

Window::~Window() {
   glfwTerminate();
   m_glfw_window = nullptr;
};

Event<void(glm::ivec2)>& Window::OnResized() {
	return m_resized_callbacks;
}

Event<void(bool)>& Window::OnFocusChanged() {
	return m_focus_changed_callbacks;
}

glm::ivec2 Window::GetSize() const {
	glm::ivec2 size;
	glfwGetWindowSize(m_glfw_window, &size.x, &size.y);
	return size;
}

bool Window::ShouldClose() const {
   if (m_glfw_window) {
      return glfwWindowShouldClose(m_glfw_window);
   }
   return true;
}

void Window::Update(float dt) {
   if (m_glfw_window) {
      m_input->Update();
      glfwSwapBuffers(m_glfw_window);
      glfwPollEvents();
   }
}
