#pragma once

#ifndef DVZ_SHADERSET_HPP
#define DVZ_SHADERSET_HPP

#include <memory>
#include <unordered_map>
#include <set>

#include "client/graphics/GLSLProgram.hpp"
#include <entt/resource/loader.hpp>
#include <entt/resource/cache.hpp>

namespace DVZ::Graphics {
	class ShaderLoader : public entt::resource_loader<ShaderLoader, GLSLProgram> {
	public:
		std::shared_ptr<GLSLProgram> load(const std::string& vert, const std::string& frag) const;
		std::shared_ptr<GLSLProgram> load(std::shared_ptr<GLSLProgram> program) const;
	};

	class ShaderSet {

	public:

		entt::resource_handle<GLSLProgram>
			get(entt::id_type shaderID);

		entt::resource_handle<GLSLProgram>
			load(entt::id_type shaderID, std::string vert, std::string frag);

		void reload(std::string shaderFile);
	private:
		using ShaderCache = entt::resource_cache<GLSLProgram>;
		ShaderCache cache;

		std::set<std::string> loadedFiles;
		std::unordered_map<entt::id_type, std::pair<std::string, std::string>> loadedShaders;
	};
}

#endif