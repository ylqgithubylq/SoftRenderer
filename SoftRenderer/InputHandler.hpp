#pragma once
#include "Common.hpp"
#include "InputManager.hpp"
namespace X
{
	class InputHandler
		: Noncopyable
	{
	public:
		class ActionMap
			: Noncopyable
		{
		public:
			ActionMap()
			{
			}
			ActionMap(ActionMap&& right)
				: actionMap_(std::move(right.actionMap_))
			{
			}
			ActionMap& operator =(ActionMap&& right)
			{
				actionMap_ = std::move(right.actionMap_);
				return *this;
			}
			void Set(InputManager::InputSemantic inputSemantic, s32 mappedSemantic)
			{
				actionMap_[inputSemantic] = mappedSemantic;
			}
			bool Contains(InputManager::InputSemantic inputSemantic) const
			{
				return actionMap_.find(inputSemantic) != actionMap_.end();
			}
			s32 Get(InputManager::InputSemantic inputSemantic) const
			{
				assert(Contains(inputSemantic));
				return actionMap_.at(inputSemantic);
			}
			void Remove(InputManager::InputSemantic inputSemantic)
			{
				assert(Contains(inputSemantic));
				actionMap_.erase(inputSemantic);
			}

			std::unordered_map<InputManager::InputSemantic, s32> const& GetAllActions() const
			{
				return actionMap_;
			}

		private:
			std::unordered_map<InputManager::InputSemantic, s32> actionMap_;
		};



	public:
		virtual ~InputHandler();

		ActionMap const& GetActionMap() const
		{
			return actionMap_;
		}

		void OnAction(InputManager::InputEvent const& inputEvent);

		void OnBeforeLogicFrame(f64 currentTime);


	protected:

		InputHandler(InputManager& manager, ActionMap actionMap);

		/*
		*	Override this if something need to do before a logic frame begin.
		*	@return: first component is true indicates action generated. second component will be enqueued into InputManager if first is true.
		*/
		virtual std::pair<bool, std::function<void()>> DoOnBeforeLogicFrame(f64 currentTime)
		{
			return std::make_pair(false, std::function<void()>());
		}
		/*
		*	@return: first component is true indicates action generated. second component will be enqueued into InputManager if first is true.
		*/
		virtual std::pair<bool, std::function<void()>> GenerateAction(InputManager::InputEvent const& inputEvent) = 0;

		ActionMap& GetMutableActionMap()
		{
			return actionMap_;
		}

		InputManager& GetInputManager() const
		{
			return manager_;
		}

	private:
		InputManager& manager_;
		ActionMap actionMap_;
	};
}


