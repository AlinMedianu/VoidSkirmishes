#include "LuaBrain.h"

namespace Lua
{
	Brain::Brain(const std::string& script, sf::Vector2f position) 
		:state{ luaL_newstate() }, player(luabridge::newTable(state)),
		onMoveMessage{}, initialMessage{}
	{
		luaL_openlibs(state);
		luabridge::getGlobalNamespace(state).
			beginClass<sf::Vector2f>("Vector2").
			addProperty("x", &sf::Vector2f::x).
			addProperty("y", &sf::Vector2f::y).
			endClass().
			beginClass<sf::FloatRect>("Rect").
			addProperty("x", &sf::FloatRect::left).
			addProperty("y", &sf::FloatRect::top).
			addProperty("width", &sf::FloatRect::width).
			addProperty("height", &sf::FloatRect::height).
			endClass();
		player["speed"] = 100;
		player["position"] = position;
		player["destination"] = position;
		luabridge::setGlobal(state, player, "player");
		luaL_dofile(state, (InputDirectory + script).c_str());
	}

	bool Brain::Move(const sf::FloatRect& map)
	{
		if (player["position"].cast<sf::Vector2f>() == player["destination"].cast<sf::Vector2f>())
		{
			player["setNextDestination"](map);
			return false;
		}
		return true;
	}

	float Brain::GetSpeed() const
	{
		return player["speed"].cast<float>();
	}

	sf::Vector2f Brain::GetPosition() const
	{
		return player["position"].cast<sf::Vector2f>();
	}

	void Brain::SetPosition(const sf::Vector2f& newPosition)
	{
		player["position"] = newPosition;
	}

	const Network::Destination& Brain::GetDestination()
	{
		onMoveMessage.destination = player["destination"].cast<sf::Vector2f>();
		return onMoveMessage;
	}

	const Network::PositionDestination& Brain::GetInitialMessage()
	{
		initialMessage.position = player["position"].cast<sf::Vector2f>();
		initialMessage.destination = player["destination"].cast<sf::Vector2f>();
		return initialMessage;
	}

	Brain::~Brain()
	{
		player = luabridge::Nil{};
		lua_close(state);
	}
}
