#ifndef LUABRAIN
#define LUABRAIN

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <LuaBridge/LuaBridge.h>
#include "NetworkMessages.h"
#include "Character.h"
#include "Math.h"
#include "LuaPlayer.h"

namespace Lua
{
	class Brain
	{
		lua_State* state;
		luabridge::LuaRef player;
		Player representation;
		sf::Vector2f initialPosition, initialFacingDirection;
		Network::Messages::Initial initialMessage;
		Network::Messages::Destination onMoveMessage;
		Network::Messages::Aim onRotateMessage;
		static std::string script;
		void SetNextDestination(const sf::FloatRect& map);
		void Aim(const Character& enemy);
		static int AddTracebackToError(lua_State* L);
	public:
		sf::Text codeDebug;
		Brain(const std::string& script, sf::Vector2f position, sf::Vector2f facingDirection);
		void Compile(const sf::FloatRect& map, const Character& enemy) noexcept;
		void Reset() noexcept;
		[[nodiscard]] bool TrySetNextDestination(const sf::FloatRect& map);
		[[nodiscard]] bool TryAim(const Character& enemy);
		[[nodiscard]] float GetMovementSpeed() const;
		[[nodiscard]] float GetTurningSpeed() const;
		[[nodiscard]] sf::Vector2f GetPosition() const;
		void SetPosition(sf::Vector2f newPosition);
		[[nodiscard]] sf::Vector2f GetFacingDirection() const;
		void SetFacingDirection(sf::Vector2f newFacingDirection);
		[[nodiscard]] const Network::Messages::Destination& GetDestination();
		[[nodiscard]] sf::Vector2f GetAimingDirection() const;
		[[nodiscard]] const Network::Messages::Initial& GetInitialMessage();
		~Brain();
	};
}

#endif // !LUABRAIN