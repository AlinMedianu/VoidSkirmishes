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
		[[nodiscard]] bool TryCallSetNextDestination(const sf::FloatRect& map);
		[[nodiscard]] bool TryCallAim(const Character& enemy);
		template<typename T>
		[[nodiscard]] bool TryCall(std::string&& functionName, const T& parameter, Player::Property changeableProperties);
		void AddErrorMessages(std::string&& message);
		static int AddTracebackToError(lua_State* L);
	public:
		sf::Text errorMessages;
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

	template<typename T>
	bool Brain::TryCall(std::string&& functionName, const T& parameter, Player::Property changeableProperties)
	{
		bool succeeded{ true };
		Player beforeCall{};
		beforeCall.SyncFrom(player);
		lua_pushcfunction(state, AddTracebackToError);
		player[functionName].push();
		luabridge::Stack<T>::push(state, parameter);
		bool unspecifiedError{ false };
		if (lua_pcall(state, 1, 1, -3))
		{
			succeeded = false;
			std::string debugMessage;
			if (lua_type(state, 1) == LUA_TSTRING)
				debugMessage = lua_tostring(state, 1);
			else
			{
				lua_pushvalue(state, 1);
				debugMessage = lua_tostring(state, -2);
			}
			size_t scriptDebugMessagePosition{ debugMessage.find(script) };
			if (scriptDebugMessagePosition != std::string::npos)
				AddErrorMessages(std::move(debugMessage));
			else
				unspecifiedError = true;
		}
		lua_settop(state, 0);
		unsigned changeablePropertiesMask{ static_cast<unsigned>(changeableProperties) };
		if (!player["position"].isInstance<sf::Vector2f>())
		{
			AddErrorMessages("The player's position has to be a Vector2");
			succeeded = false;
		}
		else if (!player["destination"].isInstance<sf::Vector2f>())
		{
			AddErrorMessages("The player's destination has to be a Vector2");
			succeeded = false;
		}
		else if (!player["facingDirection"].isInstance<sf::Vector2f>())
		{
			AddErrorMessages("The player's facingDirection has to be a Vector2");
			succeeded = false;
		}
		else if (!player["aimingDirection"].isInstance<sf::Vector2f>())
		{
			AddErrorMessages("The player's aimingDirection has to be a Vector2");
			succeeded = false;
		}
		else if (beforeCall.HasDifferentProperties(player, changeablePropertiesMask))
		{
			succeeded = false;
			std::string namesOfChangeableProperties{};
			NameFromPlayerProperties(namesOfChangeableProperties, changeablePropertiesMask);
			AddErrorMessages(script + ": You can only change the player's " +
				namesOfChangeableProperties + " in the function " + functionName + "!");
			unspecifiedError = false;
		}
		if (unspecifiedError)
		{
			if (!player[functionName].isFunction())
				AddErrorMessages(script + ": Function " + functionName + " was not found!");
			else
				AddErrorMessages("Unexpected error in the " + functionName + 
					" function! Please let me know how you got here!");
		}
		return succeeded;
	}
}

#endif // !LUABRAIN