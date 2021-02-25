#ifndef CHARACTER
#define CHARACTER

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "HealthBar.h"
#include "Laser.h"

class Character
{
	sf::Texture bodyTexture;
	sf::Sprite body;
public:
	Laser* laser;
	HealthBar* healthBar;
	explicit Character(float radius, sf::Vector2f position, float rotation, float scale, const sf::Color& colour);
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