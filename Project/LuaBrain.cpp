#include "LuaBrain.h"

namespace Lua
{
	Brain::Brain(const std::string& script, sf::Vector2f position, sf::Vector2f facingDirection)
		:state{ luaL_newstate() }, player(luabridge::newTable(state)),
		initialMessage{}, onMoveMessage{}, onRotateMessage{}
	{
		luaL_openlibs(state);
		luabridge::getGlobalNamespace(state).
			beginClass<sf::Vector2f>("Vector2").
			addProperty("x", &sf::Vector2f::x).
			addProperty("y", &sf::Vector2f::y).
			endClass().
			beginClass<sf::FloatRect>("Rect").
			addProperty("x", &sf::FloatRect::left, false).
			addProperty("y", &sf::FloatRect::top, false).
			addProperty("width", &sf::FloatRect::width, false).
			addProperty("height", &sf::FloatRect::height, false).
			endClass().
			beginClass<Character>("Character").
			addFunction("getPosition", &Character::GetPosition).
			endClass();
		player["position"] = position;
		player["destination"] = position;
		player["movementSpeed"] = 100;
		player["facingDirection"] = facingDirection;
		player["aimingDirection"] = facingDirection;
		player["turningSpeed"] = 10;
		luabridge::setGlobal(state, player, "player");
		luaL_dofile(state, (InputDirectory + script).c_str());
	}

	bool Brain::SetNextDestination(const sf::FloatRect& map)
	{
		if (player["position"].cast<sf::Vector2f>() == player["destination"].cast<sf::Vector2f>())
		{
			//TODO: remove user debug hacking
			try
			{
				player["setNextDestination"](map);
			}
			catch (const luabridge::LuaException& exception)
			{
				lua_getglobal(state, "debug");
				lua_getfield(state, -1, "traceback");
				lua_pushvalue(state, 1);
				lua_pushinteger(state, 2);
				lua_call(state, 2, 1);
				char const* s = lua_tostring(state, -1);
				int i{};
			}
			player["destination"] = Math::Confine(player["destination"].cast<sf::Vector2f>(), map);
			return true;
		}
		return false;
	}

	bool Brain::Aim(const Character& enemy)
	{
		if (Math::ApproximativelyEqual(player["facingDirection"].cast<sf::Vector2f>(),
			Math::Normalize(player["aimingDirection"].cast<sf::Vector2f>())))
		{
			player["aim"](enemy);
			return true;
		}
		return false;
	}

	float Brain::GetMovementSpeed() const
	{
		return player["movementSpeed"].cast<float>();
	}

	float Brain::GetTurningSpeed() const
	{
		return player["turningSpeed"].cast<float>();
	}

	sf::Vector2f Brain::GetPosition() const
	{
		return player["position"].cast<sf::Vector2f>();
	}

	void Brain::SetPosition(sf::Vector2f newPosition)
	{
		player["position"] = newPosition;
	}

	sf::Vector2f Brain::GetFacingDirection() const
	{
		return player["facingDirection"].cast<sf::Vector2f>();
	}

	void Brain::SetFacingDirection(sf::Vector2f newFacingDirection)
	{
		player["facingDirection"] = newFacingDirection;
	}

	const Network::Messages::Destination& Brain::GetDestination()
	{
		onMoveMessage.destination = player["destination"].cast<sf::Vector2f>();
		return onMoveMessage;
	}

	const Network::Messages::AimingDirection& Brain::GetAimingDirection()
	{
		onRotateMessage.aimingDirection = Math::Normalize(player["aimingDirection"].cast<sf::Vector2f>());
		return onRotateMessage;
	}

	const Network::Messages::Initial& Brain::GetInitialMessage()
	{
		initialMessage.position = player["position"].cast<sf::Vector2f>();
		initialMessage.destination = player["destination"].cast<sf::Vector2f>();
		initialMessage.facingDirection = player["facingDirection"].cast<sf::Vector2f>();
		initialMessage.aimingDirection = player["aimingDirection"].cast<sf::Vector2f>();
		return initialMessage;
	}

	Brain::~Brain()
	{
		player = luabridge::Nil{};
		lua_close(state);
	}
}
