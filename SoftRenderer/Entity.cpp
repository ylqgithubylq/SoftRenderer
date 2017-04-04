#include "Header.hpp"
#include "Entity.hpp"

namespace X
{
	Entity::Entity(std::wstring name)
		: name_(std::move(name))
	{
	}


	Entity::~Entity()
	{
	}
}

