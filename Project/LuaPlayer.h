#ifndef LUAPLAYER
#define LUAPLAYER

#include <LuaBridge/LuaBridge.h>
#include <SFML/System/Vector2.hpp>
#include <string>

namespace Lua
{
	struct Player
	{
		enum class Property : unsigned
		{
			Position = 1 << 0,
			Destination = 1 << 1,
			FacingDirection = 1 << 2,
			AimingDirection = 1 << 3,
			MovementSpeed = 1 << 4,
			TurningSpeed = 1 << 5
		};
		sf::Vector2f position, destination, facingDirection, aimingDirection;
		float movementSpeed, turningSpeed;
		void SyncFrom(const luabridge::LuaRef& player);
		void Sync(luabridge::LuaRef& player) const;
		[[nodiscard]] bool HasDifferentProperties(const luabridge::LuaRef& than, unsigned propertiesToNotCheck);
	};

	inline void NameFromPlayerProperties(std::string& name, unsigned properties)
	{
		using namespace std::string_literals;
		name = "";
		if (properties & static_cast<unsigned>(Player::Property::Position))
			name += (name.length() > 0 ? ", " : "") + "position"s;
		if (properties & static_cast<unsigned>(Player::Property::Destination))
			name += (name.length() > 0 ? ", " : "") + "destination"s;
		if (properties & static_cast<unsigned>(Player::Property::FacingDirection))
			name += (name.length() > 0 ? ", " : "") + "facingDirection"s;
		if (properties & static_cast<unsigned>(Player::Property::AimingDirection))
			name += (name.length() > 0 ? ", " : "") + "aimingDirection"s;
		if (properties & static_cast<unsigned>(Player::Property::MovementSpeed))
			name += (name.length() > 0 ? ", " : "") + "movementSpeed"s;
		if (properties & static_cast<unsigned>(Player::Property::TurningSpeed))
			name += (name.length() > 0 ? ", " : "") + "turningSpeed"s;
		std::string lastCombiner{ " and" };
		size_t lastCombinerPosition{ name.rfind(",") };
		if (lastCombinerPosition != std::string::npos)
			name.replace(name.begin() + lastCombinerPosition, name.begin() + lastCombinerPosition + 1, lastCombiner.begin(), lastCombiner.end());
	}
}

#endif // !LUAPLAYER