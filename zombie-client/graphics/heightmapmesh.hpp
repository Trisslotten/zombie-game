#pragma once

#include "util/IntTypes.hpp"

class Heightmap;

namespace Graphics {

class HeightmapMesh {
public:
	void Initialize();
	void Draw();
private:
	u32 m_vao = 0;
	u32 m_vbo = 0;

	u32 m_vertex_count = 0;
};

class HeightmapTexture {
public:
	void Initialize(const Heightmap& heightmap);

	void Bind(u32 texture_slot);
private:
	u32 m_texture_id = 0;
};

}