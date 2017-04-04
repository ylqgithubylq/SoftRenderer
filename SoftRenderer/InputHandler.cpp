#include "Header.hpp"
#include "InputHandler.hpp"

namespace X
{
	InputHandler::InputHandler(InputManager& manager, ActionMap actionMap)
		: manager_(manager), actionMap_(std::move(actionMap))
	{
	}


	InputHandler::~InputHandler()
	{
	}

	void InputHandler::OnAction(InputManager::InputEvent const& inputEvent)
	{
		std::pair<bool, std::function<void()>> result = GenerateAction(inputEvent);
		if (result.first)
		{
			manager_.EnqueueAction(std::move(result.second));
		}
	}

	void InputHandler::OnBeforeLogicFrame(f64 currentTime)
	{
		std::pair<bool, std::function<void()>> result = DoOnBeforeLogicFrame(currentTime);
		if (result.first)
		{
			manager_.EnqueueAction(std::move(result.second));
		}
	}
}

