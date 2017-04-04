#include "Header.hpp"
#include "Mesh.hpp"
#include "GeometryLayout.hpp"

namespace X
{
	Mesh::Mesh()
		: boundingBox_(f32V3(0, 0, 0), f32V3(0, 0, 0))
	{
	}


	Mesh::~Mesh()
	{
	}

	Mesh::SubMesh& Mesh::CreateSubMesh(std::shared_ptr<GeometryLayout> layout, std::shared_ptr<Material> material)
	{
		subMeshes_.push_back(std::make_unique<SubMesh>(*this, std::move(layout), std::move(material)));
		return *subMeshes_.back();
	}

	void Mesh::GetRenderablePackage(RenderablePackCollector& collector, Frustum const& frustum, f32M44 const& worldViewMatrix)
	{

		for (auto& subMesh : subMeshes_)
		{
			RotatedBoundingBox boxInView = Transform(subMesh->GetBoundingBox(), worldViewMatrix);
			if (IntersectRough(boxInView, frustum))
			{
				subMesh->GetRenderablePackage(collector);
			}
		}
	}

	void Mesh::CalculateBoundingBox()
	{
		static const f32 FloatMax = std::numeric_limits<f32>::max();
		f32V3 meshMin(FloatMax, FloatMax, FloatMax);
		f32V3 meshMax(-FloatMax, -FloatMax, -FloatMax);
		
		for (auto& subMesh : subMeshes_)
		{
			std::shared_ptr<VertexBuffer> const& vertexBuffer = subMesh->GetGeometryLayout()->GetVertexBuffer();
			std::vector<Vertex> const& vertices = vertexBuffer->GetData();
			f32V3 min(FloatMax, FloatMax, FloatMax);
			f32V3 max(-FloatMax, -FloatMax, -FloatMax);
			for (Vertex const& v : vertices)
			{
				min = f32V3(std::min(min.X(), v.position.X()), std::min(min.Y(), v.position.Y()), std::min(min.Z(), v.position.Z()));
				max = f32V3(std::max(max.X(), v.position.X()), std::max(max.Y(), v.position.Y()), std::max(max.Z(), v.position.Z()));
			}
			f32V3 center = (min + max) / 2;
			f32V3 halfExtend = center - min;
			subMesh->SetBoundingBox(BoundingBox(center, halfExtend));

			meshMin = f32V3(std::min(min.X(), meshMin.X()), std::min(min.Y(), meshMin.Y()), std::min(min.Z(), meshMin.Z()));
			meshMax = f32V3(std::max(max.X(), meshMax.X()), std::max(max.Y(), meshMax.Y()), std::max(max.Z(), meshMax.Z()));
		}

		f32V3 meshCenter = (meshMin + meshMax) / 2;
		f32V3 meshHalfExtend = meshCenter - meshMin;
		SetBoundingBox(BoundingBox(meshCenter, meshHalfExtend));
	}


	Mesh::SubMesh::SubMesh(Mesh& mesh, std::shared_ptr<GeometryLayout> layout, std::shared_ptr<Material> material)
		: mesh_(mesh), layout_(std::move(layout)), material_(std::move(material)), boundingBox_(f32V3(0, 0, 0), f32V3(0, 0, 0))
	{
	}

	void Mesh::SubMesh::GetRenderablePackage(RenderablePackCollector& collector)
	{
		collector.AddPackage(RenderablePackage(mesh_, layout_, material_));
	}

}

