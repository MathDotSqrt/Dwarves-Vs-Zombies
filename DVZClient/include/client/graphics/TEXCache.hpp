#pragma once 

#ifndef DVZ_TEXCACHE_HPP
#define DVZ_TEXCACHE_HPP

#include "client/graphics/TEX.hpp"

#include <entt/resource/cache.hpp>
#include <entt/resource/loader.hpp>

namespace DVZ::Graphics {
	struct TextureLoader : entt::resource_loader<TextureLoader, TEX> {
		std::shared_ptr<TEX> load(std::string filename) const {
			return std::make_shared<TEX>(TEX::Builder().buildTexture(filename));
		}

		std::shared_ptr<TEX> load(TEX::Builder settings, std::string filename) const {
			return std::make_shared<TEX>(settings.buildTexture(filename));
		}

		std::shared_ptr<TEX> load(TEX&& tex) const {
			return std::make_shared<TEX>(std::move(tex));
		}
	};

	using TEXCache = entt::resource_cache<TEX>;
}

#endif