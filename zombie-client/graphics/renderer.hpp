#pragma once

#include "shader.hpp"
#include "camera.hpp"
#include "heightmapmesh.hpp"
#include "util/IntTypes.hpp"

class Window;
class SystemManager;

namespace Graphics {

class BoxMesh {
public:
	void Initialize();
	void Draw();
private:
	u32 m_vao = 0;
	u32 m_vbo = 0;
};

class Renderer final {
public:
	Renderer(SystemManager& system_manager);
	~Renderer();

	void SetCamera(const Camera& camera);
	void ReloadShaders();

	void Render(float);
private:
	void Initialize();
	void SetViewportSize(glm::ivec2 size);

	glm::ivec2 m_viewport_size;

	ShaderProgram m_shader;
	ShaderProgram m_heightmap_shader;

	BoxMesh m_box_mesh;
	HeightmapMesh m_heightmap_mesh;

	HeightmapTexture m_height_texture;

	Camera m_camera;
};

}
