#pragma once
#include "InputHandler.hpp"

namespace X
{
	class FirstPersonCameraController
		: public InputHandler
	{
	public:
		enum class MoveSemantic
		{
			MoveForward,
			MoveBack,
			MoveLeft,
			MoveRight,
			MoveUp,
			MoveDown,
			Turn,
			TriggerTurn,
			SpeedUp,

			RoamSemanticCount,
		};

	public:
		FirstPersonCameraController(InputManager& manager, f32 moveScaler = 1.0f, f32 rotateScaler = 1.0f, f32 speedScaler = 10.0);
		virtual ~FirstPersonCameraController() override;

		void AttachToCamera(std::shared_ptr<Entity> const& cameraObject);

	protected:

		virtual std::pair<bool, std::function<void()>> DoOnBeforeLogicFrame(f64 currentTime) override;

		virtual std::pair<bool, std::function<void()>> GenerateAction(InputManager::InputEvent const& inputEvent) override;

	private:
		std::function<void()> GenerateFrameAction(f32 delta);
		std::function<void()> GenerateMoveAction(f32 forward, f32 left, f32 up);
		std::function<void()> GenerateRotateAction(f32V2 const& deltaTurn);


	private:
		std::shared_ptr<Entity> cameraObject_;

		f32 moveScaler_;
		f32 rotateScaler_;
		f32 speedScaler_;

		f64 previousFrameTime_;
		Point<s32, 2> previousPointerPosition_;

		std::array<s32, static_cast<u32>(MoveSemantic::RoamSemanticCount)> semanticStates_;
		s32 forward_;
		s32 right_;
		s32 up_;

		f32 azimuthAngle_; // camera to object
		f32 elevationAngle_; // camera to object

		bool turnTriggered_;
		bool spedUp_;
	};
}


