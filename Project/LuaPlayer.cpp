#include "LuaPlayer.h"

namespace Lua
{
	void Player::SyncFrom(const luabridge::LuaRef& player)
	{
		position = player["position"];
		destination = player["destination"];
		movementSpeed = player["movementSpeed"];
		facingDirection = player["facingDirection"];
		aimingDirection = player["aimingDirection"];
		turningSpeed = player["turningSpeed"];
	}

	void Player::Sync(luabridge::LuaRef& player) const
	{
		player["position"] = position;
		player["destination"] = destination;
		player["movementSpeed"] = movementSpeed;
		player["facingDirection"] = facingDirection;
		player["aimingDirection"] = aimingDirection;
		player["turningSpeed"] = turningSpeed;
	}

	bool Player::HasDifferentProperties(const luabridge::LuaRef& than, unsigned propertiesToNotCheck)
	{
		return ~propertiesToNotCheck & static_cast<unsigned>(Property::Position) && position != than["position"].cast<sf::Vector2f>() ||
			~propertiesToNotCheck & static_cast<unsigned>(Property::Destination) && destination != than["destination"].cast<sf::Vector2f>() ||
			~propertiesToNotCheck & static_cast<unsigned>(Property::FacingDirection) && facingDirection != than["facingDirection"].cast<sf::Vector2f>() ||
			~propertiesToNotCheck & static_cast<unsigned>(Property::AimingDirection) && aimingDirection != than["aimingDirection"].cast<sf::Vector2f>() ||
			~propertiesToNotCheck & static_cast<unsigned>(Property::MovementSpeed) && movementSpeed != than["movementSpeed"] ||
			~propertiesToNotCheck & static_cast<unsigned>(Property::TurningSpeed) && turningSpeed != than["turningSpeed"];
	}
}
