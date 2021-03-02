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
}
