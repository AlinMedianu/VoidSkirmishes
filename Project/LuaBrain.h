#ifndef LUABRAIN
#define LUABRAIN

#include <SFML/Graphics/Rect.hpp>
#include <LuaBridge/LuaBridge.h>
#include "NetworkMessages.h"
#include "Character.h"
#include "Math.h"

namespace Lua
{
	class Brain
	{
		lua_State* state;
		luabridge::LuaRef player;
		sf::Vector2f position, facingDirection;
		Network::Messages::Initial initialMessage;
		Network::Messages::Destination onMoveMessage;
		Network::Messages::AimingDirection onRotateMessage;
	public:
		Brain(const std::string& script, sf::Vector2f position, sf::Vector2f facingDirection);
		void Reset();
		[[nodiscard]]bool SetNextDestination(const sf::FloatRect& map);
		[[nodiscard]]bool Aim(const Character& enemy);
		[[nodiscard]]float GetMovementSpeed() const;
		[[nodiscard]]float GetTurningSpeed() const;
		[[nodiscard]]sf::Vector2f GetPosition() const;
		void SetPosition(sf::Vector2f newPosition);
		[[nodiscard]]sf::Vector2f GetFacingDirection() const;
		void SetFacingDirection(sf::Vector2f newFacingDirection);
		[[nodiscard]]const Network::Messages::Destination& GetDestination();
		[[nodiscard]]const Network::Messages::AimingDirection& GetAimingDirection();
		[[nodiscard]]const Network::Messages::Initial& GetInitialMessage();
		~Brain();
	};
}

#endif // !LUABRAIN