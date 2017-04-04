#pragma once
#include "Common.hpp"

namespace X
{
	struct Vertex
	{
		f32V3 position;
		f32V3 normal;
		f32V2 textureCoordinate;
		Vertex()
		{
		}
		Vertex(f32V3 const& position, f32V3 const& normal, f32V2 textureCoordinate)
			: position(position), normal(normal), textureCoordinate(textureCoordinate)
		{
		}


		friend Vertex operator +(Vertex const& left, Vertex const& right)
		{
			return Vertex(left.position + right.position, left.normal + right.normal, left.textureCoordinate + right.textureCoordinate);
		}

		friend Vertex operator -(Vertex const& left, Vertex const& right)
		{
			return Vertex(left.position - right.position, left.normal - right.normal, left.textureCoordinate - right.textureCoordinate);
		}

		friend Vertex operator *(Vertex const& left, Vertex const& right)
		{
			return Vertex(left.position * right.position, left.normal * right.normal, left.textureCoordinate * right.textureCoordinate);
		}
		friend Vertex operator *(Vertex const& left, f32 right)
		{
			return Vertex(left.position * right, left.normal * right, left.textureCoordinate * right);
		}
		friend Vertex operator *(f32 left, Vertex const& right)
		{
			return Vertex(left * right.position, left * right.normal, left * right.textureCoordinate);
		}

		friend Vertex operator /(Vertex const& left, Vertex const& right)
		{
			return Vertex(left.position / right.position, left.normal / right.normal, left.textureCoordinate / right.textureCoordinate);
		}
		friend Vertex operator /(Vertex const& left, f32 right)
		{
			return Vertex(left.position / right, left.normal / right, left.textureCoordinate / right);
		}

	};

	template <>
	inline Vertex Lerp3<Vertex>(Vertex const& v0, Vertex const& v1, Vertex const& v2, f32 t0, f32 t1, f32 t2)
	{
		struct VNFA 
		{
			std::array<f32, sizeof(Vertex) / sizeof(f32)> floats;
		};
		VNFA const* pv0 = reinterpret_cast<VNFA const*>(&v0);
		VNFA const* pv1 = reinterpret_cast<VNFA const*>(&v1);
		VNFA const* pv2 = reinterpret_cast<VNFA const*>(&v2);

		Vertex v;
		VNFA* pvr = reinterpret_cast<VNFA*>(&v);

		for (u32 i = 0; i < pvr->floats.size(); ++i)
		{
			pvr->floats[i] = pv0->floats[i] * t0 + pv1->floats[i] * t1 + pv2->floats[i] * t2;
		}
		return v;
	}

}