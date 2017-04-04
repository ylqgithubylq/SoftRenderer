#pragma once
#include "Common.hpp"
namespace X
{
	enum class ComponentType
	{
		Renderable,
		Camera,
		Light,

		Transformation,
		Count = Transformation, // Transformation is contained in Entity
	};

	template <typename T>
	struct ComponentTypeFromType
	{
		static ComponentType const Value = ComponentType::Count;
	};
	template <>
	struct ComponentTypeFromType<Renderable>
	{
		static ComponentType const Value = ComponentType::Renderable;
	};
	template <>
	struct ComponentTypeFromType<Camera>
	{
		static ComponentType const Value = ComponentType::Camera;
	};
	template <>
	struct ComponentTypeFromType<Light>
	{
		static ComponentType const Value = ComponentType::Light;
	};
	template <>
	struct ComponentTypeFromType<Transformation>
	{
		static ComponentType const Value = ComponentType::Transformation;
	};

	class Component
		: Noncopyable
	{
	protected:
		Component()
			: owner_(nullptr), active_(true)
		{
		}
	public:
		virtual ~Component() = 0
		{
		}

		Entity* GetOwner() const
		{
			return owner_;
		}
		void SetOwner(Entity* owner)
		{
			owner_ = owner;
		}

		virtual ComponentType GetComponentType() const = 0;

		bool IsActive() const
		{
			return active_;
		}
		void SetActive(bool active)
		{
			active_ = active;
		}
	private:
		Entity* owner_;
		bool active_;
	};
}


