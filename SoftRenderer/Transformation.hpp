#pragma once
#include "Component.hpp"
namespace X
{

	class Transformation
		: public Component
	{
	public:
		Transformation();
		virtual ~Transformation() override;

		virtual ComponentType GetComponentType() const override
		{
			return ComponentType::Transformation;
		}

		f32 GetScaling() const
		{
			return scaling_;
		}
		void SetScaling(f32 scaling)
		{
			scaling_ = scaling;
		}
		f32V3 GetPosition() const
		{
			return position_;
		}
		void SetPosition(f32V3 const& position)
		{
			position_ = position;
		}
		f32M44 GetOrientation() const
		{
			return orientation_;
		}
		void SetOrientation(f32M44 const& rotation)
		{
			orientation_ = rotation;
		}

		void Translate(f32V3 const& translate)
		{
			position_ = position_ + translate;
		}
		void Rotate(f32M44 const& rotate)
		{
			orientation_ = orientation_ * rotate;
		}
		void Scale(f32 scale)
		{
			scaling_ = scaling_ * scale;
		}

		f32V3 const& GetModelFrontDirection() const
		{
			return front_;
		}

		/*
		*	Default value is +z.
		*/
		void SetModelFrontDirection(f32V3 const& front)
		{
			front_ = front;
		}

		f32V3 const& GetModelUpDirection() const
		{
			return up_;
		}
		/*
		*	Default value is +y.
		*/
		void SetModelUpDirection(f32V3 const& up)
		{
			up_ = up;
		}

		f32M44 GetWorldMatrix() const;

	private:
		f32 scaling_;
		f32V3 position_;
		f32M44 orientation_;

		f32V3 front_;
		f32V3 up_;
	};
}


