#include "Header.hpp"
#include "CameraController.hpp"
#include "Entity.hpp"
#include "Camera.hpp"

namespace X
{
	namespace
	{
		InputHandler::ActionMap GenerateActionMap()
		{
			InputHandler::ActionMap map;
			map.Set(InputManager::InputSemantic::K_W, static_cast<u32>(FirstPersonCameraController::MoveSemantic::MoveForward));
			map.Set(InputManager::InputSemantic::K_S, static_cast<u32>(FirstPersonCameraController::MoveSemantic::MoveBack));
			map.Set(InputManager::InputSemantic::K_A, static_cast<u32>(FirstPersonCameraController::MoveSemantic::MoveLeft));
			map.Set(InputManager::InputSemantic::K_D, static_cast<u32>(FirstPersonCameraController::MoveSemantic::MoveRight));
			map.Set(InputManager::InputSemantic::K_V, static_cast<u32>(FirstPersonCameraController::MoveSemantic::MoveUp));
			map.Set(InputManager::InputSemantic::K_C, static_cast<u32>(FirstPersonCameraController::MoveSemantic::MoveDown));
			map.Set(InputManager::InputSemantic::M_Move, static_cast<u32>(FirstPersonCameraController::MoveSemantic::Turn));
			map.Set(InputManager::InputSemantic::K_LeftShift, static_cast<u32>(FirstPersonCameraController::MoveSemantic::SpeedUp));
			map.Set(InputManager::InputSemantic::K_RightShift, static_cast<u32>(FirstPersonCameraController::MoveSemantic::SpeedUp));
			map.Set(InputManager::InputSemantic::M_Button0, static_cast<u32>(FirstPersonCameraController::MoveSemantic::TriggerTurn));
			return map;
		}

		f32 const DefaultMoveSpeed = 5.0f;
	}

	FirstPersonCameraController::FirstPersonCameraController(InputManager& manager, f32 moveScaler, f32 rotateScaler, f32 speedScaler)
		: InputHandler(manager, GenerateActionMap()),
		moveScaler_(moveScaler), rotateScaler_(rotateScaler), speedScaler_(speedScaler), previousFrameTime_(0),
		previousPointerPosition_(0, 0), forward_(0), right_(0), up_(0), azimuthAngle_(0), elevationAngle_(0), turnTriggered_(false), spedUp_(false)
	{
		semanticStates_.fill(0);
	}


	FirstPersonCameraController::~FirstPersonCameraController()
	{
	}

	void FirstPersonCameraController::AttachToCamera(std::shared_ptr<Entity> const& cameraObject)
	{
		assert(cameraObject->GetComponent<Camera>() != nullptr);
		cameraObject_ = cameraObject;
		Transformation* cameraTransformation = cameraObject->GetComponent<Transformation>();

		f32V3 front = cameraTransformation->GetModelFrontDirection();
		f32V3 left = Cross(front, cameraTransformation->GetModelUpDirection());
		f32V3 up = Cross(left, front);

		f32V3 actualHeading = TransformDirection(cameraTransformation->GetModelFrontDirection(), cameraTransformation->GetOrientation());
		f32 angleToUp = Angle(actualHeading, up);
		elevationAngle_ = PI / 2 - angleToUp;

		// get the projection vector of the displacement in front & right plane
		f32V3 axisToRotateDown = Cross(actualHeading, up);
		// rotate back to the plane
		f32V3 directionInFrontRightPlane = TransformDirection(actualHeading, RotationMatrix(-elevationAngle_, axisToRotateDown));

		bool positive = Dot(Cross(directionInFrontRightPlane, front), up) > 0;
		// the angle from directionInFrontRightPlane to front referenced by axis up, so negate it.
		azimuthAngle_ = -(Angle(directionInFrontRightPlane, front) * (positive ? 1 : -1));

		f32M44 elevation = RotationMatrix(elevationAngle_, left);
		f32M44 azimuth = RotationMatrix(azimuthAngle_, up);
		f32M44 totalRotation = elevation * azimuth;

		cameraTransformation->SetOrientation(totalRotation);
	}

	std::pair<bool, std::function<void()>> FirstPersonCameraController::DoOnBeforeLogicFrame(f64 currentTime)
	{
		f32 delta = static_cast<f32>(currentTime - previousFrameTime_);
		previousFrameTime_ = currentTime;

		return std::make_pair(true, GenerateFrameAction(delta));
	}

