#ifndef CHARACTER
#define CHARACTER

#include <SFML/Graphics/CircleShape.hpp>
#include "HealthBar.h"

class Character
{
	sf::CircleShape body;
	HealthBar* healthBar;
public:
	explicit Character(float radius, sf::Vector2f position, float rotation, const sf::Color& colour);
	void AddHealthBar(HealthBar& healthbar) noexcept;
	[[nodiscard]] sf::FloatRect GetBounds() const;
	[[nodiscard]] sf::Vector2f GetPosition() const;
	void SetPosition(sf::Vector2f newPosition);
	[[nodiscard]] float GetRotation() const;
	void SetRotation(float newAngle);
	void Draw(sf::RenderWindow& on);
};

#endif // !CHARACTER