#pragma once
#include "Component.hpp"

namespace X
{
	class Camera
		: public Component
	{
	protected:
		Camera();
	public:
		virtual ~Camera() override;

		virtual ComponentType GetComponentType() const override
		{
			return ComponentType::Camera;
		}

		f32M44 GetViewMatrix() const;

		virtual f32M44 GetProjectionMatrix() const = 0;

		virtual Frustum const& GetFrustum() const = 0;

	};

	/*
	*      +z is the front direction. +y is the up direction.
	*/
	class  PerspectiveCamera
		: public Camera
	{
	public:
		PerspectiveCamera(f32 fieldOfView, f32 aspectRatio, f32 near, f32 far);
		virtual ~PerspectiveCamera() override;


		virtual f32M44 GetProjectionMatrix() const override
		{
			return projectionMatrix_;
		}
		virtual Frustum const& GetFrustum() const override
		{
			return viewSpaceFrustum_;
		}

		f32 GetFieldOfView() const
		{
			return fieldOfView_;
		}
		f32 GetAspectRatio() const
		{
			return aspectRatio_;
		}
		f32 GetNear() const
		{
			return near_;
		}
		f32 GetFar() const
		{
			return far_;
		}
	private:
		f32 fieldOfView_;
		f32 aspectRatio_;
		f32 near_;
		f32 far_;

		f32M44 projectionMatrix_;

		Frustum viewSpaceFrustum_;

	};
}


