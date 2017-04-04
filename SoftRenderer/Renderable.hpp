#pragma once
#include "Common.hpp"
#include "Component.hpp"
#include "Buffer.hpp"

namespace X
{
	struct RenderablePackage
	{
		Renderable* renderable;
		std::shared_ptr<GeometryLayout> layout;
		std::shared_ptr<Material> material;
		RenderablePackage(Renderable& renderable, std::shared_ptr<GeometryLayout> layout, std::shared_ptr<Material> material)
			: renderable(&renderable), layout(layout), material(std::move(material))
		{
		}
		RenderablePackage(RenderablePackage&& right)
			: renderable(right.renderable), layout(right.layout), material(std::move(right.material))
		{
		}
		RenderablePackage& operator =(RenderablePackage&& right)
		{
			renderable = right.renderable;
			layout = std::move(right.layout);
			material = std::move(right.material);
		}
	};

	struct RenderablePackCollector
		: Noncopyable
	{

		void AddPackage(RenderablePackage package)
		{
			packages_.push_back(std::move(package));
		}

		std::vector<RenderablePackage> const& GetAllPackages() const
		{
			return packages_;
		}

		void Clear()
		{
			packages_.clear();
		}
	private:
		std::vector<RenderablePackage> packages_;
	};

	class Renderable
		: public Component
	{
	public:
		Renderable();
		virtual ~Renderable() override;

		virtual ComponentType GetComponentType() const override
		{
			return ComponentType::Renderable;
		}

		virtual void GetRenderablePackage(RenderablePackCollector& collector, Frustum const& frustum, f32M44 const& worldViewMatrix) = 0;

		virtual BoundingBox GetBoundingBox() = 0;

	};
}


