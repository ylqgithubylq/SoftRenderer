#include "Header.hpp"
#include "Camera.hpp"
#include "Entity.hpp"
namespace X
{

	Camera::Camera()
	{
	}


	Camera::~Camera()
	{
	}

	f32M44 Camera::GetViewMatrix() const
	{
		Transformation* transformation = GetOwner()->GetComponent<Transformation>();
		f32M44 worldMatrix = transformation->GetWorldMatrix();
		f32V3 to = TransformDirection(transformation->GetModelFrontDirection(), worldMatrix);
		f32V3 up = TransformDirection(transformation->GetModelUpDirection(), worldMatrix);
		return LookToViewMatrix(transformation->GetPosition(), to, up);
	}


	PerspectiveCamera::PerspectiveCamera(f32 fieldOfView, f32 aspectRatio, f32 near, f32 far)
		: fieldOfView_(fieldOfView), aspectRatio_(aspectRatio), near_(near), far_(far),
		viewSpaceFrustum_(fieldOfView, aspectRatio, near, far)
	{
		projectionMatrix_ = FrustumProjectionMatrix(fieldOfView, aspectRatio, near, far);
	}

	PerspectiveCamera::~PerspectiveCamera()
	{
	}

}

