#pragma once
#ifndef DVZ_BASICRENDERER_HPP
#define DVZ_BASICRENDERER_HPP

#include "client/graphics/MeshCache.hpp"
#include "client/graphics/TEXCache.hpp"
#include "client/graphics/ShaderSet.hpp"
#include "client/graphics/Scene.hpp"

#include <glm/glm.hpp>

namespace DVZ::Graphics {
	class BasicRenderer {
	public:
		MeshCache meshCache;
		TEXCache texCache;

		BasicRenderer();
		void render(const Scene& scene);

	private:
		void prerender();

		ShaderSet ss;
		glm::mat4 projection;
	};
}

#endif