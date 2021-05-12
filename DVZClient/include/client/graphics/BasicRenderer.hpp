#pragma once
#ifndef DVZ_BASICRENDERER_HPP
#define DVZ_BASICRENDERER_HPP

#include "client/graphics/MeshCache.hpp"
#include "client/graphics/ShaderSet.hpp"

namespace DVZ::Graphics {
	class BasicRenderer {
	public:
		BasicRenderer();
		void render();

	private:
		void prerender();

		MeshCache cache;
		ShaderSet ss;

	};
}

#endif