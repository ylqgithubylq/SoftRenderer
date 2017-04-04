#pragma once
#include "Common.hpp"
#include "TextureStorage.hpp"
namespace X
{
	class Texture2D
		: Noncopyable
	{
	public:
		Texture2D();
		virtual ~Texture2D();

		virtual Size<u32, 2> const& GetSize(u32 mipmapLevel) const = 0;
		virtual u32 GetMipmapCount() const = 0;
	};
	
// 	template<typename ElementType>
// 	class ConcreteTexture2D
// 		: public Texture2D
// 	{
// 	public:
// 		ConcreteTexture2D(Size<u32, 2> const& size)
// 			: storage_(size)
// 		{
// 		}
// 
// 		virtual ~ConcreteTexture2D() override
// 		{
// 		}
// 
// 		virtual Size<u32, 2> const& GetSize(u32 mipmapLevel) const override
// 		{
// 			return storage_.GetSize(mipmapLevel);
// 		}
// 		virtual u32 GetMipmapCount() const override
// 		{
// 			return storage_.GetMipmapCount();
// 		}
// 
// 		void Clear(u32 mipmapLevel, ElementType const& value)
// 		{
// 			storage_.Clear(mipmapLevel, value);
// 		}
// 
// 		void SetValues(u32 mipmapLevel, ElementType const* values, u32 valueLength)
// 		{
// 			storage_.SetValues(mipmapLevel, values, valueLength);
// 		}
// 
// 		void SetValue(u32 mipmapLevel, Point<u32, 2> const& point, ElementType const& value)
// 		{
// 			storage_.SetValue(0, point, value);
// 		}
// 		ElementType const& GetValue(u32 mipmapLevel, Point<u32, 2> const& point) const
// 		{
// 			return storage_.GetValue(mipmapLevel, point);
// 		}
// 		ElementType* GetValues(u32 mipmapLevel)
// 		{
// 			return storage_.GetValues(mipmapLevel);
// 		}
// 
// 	private:
// 		TextureMipmapStorage<ElementType, 2> storage_;
// 	};

	template<typename ElementType>
	class ConcreteTexture2D
		: public Texture2D
	{
		static u32 OffsetInLevel(Size<u32, 2> const& size, Point<u32, 2> const& point)
		{

			return size.X() * point.Y() + point.X();
		}

	public:
		ConcreteTexture2D(Size<u32, 2> const& size)
			: size_(size)
		{
			data_.resize(OffsetInLevel(size_, Point<u32, 2>(size_.X() - 1, size_.Y() - 1)) + 1);
		}

		virtual ~ConcreteTexture2D() override
		{
		}

		virtual Size<u32, 2> const& GetSize(u32 mipmapLevel) const override
		{
			return size_;
		}
		virtual u32 GetMipmapCount() const override
		{
			return 1;
		}

		void Clear(u32 mipmapLevel, ElementType const& value)
		{
			std::fill(data_.begin(), data_.end(), value);
		}

		void SetValues(u32 mipmapLevel, ElementType const* values, u32 valueLength)
		{
			assert(OffsetInLevel(size_, Point<u32, 2>(size_.X() - 1, size_.Y() - 1)) + 1 == valueLength);
			std::copy(values, values + valueLength, data_.begin());
		}

		void SetValue(u32 mipmapLevel, Point<u32, 2> const& point, ElementType const& value)
		{
			assert(point.X() < size_.X() && point.Y() < size_.Y());
			u32 offsetInLevel = OffsetInLevel(size_, point);
			data_[offsetInLevel] = value;

		}
		ElementType const& GetValue(u32 mipmapLevel, Point<u32, 2> const& point) const
		{
			u32 offsetInLevel = OffsetInLevel(size_, point);
			return data_[offsetInLevel];
		}
		ElementType* GetValues(u32 mipmapLevel)
		{
			return &data_[0];
		}

	private:
		std::vector<ElementType> data_;
		Size<u32, 2> size_;
	};
}


