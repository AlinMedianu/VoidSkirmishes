#ifndef CHARACTER
#define CHARACTER

#include <SFML/Graphics/CircleShape.hpp>
#include "HealthBar.h"
#include "Laser.h"

class Character
{
	sf::CircleShape body;
public:
	Laser* laser;
	HealthBar* healthBar;
	explicit Character(float radius, sf::Vector2f position, float rotation, const sf::Color& colour);
	[[nodiscard]] sf::FloatRect GetBounds() const;
	[[nodiscard]] sf::Vector2f GetPosition() const;
	void SetPosition(sf::Vector2f newPosition);
	[[nodiscard]] float GetRotation() const;
	void SetRotation(float newAngle);
	void Update(float deltaTime);
	void Draw(sf::RenderWindow& on);
	[[nodiscard]] bool Shoot(sf::Vector2f towards, Character& enemy);
	void FakeShoot(sf::Vector2f towards);
};

#endif // !CHARACTER