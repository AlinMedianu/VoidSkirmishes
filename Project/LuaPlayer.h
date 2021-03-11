#ifndef LUAPLAYER
#define LUAPLAYER

#include <LuaBridge/LuaBridge.h>
#include <SFML/System/Vector2.hpp>

namespace Lua
{
	struct Player
	{
		sf::Vector2f position, destination, facingDirection, aimingDirection;
		float movementSpeed, turningSpeed;
		void SyncFrom(const luabridge::LuaRef& player);
		void Sync(luabridge::LuaRef& player) const;
	};
}

#endif // !LUAPLAYER