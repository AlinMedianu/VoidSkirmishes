#ifndef LUABRAIN
#define LUABRAIN

#include <SFML/Graphics/Rect.hpp>
#include <LuaBridge/LuaBridge.h>
#include "NetworkMessages.h"

namespace Lua
{
	class Brain
	{
		lua_State* state;
		luabridge::LuaRef player;
		Network::Destination onMoveMessage;
		Network::PositionDestination initialMessage;
	public:
		Brain(const std::string& script, sf::Vector2f position);
		bool Move(const sf::FloatRect& map);
		float GetSpeed() const;
		sf::Vector2f GetPosition() const;
		void SetPosition(const sf::Vector2f& newPosition);
		const Network::Destination& GetDestination();
		const Network::PositionDestination& GetInitialMessage();
		~Brain();
	};
}

#endif // !LUABRAIN