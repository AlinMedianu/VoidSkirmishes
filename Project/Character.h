#ifndef CHARACTER
#define CHARACTER

#include <SFML/Graphics/Sprite.hpp>
#include "Resources.h"
#include "HealthBar.h"
#include "Laser.h"

struct ShootContext
{
	bool hit;
	sf::Int32 health;
};

class Character
{
	sf::Sprite body;
	sf::Transformable nonRotatedBody;
public:
	Laser* laser;
	HealthBar* healthBar;
	explicit Character(Resources& resources, sf::Vector2f position, float rotation, float scale, const sf::Color& colour);
	[[nodiscard]] sf::FloatRect GetBounds() const;
	[[nodiscard]] sf::Vector2f GetPosition() const;
	void SetPosition(sf::Vector2f newPosition);
	[[nodiscard]] float GetRotation() const;
	void SetRotation(float newAngle);
	void Update(float deltaTime);
	void Draw(sf::RenderWindow& on);
	[[nodiscard]] ShootContext Shoot(sf::Vector2f towards, Character& enemy);
	void FakeShoot(sf::Vector2f towards);
};

#endif // !CHARACTER