#include "Header.hpp"
#include "InputManager.hpp"
#include "InputHandler.hpp"
#include "Context.hpp"

namespace X
{
	InputManager::InputManager(Context& context)
		: context_(context), previousPointerPosition_(0, 0), pointerPosition_(0, 0)
	{
		semanticStates_.fill(false);
	}


	InputManager::~InputManager()
	{
	}


	bool InputManager::AddInputHandler(std::shared_ptr<InputHandler> inputHandler)
	{
		assert(inputHandler != nullptr);
		auto result = inputHandlers_.insert(std::move(inputHandler));
		return result.second;
	}

	bool InputManager::RemoveInputHandler(std::shared_ptr<InputHandler> const& inputHandler)
	{
		assert(inputHandler != nullptr);
		auto found = inputHandlers_.find(inputHandler);
		if (found == inputHandlers_.end())
		{
			return false;
		}
		inputHandlers_.erase(found);

		return true;
	}

	void InputManager::ExecuteAllQueuedActions(f64 currentTime)
	{
		for (auto i = inputHandlers_.begin(); i != inputHandlers_.end(); ++i)
		{
			(*i)->OnBeforeLogicFrame(currentTime);
		}

		while (!actionQueue_.empty())
		{
			actionQueue_.front()();
			actionQueue_.pop();
		}
	}


	void InputManager::InjectKeyDown(InputSemantic semantic)
	{
		semanticStates_[static_cast<u32>(semantic)] = true;
		DispatchInputEvent(semantic, static_cast<u32>(true));
	}

	void InputManager::InjectKeyUp(InputSemantic semantic)
	{
		semanticStates_[static_cast<u32>(semantic)] = false;
		DispatchInputEvent(semantic, static_cast<u32>(false));
	}

	void InputManager::InjectMouseDown(InputSemantic semantic, s32 x, s32 y)
	{
		semanticStates_[static_cast<u32>(semantic)] = true;
		previousPointerPosition_ = pointerPosition_;
		pointerPosition_ = Point<s32, 2>(x, y);
		DispatchInputEvent(semantic, static_cast<u32>(true));
	}

	void InputManager::InjectMouseUp(InputSemantic semantic, s32 x, s32 y)
	{
		semanticStates_[static_cast<u32>(semantic)] = false;
		previousPointerPosition_ = pointerPosition_;
		pointerPosition_ = Point<s32, 2>(x, y);
		DispatchInputEvent(semantic, static_cast<u32>(false));
	}

	void InputManager::InjectMouseWheel(InputSemantic semantic, s32 x, s32 y, s32 delta)
	{
		previousPointerPosition_ = pointerPosition_;
		pointerPosition_ = Point<s32, 2>(x, y);
		DispatchInputEvent(semantic, delta);
	}

	void InputManager::InjectMouseMove(InputSemantic semantic, s32 x, s32 y)
	{
		previousPointerPosition_ = pointerPosition_;
		pointerPosition_ = Point<s32, 2>(x, y);
		DispatchInputEvent(semantic, 0);
	}

	void InputManager::DispatchInputEvent(InputSemantic semantic, s32 data)
	{
		f64 currentTime = context_.GetElapsedTime();
		for (auto i = inputHandlers_.begin(); i != inputHandlers_.end(); ++i)
		{
			InputHandler::ActionMap const& actionMap = (*i)->GetActionMap();
			std::unordered_map<InputSemantic, s32> const& mapData = actionMap.GetAllActions();
			auto found = mapData.find(semantic);
			if (found != mapData.end())
			{
				InputEvent event(found->second, data, pointerPosition_, currentTime);
				(*i)->OnAction(event);
			}
		}
	}

}

