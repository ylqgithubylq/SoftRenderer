#pragma once
#include "Common.hpp"
namespace X
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		void AddEntity(std::shared_ptr<Entity> entity)
		{
			entities_.push_back(std::move(entity));
		}

		Entity* GetActiveCameraEntity()
		{
			return activeCamera_.get();
		}
		void SetActiveCamera(std::shared_ptr<Entity> activeCamera)
		{
			activeCamera_ = std::move(activeCamera);
		}

		std::vector<std::shared_ptr<Entity>> const& GetAllEntities() const
		{
			return entities_;
		}

	private:
		std::vector<std::shared_ptr<Entity>> entities_;
		std::shared_ptr<Entity> activeCamera_;
	};
}

