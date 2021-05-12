#pragma once

#ifndef DVZ_ENGINE_HPP
#define DVZ_ENGINE_HPP

#include "client/graphics/BasicRenderer.hpp"

namespace DVZ{
	class Engine {
	public:
		Engine();

		void update(float delta);
		void render();

	private:
		Graphics::BasicRenderer renderer;
	};
}

#endif