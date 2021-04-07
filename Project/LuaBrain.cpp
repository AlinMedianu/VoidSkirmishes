#include "LuaBrain.h"
namespace Lua
{
	std::string Brain::script = "";

	Brain::Brain(const std::string& script, sf::Vector2f position, sf::Vector2f facingDirection)
		:state{ luaL_newstate() }, player(luabridge::newTable(state)), representation{},
		initialPosition{ position }, initialFacingDirection{ facingDirection },
		initialMessage{}, onMoveMessage{}, onRotateMessage{}, errorMessages{}
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
		representation.SyncFrom(player);
		player = luabridge::newTable(state);
		Reset();
		luabridge::setGlobal(state, player, "player");		
		luaL_dofile(state, (InputDirectory + script).c_str());
		representation.Sync(player);
		if (TryCallSetNextDestination(map) && TryCallAim(enemy))
			errorMessages.setString("");
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

	int Brain::AddTracebackToError(lua_State* state)
	{
		std::string errorMessage = lua_tostring(state, -1);
		luaL_traceback(state, state, errorMessage.c_str(), errorMessage.find(script) != std::string::npos ? 1 : 2);
		lua_remove(state, -2);
		return 1;
	};

	bool Brain::TrySetNextDestination(const sf::FloatRect& map)
	{
		if (!player["position"].isInstance<sf::Vector2f>() ||
			!player["destination"].isInstance<sf::Vector2f>() || 
			player["position"].cast<sf::Vector2f>() == player["destination"].cast<sf::Vector2f>())
		{
			if (!TryCallSetNextDestination(map))
				return false;
			player["destination"] = Math::Confine(player["destination"].cast<sf::Vector2f>(), map);
			return player["position"].cast<sf::Vector2f>() != player["destination"].cast<sf::Vector2f>();
		}
		return false;
	}

	bool Brain::TryCallSetNextDestination(const sf::FloatRect& map)
	{
		return TryCall("setNextDestination", map, Player::Property::Destination);
	}

	bool Brain::TryAim(const Character& enemy)
	{
		if (!player["facingDirection"].isInstance<sf::Vector2f>() ||
			!player["aimingDirection"].isInstance<sf::Vector2f>() || 
			Math::ApproximativelyEqual(player["facingDirection"].cast<sf::Vector2f>(),
			Math::Normalize(player["aimingDirection"].cast<sf::Vector2f>())))
		{		
			return TryCallAim(enemy) &&
				!Math::ApproximativelyEqual(player["facingDirection"].cast<sf::Vector2f>(),
				Math::Normalize(player["aimingDirection"].cast<sf::Vector2f>()));
		}
		return false;
	}

	bool Brain::TryCallAim(const Character& enemy)
	{
		return TryCall("aim", enemy, Player::Property::AimingDirection);
	}

	void Brain::AddErrorMessages(std::string&& message)
	{
		if (!errorMessages.getString().isEmpty())
			errorMessages.setString(errorMessages.getString() + "\n\n" + message);
		else
			errorMessages.setString(message);
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

	sf::Vector2f Brain::GetAimingDirection() const
	{		
		return Math::Normalize(player["aimingDirection"].cast<sf::Vector2f>());
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
