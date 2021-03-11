#ifndef UIBUTTON
#define UIBUTTON

#include <array>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include "Resources.h"

namespace UI
{
	class Button
	{		
	public:
		enum class State : size_t { Default, Down, Hover, Inactive, COUNT };
		explicit Button(Resources& resources, sf::Vector2f position, sf::Vector2f scale, std::string&& message);
		void ReactTo(const sf::Event& mouseEvent);
		void Activate();
		void Deactivate();
		[[nodiscard]] bool WasClicked() noexcept;
		[[nodiscard]] State GetCurrentState() const noexcept;
		[[nodiscard]] sf::Vector2f Size() const;
		void Draw(sf::RenderWindow& on);
	private:
		void UpdateTexture();
		bool clicked;
		State current;
		std::array<sf::Texture*, static_cast<size_t>(State::COUNT)> textures;
		sf::Sprite body;
		sf::Text message;
	};
}

#endif // !UIBUTTON