	std::pair<bool, std::function<void()>> FirstPersonCameraController::GenerateAction(InputManager::InputEvent const& inputEvent)
	{
		switch (static_cast<MoveSemantic>(inputEvent.mappedSemantic))
		{
		case MoveSemantic::MoveForward:
		case MoveSemantic::MoveBack:
		case MoveSemantic::MoveLeft:
		case MoveSemantic::MoveRight:
		case MoveSemantic::MoveUp:
		case MoveSemantic::MoveDown:
		{
			semanticStates_[static_cast<u32>(inputEvent.mappedSemantic)] = inputEvent.data;
			forward_ = semanticStates_[static_cast<u32>(MoveSemantic::MoveForward)] - semanticStates_[static_cast<u32>(MoveSemantic::MoveBack)];
			right_ = semanticStates_[static_cast<u32>(MoveSemantic::MoveRight)] - semanticStates_[static_cast<u32>(MoveSemantic::MoveLeft)];
			up_ = semanticStates_[static_cast<u32>(MoveSemantic::MoveUp)] - semanticStates_[static_cast<u32>(MoveSemantic::MoveDown)];
		}
			break;
		case MoveSemantic::Turn:
		{
			if (turnTriggered_)
			{
				Point<s32, 2> delta = Point<s32, 2>(inputEvent.pointerPosition.X() - previousPointerPosition_.X(), inputEvent.pointerPosition.Y() - previousPointerPosition_.Y());
				// negative x because screen coordinate is 180 rotated by Y axis of camera coordinate
				f32V2 deltaTurn(-delta.X() * rotateScaler_, delta.Y() * rotateScaler_);

				GetInputManager().EnqueueAction(GenerateRotateAction(deltaTurn));
			}
			previousPointerPosition_ = inputEvent.pointerPosition;
		}
			break;
		case MoveSemantic::TriggerTurn:
		{
			turnTriggered_ = !!inputEvent.data;
		}
			break;
		case MoveSemantic::SpeedUp:
		{
			spedUp_ = !!inputEvent.data;
		}
			break;
		default:
		{
			assert(false);
		}
		}
		return std::make_pair(false, std::function<void()>());
	}


	std::function<void()> FirstPersonCameraController::GenerateFrameAction(f32 delta)
	{
		f32 spedUpDelta = DefaultMoveSpeed * moveScaler_ * delta * ((spedUp_ ? 1 : 0) * speedScaler_ + 1);
		return GenerateMoveAction(spedUpDelta * forward_, spedUpDelta * right_, spedUpDelta * up_);
	}

	std::function<void()> FirstPersonCameraController::GenerateMoveAction(f32 forward, f32 right, f32 up)
	{
		std::shared_ptr<Entity> const& cameraObject = cameraObject_;

		std::function<void()> action = [cameraObject, forward, right, up] ()
		{
			Transformation* cameraTransformation = cameraObject->GetComponent<Transformation>();
			f32M44 orientation = cameraTransformation->GetOrientation();
			f32V3 forwardTransformed = TransformDirection(f32V3(0, 0, 1), orientation);
			f32V3 rightTransformed = TransformDirection(f32V3(1, 0, 0), orientation);
			f32V3 upTransformed = TransformDirection(f32V3(0, 1, 0), orientation);
			f32V3 finalVector = forwardTransformed * forward + rightTransformed * right + upTransformed * up;
			cameraTransformation->Translate(finalVector);
		};
		return action;
	}


	std::function<void()> FirstPersonCameraController::GenerateRotateAction(f32V2 const& deltaTurn)
	{
		std::function<void()> action = [this, deltaTurn] ()
		{
			f32 deltaX = RadianFromDegree(deltaTurn.X()) * rotateScaler_;
			f32 deltaY = RadianFromDegree(deltaTurn.Y()) * rotateScaler_;
			azimuthAngle_ += deltaX;
			elevationAngle_ += deltaY;
			if (azimuthAngle_ > PI)
			{
				azimuthAngle_ -= 2 * PI;
			}
			else if (azimuthAngle_ < -PI)
			{
				azimuthAngle_ += 2 * PI;
			}

			elevationAngle_ = Clamp(elevationAngle_, -PI / 2 + RadianFromDegree(5), PI / 2 - RadianFromDegree(5));

			Transformation* cameraTransformation = cameraObject_->GetComponent<Transformation>();
			f32V3 up = cameraTransformation->GetModelUpDirection();
			f32V3 left = Cross(cameraTransformation->GetModelFrontDirection(), up);
			up = Cross(left, cameraTransformation->GetModelFrontDirection());

			f32M44 elevation = RotationMatrix(-elevationAngle_, left);
			f32M44 azimuth = RotationMatrix(azimuthAngle_, up);
			f32M44 totalRotation = elevation * azimuth;

			cameraTransformation->SetOrientation(totalRotation);
		};
		return action;
	}

}
