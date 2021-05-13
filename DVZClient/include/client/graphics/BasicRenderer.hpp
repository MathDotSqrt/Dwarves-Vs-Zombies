#pragma once
#ifndef DVZ_BASICRENDERER_HPP
#define DVZ_BASICRENDERER_HPP

#include "client/graphics/MeshCache.hpp"
#include "client/graphics/TEXCache.hpp"
#include "client/graphics/ShaderSet.hpp"

#include <glm/glm.hpp>

namespace DVZ::Graphics {
	class BasicRenderer {
	public:
		BasicRenderer();
		void render();

	private:
		void prerender();

		MeshCache cache;
		TEXCache texCache;
		ShaderSet ss;
		glm::mat4 projection;
	};
}

#endif