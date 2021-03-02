#include "LuaBrain.h"

namespace Lua
{
	std::string Brain::script = "";

	Brain::Brain(const std::string& script, sf::Vector2f position, sf::Vector2f facingDirection)
		:state{ luaL_newstate() }, player(luabridge::newTable(state)), representation{},
		initialPosition{ position }, initialFacingDirection{ facingDirection },
		initialMessage{}, onMoveMessage{}, onRotateMessage{}, codeDebug{}
	{
		Brain::script = script;
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
		Reset();
		luabridge::setGlobal(state, player, "player");
	}

	void Brain::Compile(const sf::FloatRect& map, const Character& enemy) noexcept
	{
		if (!luaL_dofile(state, (InputDirectory + script).c_str()))
			codeDebug.setString("");
		representation.SyncFrom(player);
		SetNextDestination(map);
		if (codeDebug.getString() == "")
			Aim(enemy);
		representation.Sync(player);
	}

	void Brain::Reset() noexcept
	{
		player["position"] = initialPosition;
		player["destination"] = initialPosition;
		player["movementSpeed"] = 100;
		player["facingDirection"] = initialFacingDirection;
		player["aimingDirection"] = initialFacingDirection;
		player["turningSpeed"] = 10;
	}

	bool Brain::TrySetNextDestination(const sf::FloatRect& map)
	{
		if (player["position"].cast<sf::Vector2f>() == player["destination"].cast<sf::Vector2f>())
		{
			SetNextDestination(map);
			player["destination"] = Math::Confine(player["destination"].cast<sf::Vector2f>(), map);
			return player["position"].cast<sf::Vector2f>() != player["destination"].cast<sf::Vector2f>();
		}
		return false;
	}

	int Brain::AddTracebackToError(lua_State* state)
	{
		std::string errorMessage = lua_tostring(state, -1);
		luaL_traceback(state, state, errorMessage.c_str(), errorMessage.find(script) != std::string::npos ? 1 : 2);
		lua_remove(state, -2);
		return 1;
	};

	void Brain::SetNextDestination(const sf::FloatRect& map)
	{		
		lua_pushcfunction(state, AddTracebackToError);
		player["setNextDestination"].push();
		luabridge::Stack<sf::FloatRect>::push(state, map);
		if (lua_pcall(state, 1, 1, -3))
		{		
			std::string debugMessage;
			if (lua_type(state, 1) == LUA_TSTRING)
				debugMessage = lua_tostring(state, 1);
			else
			{
				lua_pushvalue(state, 1);				
				debugMessage = lua_tostring(state, -2);
			}
			codeDebug.setString(debugMessage);
			lua_pop(state, 1);
		}
		lua_pop(state, 1);
		lua_settop(state, 0);
	}

	bool Brain::TryAim(const Character& enemy)
	{
		if (Math::ApproximativelyEqual(player["facingDirection"].cast<sf::Vector2f>(),
			Math::Normalize(player["aimingDirection"].cast<sf::Vector2f>())))
		{
			Aim(enemy);
			return !Math::ApproximativelyEqual(player["facingDirection"].cast<sf::Vector2f>(),
				Math::Normalize(player["aimingDirection"].cast<sf::Vector2f>()));
		}
		return false;
	}

	void Brain::Aim(const Character& enemy)
	{
		lua_pushcfunction(state, AddTracebackToError);
		player["aim"].push();
		luabridge::Stack<Character>::push(state, enemy);
		if (lua_pcall(state, 1, 1, -3))
		{
			std::string debugMessage;
			if (lua_type(state, 1) == LUA_TSTRING)
				debugMessage = lua_tostring(state, 1);
			else
			{
				lua_pushvalue(state, 1);
				debugMessage = lua_tostring(state, -2);
			}
			if (codeDebug.getString() != "")
				codeDebug.setString(codeDebug.getString() + "\n\n" + debugMessage);
			else
				codeDebug.setString(debugMessage);
			lua_pop(state, 1);
		}
		lua_pop(state, 1);
		lua_settop(state, 0);
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
		initialMessage.facingDirection = player["facingDirection"].cast<sf::Vector2f>();
		return initialMessage;
	}

	Brain::~Brain()
	{
		player = luabridge::Nil{};
		lua_close(state);
	}
}
