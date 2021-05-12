#pragma once 

#ifndef DVZ_MESHCACHE_HPP
#define DVZ_MESHCACHE_HPP

#include <entt/resource/cache.hpp>
#include <entt/resource/loader.hpp>
#include <entt/core/hashed_string.hpp>


#include "client/graphics/Attrib.hpp"
#include "client/graphics/Geometry.hpp"
#include "client/graphics/Mesh.hpp"

namespace DVZ::Graphics {
	struct MeshLoader : entt::resource_loader<MeshLoader, Mesh> {
		/*Geometry<PositionAttrib, NormalAttrib, TexcoordAttrib> loadModel(std::string filename, glm::vec3 offset) const {
			static Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate);
			assert(scene);

			const auto mesh = scene->mMeshes[0];

			const auto num_vectices = mesh->mNumVertices;
			const auto num_indices = mesh->mNumFaces * 3;

			Geometry<PositionAttrib, NormalAttrib, TexcoordAttrib> geometry;
			geometry.resizeVerticies(num_vectices);
			geometry.resizeIndices(num_indices);

			for (int i = 0; i < num_vectices; i++) {
				const auto p = mesh->mVertices[i];
				const auto n = mesh->mNormals[i];
				const auto t = mesh->mTextureCoords[0][i];

				geometry.pushVertex(glm::vec3(p.x, p.y, p.z) + offset, glm::vec3(n.x, n.y, n.z), glm::vec2(t.x, t.y));
			}

			for (int i = 0; i < mesh->mNumFaces; i++) {
				const auto face = mesh->mFaces[i];
				const auto indices = face.mIndices;
				geometry.pushTriangle(indices[0], indices[1], indices[2]);
			}
			return geometry;
		}

		std::shared_ptr<Mesh> load(std::string filename, glm::vec3 offset = glm::vec3(0)) const {
			return std::make_shared<Mesh>(loadModel(filename, offset));
		}*/

		template<typename ...T>
		std::shared_ptr<Mesh> load(const Geometry<T...>& geometry) const {
			return std::make_shared<Mesh>(geometry);
		}

	};

	using MeshCache = entt::resource_cache<Mesh>;
}

#endif