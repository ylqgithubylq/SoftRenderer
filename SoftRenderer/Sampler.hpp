#pragma once
#include "Common.hpp"
#include "Texture2D.hpp"
namespace X
{

	class Sampler
		: Noncopyable
	{
	public:
		enum class AddressMode
		{
			ClampToEdge,
			Repeat,
		};
		class Addresser
			: Noncopyable
		{
		};
		class ClampToEdgeAddresser
			: public Addresser
		{
		public:
			f32V2 GetAddress(f32V2 const& samplePoint)
			{
				return f32V2(Clamp(samplePoint.X(), 0.f, 1.f - std::numeric_limits<f32>::epsilon()), Clamp(samplePoint.Y(), 0.f, 1.f - std::numeric_limits<f32>::epsilon()));
			}
		};
		class RepeatAddresser
			: public Addresser
		{
		public:
			f32V2 GetAddress(f32V2 const& samplePoint)
			{				
				return f32V2((samplePoint.X() - std::floor(samplePoint.X())) * (1.f - std::numeric_limits<f32>::epsilon()),
					(samplePoint.Y() - std::floor(samplePoint.Y())) * (1.f - std::numeric_limits<f32>::epsilon()));
			}

		};


	public:
		Sampler();
		virtual ~Sampler();
	};

	template <typename AddresserT>
	class PointSampler
		: public Sampler
	{
	public:
		PointSampler()
		{
		}
		virtual ~PointSampler() override
		{
		}

		// repeat
		template <typename ElementType>
		ElementType Sample(ConcreteTexture2D<ElementType> const& texture, f32V2 const& samplePoint) const
		{
			f32V2 sample = AddresserT().GetAddress(samplePoint);
			f32 x = sample.X() * texture.GetSize(0).X();
			f32 y = sample.Y() * texture.GetSize(0).Y();
			return texture.GetValue(0, Point<u32, 2>(static_cast<u32>(x), static_cast<u32>(y)));
		}
	};

	class LinearSampler
		: public Sampler
	{
	public:
		LinearSampler();
		virtual ~LinearSampler() override;

		// clamped to edge
		template <typename ElementType>
		ElementType Sample(ConcreteTexture2D<ElementType> const& texture, f32V2 const& samplePoint) const
		{
			// TODO not correct
			assert(false);
			f32 x = samplePoint.X() * texture.GetSize(0).X();
			f32 y = samplePoint.Y() * texture.GetSize(0).Y();
			Point<u32, 2> bl(std::lround(x), std::lround(y));
			Point<u32, 2> br(bl.X() + 1, bl.Y());
			Point<u32, 2> tl(bl.X(), bl.Y() + 1);
			Point<u32, 2> tr(bl.X() + 1, bl.Y() + 1);
			ElementType blv = texture.GetValue(0, bl);
			ElementType brv = texture.GetValue(0, br);
			ElementType tlv = texture.GetValue(0, tl);
			ElementType trv = texture.GetValue(0, tr);
			ElementType bv = Lerp(blv, brv, x - bl.X());
			ElementType tv = Lerp(tlv, trv, x - bl.X());
			ElementType v = Lerp(bv, tv, samplePoint.Y() - bl.Y());
			return v;
		}
	};
}


