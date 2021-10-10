#include "core/voxel/VoxelCollision.hpp"

using namespace DVZ;
using namespace DVZ::Voxel;

Collision::AABB get_swept_broadphase_box(const Collision::AABB& aabb, const glm::vec3& vel) {
	const auto new_min = glm::min(aabb.min, aabb.min + vel);
	const auto new_max = glm::max(aabb.max, aabb.max + vel);

	return Collision::AABB{ new_min, new_max };
}

std::vector<std::pair<WorldCoords, BlockType>> DVZ::Voxel::broadphase(
	const Collision::AABB& worldspace_aabb, const glm::vec3& vel, const GetBlockFunc& getBlock
) {
	//fine packing because same cache line
	std::vector<std::pair<WorldCoords, BlockType>> potential_collisions;

	const auto broadphase_box = get_swept_broadphase_box(worldspace_aabb, vel);
	const WorldCoords block_min = Voxel::toWorldCoords(broadphase_box.min);
	const WorldCoords block_max = Voxel::toWorldCoords(broadphase_box.max);

	for (int x = block_min.x; x <= block_max.x; x++) {
		for (int z = block_min.z; z <= block_max.z; z++) {
			for (int y = block_min.y; y <= block_max.y; y++) {
				const WorldCoords block_coord(x, y, z);
				const auto block = getBlock(block_coord);
				if (Voxel::getBlockData(block).meshType == Voxel::MeshType::MESH_TYPE_CUBE) {
					potential_collisions.emplace_back(block_coord, block);
				}
			}
		}
	}

	return potential_collisions;
}

float line_to_plane(
	const glm::vec3& p,
	const glm::vec3& u,
	const glm::vec3& v,
	const glm::vec3& normal
) {
	const float n_dot_u = glm::dot(u, normal);
	
	if (n_dot_u == 0) return std::numeric_limits<float>::infinity();

	return glm::dot(normal, v - p) / n_dot_u;
}

bool between(float x, float a, float b) {
	return x >= a && x <= b;
}

float swept_aabb(
	const glm::vec3& vel,
	const Collision::AABB& moving_aabb,
	const Collision::AABB& static_aabb,
	glm::vec3& normal
) {

	const auto minkowski_min = static_aabb.min - moving_aabb.max;
	const auto minkowski_width = (moving_aabb.max - moving_aabb.min) + (static_aabb.max - static_aabb.min);

	const Collision::AABB minkowsi{ minkowski_min, minkowski_min + minkowski_width };

	float collision_time = 1;
	normal = glm::vec3();

	//X MIN
	if (vel.x > 0) {
		constexpr glm::vec3 NEG_X{ -1, 0, 0 };
		const auto nearest_point = minkowsi.min;
		const auto s = line_to_plane(glm::vec3(), vel, nearest_point, NEG_X);

		bool a = (s >= 0 && s < collision_time);
		bool between_y = between(s * vel.y, minkowsi.min.y, minkowsi.max.y);
		bool between_z = between(s * vel.z, minkowsi.min.z, minkowsi.max.z);
		if (a && between_y && between_z) {
			collision_time = s;
			normal = NEG_X;
		}
	}

	//X MAX
	if (vel.x < 0) {
		constexpr glm::vec3 POS_X{ 1, 0, 0 };
		const auto nearest_point = glm::vec3(minkowsi.max.x, minkowsi.min.y, minkowsi.min.z);;

		const auto s = line_to_plane(glm::vec3(), vel, nearest_point, POS_X);

		bool a = (s >= 0 && s < collision_time);
		bool between_y = between(s * vel.y, minkowsi.min.y, minkowsi.max.y);
		bool between_z = between(s * vel.z, minkowsi.min.z, minkowsi.max.z);
		if (a && between_y && between_z) {
			collision_time = s;
			normal = POS_X;
		}
	}

	//Y MIN
	if (vel.y > 0) {
		constexpr glm::vec3 NEG_Y{ 0, -1, 0 };
		const auto nearest_point = minkowsi.min;
		const auto s = line_to_plane(glm::vec3(), vel, nearest_point, NEG_Y);

		bool a = (s >= 0 && s < collision_time);
		bool between_x = between(s * vel.x, minkowsi.min.x, minkowsi.max.x);
		bool between_z = between(s * vel.z, minkowsi.min.z, minkowsi.max.z);
		if (a && between_x && between_z) {
			collision_time = s;
			normal = NEG_Y;
		}
	}

	//Y MAX
	if (vel.y < 0) {
		constexpr glm::vec3 POS_Y{ 0, 1, 0 };
		const auto nearest_point = glm::vec3(minkowsi.min.x, minkowsi.max.y, minkowsi.min.z);;

		const auto s = line_to_plane(glm::vec3(), vel, nearest_point, POS_Y);

		bool a = (s >= 0 && s < collision_time);
		bool between_x = between(s * vel.x, minkowsi.min.x, minkowsi.max.x);
		bool between_z = between(s * vel.z, minkowsi.min.z, minkowsi.max.z);
		if (a && between_x && between_z) {
			collision_time = s;
			normal = POS_Y;
		}
	}

	//Z MIN
	if (vel.z > 0) {
		constexpr glm::vec3 NEG_Z{ 0, 0, -1 };
		const auto nearest_point = minkowsi.min;
		const auto s = line_to_plane(glm::vec3(), vel, nearest_point, NEG_Z);

		bool a = (s >= 0 && s < collision_time);
		bool between_x = between(s * vel.x, minkowsi.min.x, minkowsi.max.x);
		bool between_y = between(s * vel.y, minkowsi.min.y, minkowsi.max.y);
		if (a && between_x && between_y) {
			collision_time = s;
			normal = NEG_Z;
		}
	}

	//Z MAX
	if (vel.z < 0) {
		constexpr glm::vec3 POS_Z{ 0, 0, 1 };
		const auto nearest_point = glm::vec3(minkowsi.min.x, minkowsi.min.y, minkowsi.max.z);;

		const auto s = line_to_plane(glm::vec3(), vel, nearest_point, POS_Z);

		bool a = (s >= 0 && s < collision_time);
		bool between_x = between(s * vel.x, minkowsi.min.x, minkowsi.max.x);
		bool between_y = between(s * vel.y, minkowsi.min.y, minkowsi.max.y);
		if (a && between_x && between_y) {
			collision_time = s;
			normal = POS_Z;
		}
	}

	return collision_time;
}

