#ifndef HEALTHBAR
#define HEALTHBAR

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include "NetworkMessages.h"

class HealthBar
{
	const float width;
	const sf::Int32 maxHealth;
	sf::Int32 health;
	Network::Messages::EnemyHealth healthMessage;
	sf::RectangleShape image;
	void UpdateSize();
public:
	explicit HealthBar(const sf::FloatRect& characterBounds, sf::Vector2f sizeModifier, sf::Int32 health, const sf::Color& colour);
	void IncrementHealth(sf::Int32 increment);
	[[nodiscard]] const Network::Messages::EnemyHealth& GetHealth() noexcept;
	void SetHealth(sf::Int32 newHealth);
	void MoveTo(const sf::FloatRect& characterBounds);
	void Draw(sf::RenderWindow& on) const;
};

#endif // !HEALTHBAR