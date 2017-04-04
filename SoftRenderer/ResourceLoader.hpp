#pragma once
#include "Common.hpp"
#include "Texture2D.hpp"

namespace X
{
	class ResourceLoader
	{
	public:
		ResourceLoader(std::string rootPath);
		~ResourceLoader();

		bool AddResourceLocation(std::string path);

		std::shared_ptr<ConcreteTexture2D<f32V3>> LoadTexture(std::string const& path);
		std::unique_ptr<Mesh> LoadMesh(std::string const& path);

	private:
		struct Impl;
		std::unique_ptr<Impl> impl;
	};
}