std::tuple<float, glm::vec3, BlockType>
calc_nearest_swept_aabb(
	const glm::vec3& vel_delta,
	const Collision::AABB& moving_aabb,
	std::vector<std::pair<WorldCoords, BlockType>>& broadphase
) {

	int index = 0;
	std::tuple<float, glm::vec3, BlockType> collision{ 0.0f, glm::vec3(), BlockType::AIR };
	for (int i = 0; i < broadphase.size(); i++) {
		const auto& element = broadphase[i];
		const auto& block_coord = element.first;
		const auto block = element.second;
		const auto block_aabb = Collision::AABB(block_coord, glm::vec3(block_coord) + glm::vec3(1));


		glm::vec3 normal;
		const float collision_time = swept_aabb(vel_delta, moving_aabb, block_aabb, normal);

		if (i == 0 || std::get<0>(collision) > collision_time) {
			index = i;
			std::get<0>(collision) = collision_time;
			std::get<1>(collision) = normal;
			std::get<2>(collision) = block;
		}
	}

	if (broadphase.size() > 0) {
		broadphase.erase(broadphase.begin() + index);
	}

	return collision;
}

glm::vec3 DVZ::Voxel::sample_terrain_collision(
	const glm::vec3& pos,
	const glm::vec3& vel,
	const Collision::AABB& aabb,
	std::vector<std::pair<WorldCoords, BlockType>>& broadphase
) {
	//sample = Component::VoxelCollisionSample();

	auto new_pos = pos;
	auto new_vel_delta = vel;
	while (broadphase.size()) {
		auto worldspace_aabb = aabb;
		worldspace_aabb.min += new_pos;
		worldspace_aabb.max += new_pos;

		const auto collision = calc_nearest_swept_aabb(new_vel_delta, worldspace_aabb, broadphase);
		const float collision_time = std::get<0>(collision);
		const auto& normal = std::get<1>(collision);

		new_pos += new_vel_delta * collision_time + normal * 0.0001f;

		if (collision_time < 1) {
			const float n_dot_n = glm::dot(normal, normal);
			if (n_dot_n != 0) {
				const float remaining_time = 1 - collision_time;
				const float a_dot_n = remaining_time * (glm::dot(new_vel_delta, normal));
				new_vel_delta = remaining_time * new_vel_delta - (a_dot_n / n_dot_n) * normal;

				//const Voxel::Block block = std::get<2>(collision);
				//set_sample(block, normal, sample);
			}
		}
		else {
			new_vel_delta = glm::vec3();
			break;
		}
	}
	new_pos += new_vel_delta;
	return (new_pos - pos);
}