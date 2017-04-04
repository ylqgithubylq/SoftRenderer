#pragma once
#include "Common.hpp"
#include "Transformation.hpp"
#include "Component.hpp"

namespace X
{
	class Entity
		: Noncopyable
	{
	public:
		Entity(std::wstring name);
		~Entity();

		std::wstring const& GetName() const
		{
			return name_;
		}

		template <typename T>
		T* GetComponent() const
		{
			return CheckedCast<T*>(components_[static_cast<u32>(ComponentTypeFromType<T>::Value)].get());
		}
		template <>
		Transformation* GetComponent<Transformation>() const
		{
			return const_cast<Transformation*>(&transformation_);
		}

		template<typename T>
		std::unique_ptr<T> SetComponent(std::unique_ptr<T> component)
		{	
			std::unique_ptr<Component> newOne(component.release());
			if (newOne != nullptr)
			{
				newOne->SetOwner(this);
			}
			std::unique_ptr<Component>& oldOne = components_[static_cast<u32>(newOne->GetComponentType())];
			std::swap(newOne, oldOne);
			if (newOne != nullptr)
			{
				newOne->SetOwner(nullptr); // set the owner of old component to nullptr
			}
			std::unique_ptr<T> toReturn(CheckedCast<T*>(newOne.release()));
			return toReturn;
		}
		template<>
		std::unique_ptr<Transformation> SetComponent(std::unique_ptr<Transformation> component)
		{
			assert(false);
			return component;
		}

	private:
		std::wstring name_;
		Transformation transformation_;
		std::array<std::unique_ptr<Component>, static_cast<u32>(ComponentType::Count)> components_;
	};
}

