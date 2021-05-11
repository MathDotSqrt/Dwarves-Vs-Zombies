#pragma once

#ifndef DVZ_ENGINE_HPP
#define DVZ_ENGINE_HPP

namespace DVZ{
	class Engine {
	public:
		Engine();

		void update(float delta);
		void render();
	};
}

#endif