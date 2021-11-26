#ifndef DVZ_PREDICT_CLIENT_HPP
#define DVZ_PREDICT_CLIENT_HPP

#include "client/engine.hpp"
#include "core/CoreComponents.hpp"

namespace DVZ {
	struct ClientPlayerState {
		Transformation transform;
		glm::vec3 velocity;
	};

	ClientPlayerState predict_client_player_state(const Engine& engine, const MovementState& state, const Transformation& transform);
}

#endif 