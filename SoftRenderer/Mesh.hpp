#pragma once
#include "Common.hpp"
#include "Renderable.hpp"


namespace X
{
	class Mesh
		: public Renderable
	{
	public:

		class SubMesh
			: X::Noncopyable
		{
		public:
			SubMesh(Mesh& mesh, std::shared_ptr<GeometryLayout> layout, std::shared_ptr<Material> material);

			void GetRenderablePackage(RenderablePackCollector& collector);

			std::shared_ptr<GeometryLayout> const& GetGeometryLayout() const
			{
				return layout_;
			}
			std::shared_ptr<Material> const& GetMaterial() const
			{
				return material_;
			}

			void SetBoundingBox(BoundingBox const& box)
			{
				boundingBox_ = box;
			}
			BoundingBox const& GetBoundingBox() const
			{
				return boundingBox_;
			}

		private:
			Mesh& mesh_;

			std::shared_ptr<GeometryLayout> layout_;
			std::shared_ptr<Material> material_;

			BoundingBox boundingBox_;
		};

	public:
		Mesh();
		virtual ~Mesh() override;

		SubMesh& CreateSubMesh(std::shared_ptr<GeometryLayout> layout, std::shared_ptr<Material> material);

		virtual void GetRenderablePackage(RenderablePackCollector& collector, Frustum const& frustum, f32M44 const& worldViewMatrix) override;

		u32 GetSubMeshCount() const
		{
			return subMeshes_.size();
		}
		SubMesh& GetSubMesh(u32 index) const
		{
			return *subMeshes_[index];
		}

		void SetBoundingBox(BoundingBox const& box)
		{
			boundingBox_ = box;
		}

		virtual BoundingBox GetBoundingBox() override
		{
			assert(boundingBox_.GetHalfExtend().LengthSquared() != 0); // not initialized
			return boundingBox_;
		}

		void CalculateBoundingBox();

	private:
		std::vector<std::unique_ptr<SubMesh>> subMeshes_;
		BoundingBox boundingBox_;
	};
}


