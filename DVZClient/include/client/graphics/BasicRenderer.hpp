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
		void render(const InterpolatedScene& scene);

	private:
		void prerender(const PerspectiveCamera& camera);

		ShaderSet ss;
		glm::mat4 P;
		glm::mat4 V;
	};
}

#endif