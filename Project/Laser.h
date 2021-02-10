#ifndef LASER
#define LASER

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "Math.h"

class Laser
{
	sf::RectangleShape body;
	float lifetimeTimer;
public:
	explicit Laser(sf::Vector2f size, const sf::Color& colour);
	bool TryHit(const std::array<sf::Vector2f, 4>& otherIntersectionRectangle, 
		sf::Vector2f from, sf::Vector2f direction, float duration);
	void Flash(sf::Vector2f from, sf::Vector2f direction, float duration);
	void Update(float deltaTime);
	void Draw(sf::RenderWindow& on) const;
};

#endif // !LASER