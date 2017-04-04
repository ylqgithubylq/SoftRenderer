#pragma once
#include "Common.hpp"
namespace X
{
	template <typename ElementType, u32 Dimension>
	struct TextureMipmapStorage
	{
		static u32 OffsetInLevel(Size<u32, Dimension> const& levelSize, Point<u32, Dimension> const& point)
		{

			u32 offsetInLevel = point[Dimension - 1];
			for (s32 i = Dimension - 2; i >= 0; --i)
			{
				offsetInLevel = offsetInLevel * levelSize[i] + point[i];
			}
			return offsetInLevel;
		}

		TextureMipmapStorage(Size<u32, Dimension> size)
		{
			u32 mipmapCount = 1;
			sizes_.push_back(size); // level 0
			u32 offset = 0;
			offsets_.push_back(offset);
			while (*std::max_element(size.data.begin(), size.data.end()) > 1)
			{
				offset += std::accumulate(size.data.begin(), size.data.end(), 1, [] (u32 left, u32 right)
				{
					return left * right;
				});
				offsets_.push_back(offset);
				for (u32 i = 0; i < Dimension; ++i)
				{
					size[i] = std::max(size[i] / 2, 1u);
				}
				sizes_.push_back(size);

				++mipmapCount;
			}
			offsets_.push_back(offset + 1); // one after data
			mipmapCount_ = mipmapCount;

			data_.resize(offsets_.back());
		}

		Size<u32, Dimension> const& GetSize(u32 mipmapLevel) const
		{
			assert(mipmapLevel < mipmapCount_);
			return sizes_[mipmapLevel];
		}

		u32 GetMipmapCount() const
		{
			return mipmapCount_;
		}

		void Clear(u32 mipmapLevel, ElementType const& value)
		{
			std::fill(data_.begin() + offsets_[mipmapLevel], data_.begin() + offsets_[mipmapLevel + 1], value);
		}

		void SetValues(u32 mipmapLevel, ElementType const* values, u32 valueLength)
		{
			assert(mipmapLevel < mipmapCount_);
			assert(offsets_[mipmapLevel + 1] - offsets_[mipmapLevel] == valueLength);
			u32 offset = offsets_[mipmapLevel];
			std::copy(values, values + valueLength, data_.begin() + offset);
		}

		void SetValue(u32 mipmapLevel, Point<u32, Dimension> const& point, ElementType const& value)
		{
			assert(mipmapLevel < mipmapCount_);
			assert(point.X() < sizes_[mipmapLevel].X() && point.Y() < sizes_[mipmapLevel].Y());
			u32 offsetInLevel = OffsetInLevel(sizes_[mipmapLevel], point);
			data_[offsets_[mipmapLevel] + offsetInLevel] = value;
		}

		ElementType const& GetValue(u32 mipmapLevel, Point<u32, Dimension> const& point) const
		{
			assert(mipmapLevel < mipmapCount_);
			assert(point.X() < sizes_[mipmapLevel].X() && point.Y() < sizes_[mipmapLevel].Y());
			u32 offsetInLevel = OffsetInLevel(sizes_[mipmapLevel], point);
			return data_[offsets_[mipmapLevel] + offsetInLevel];
		}

		ElementType* GetValues(u32 mipmapLevel)
		{
			return &data_[offsets_[mipmapLevel]];
		}


		std::vector<ElementType> data_;
		std::vector<Size<u32, Dimension>> sizes_;
		std::vector<u32> offsets_;
		u32 mipmapCount_;
	};



}