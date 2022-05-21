#include "renderer.hpp"

#include <glm/gtx/transform.hpp>

#include "Window.hpp"
#include "engine/heightmap.hpp"
#include "engine/SystemManager.hpp"

using namespace Graphics;

Renderer::Renderer(SystemManager& system_manager)
	: m_camera{ glm::vec3(0), glm::quat() }
{
	auto& window = system_manager.Get<Window>();

	SetViewportSize(window.GetSize());
	window.OnResized().Listen([this](glm::ivec2 size) {
		SetViewportSize(size);
	});

	system_manager.OnInitialize().Listen([this]() { Initialize(); });
	system_manager.OnUpdate().Listen([this](float dt) { Render(dt); });
}

Renderer::~Renderer() = default;

void Renderer::SetCamera(const Camera& camera) {
	m_camera = camera;
}

void Graphics::Renderer::ReloadShaders() {
	m_heightmap_shader.Reload();
	m_shader.Reload();
}

void Renderer::Initialize() {
	glClearColor(216.f/255.f, 242.f / 255.f, 255.f / 255.f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	{
		m_shader.Add("shader.vert");
		m_shader.Add("shader.frag");
		m_shader.Compile();

		m_heightmap_shader.Add("heightmap.vert");
		m_heightmap_shader.Add("heightmap.frag");
		m_heightmap_shader.Compile();
	}

	m_box_mesh.Initialize();
	m_heightmap_mesh.Initialize();


	Heightmap heightmap;
	heightmap.Generate();

	m_height_texture.Initialize(heightmap);
}

void Graphics::Renderer::SetViewportSize(glm::ivec2 size) {
	m_viewport_size = size;
	glViewport(0, 0, size.x, size.y);
}

void Renderer::Render(float) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const glm::vec3 forward{ 1, 0, 0 };
	const glm::vec3 up{ 0, 0, 1 };

	float aspect = float(m_viewport_size.x) / float(m_viewport_size.y);
	glm::mat4 projection = glm::infinitePerspective(glm::radians(80.0f), aspect, 0.5f);
	glm::mat4 view = glm::lookAt(m_camera.position, m_camera.orientation * forward + m_camera.position, up);
	glm::mat4 camera = projection * view;

	{

		glDisable(GL_CULL_FACE);
		static float angle = 0.0f;
		angle += 0.01f;

		m_shader.Use();
		m_shader.Uniform("camera", camera);

		auto drawBox = [this](glm::vec3 pos, glm::vec3 size, glm::vec3 color) {
			auto translation = glm::translate(pos + glm::vec3(0,0,70));
			auto scale = glm::scale(size);
			glm::mat4 transform = translation * scale;
			m_shader.Uniform("transform", transform);
			m_shader.Uniform("color", color);
			m_box_mesh.Draw();
		};

		drawBox(glm::vec3(0,0,3.1), glm::vec3(5, 6.6, 5), glm::vec3(0.3));
		drawBox(glm::vec3(2.5, 1.5, 3.1 - 2.5 + 1), glm::vec3(0.2, 1, 2), glm::vec3(0.4));
		drawBox(glm::vec3(2.5, -1.5, 3.1 - 2.5 + 1.5), glm::vec3(0.2, 1, 1), glm::vec3(0.4,0.4,0.8));
		drawBox(glm::vec3(2.5, -1.5, 3.1 + 1), glm::vec3(0.2, 1, 1), glm::vec3(0.4, 0.4, 0.8));
		drawBox(glm::vec3(2.5, 1.5, 3.1 + 1), glm::vec3(0.2, 1, 1), glm::vec3(0.4, 0.4, 0.8));
	}

	{
		glEnable(GL_CULL_FACE);
		m_height_texture.Bind(0);
		m_heightmap_shader.Use();
		m_heightmap_shader.Uniform("camera", camera);
		m_heightmap_shader.Uniform("heightmap", 0);
		m_heightmap_mesh.Draw();
	}
}

void BoxMesh::Initialize() {
	float vertices[] = {
		 -0.5f, -0.5f, -0.5f,
		  0.5f, -0.5f, -0.5f,
		  0.5f,  0.5f, -0.5f,
		  0.5f,  0.5f, -0.5f,
		 -0.5f,  0.5f, -0.5f,
		 -0.5f, -0.5f, -0.5f,

		 -0.5f, -0.5f,  0.5f,
		  0.5f, -0.5f,  0.5f,
		  0.5f,  0.5f,  0.5f,
		  0.5f,  0.5f,  0.5f,
		 -0.5f,  0.5f,  0.5f,
		 -0.5f, -0.5f,  0.5f,

		 -0.5f,  0.5f,  0.5f,
		 -0.5f,  0.5f, -0.5f,
		 -0.5f, -0.5f, -0.5f,
		 -0.5f, -0.5f, -0.5f,
		 -0.5f, -0.5f,  0.5f,
		 -0.5f,  0.5f,  0.5f,

		  0.5f,  0.5f,  0.5f,
		  0.5f,  0.5f, -0.5f,
		  0.5f, -0.5f, -0.5f,
		  0.5f, -0.5f, -0.5f,
		  0.5f, -0.5f,  0.5f,
		  0.5f,  0.5f,  0.5f,

		 -0.5f, -0.5f, -0.5f,
		  0.5f, -0.5f, -0.5f,
		  0.5f, -0.5f,  0.5f,
		  0.5f, -0.5f,  0.5f,
		 -0.5f, -0.5f,  0.5f,
		 -0.5f, -0.5f, -0.5f,

		 -0.5f,  0.5f, -0.5f,
		  0.5f,  0.5f, -0.5f,
		  0.5f,  0.5f,  0.5f,
		  0.5f,  0.5f,  0.5f,
		 -0.5f,  0.5f,  0.5f,
		 -0.5f,  0.5f, -0.5f
	};

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	glBindVertexArray(m_vao);
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}
	glBindVertexArray(0);
}

void BoxMesh::Draw() {
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
