#include "Laser.h"

Laser::Laser(sf::Vector2f size, const sf::Color& colour)
	:body{ size }, lifetimeTimer{}
{
	body.setOrigin({ body.getLocalBounds().left + body.getLocalBounds().width / 2.f, 
		body.getLocalBounds().top + body.getLocalBounds().height });
	body.setFillColor(colour);
}

bool Laser::TryHit(const std::array<sf::Vector2f, 4>& otherIntersectionRectangle, 
	sf::Vector2f from, sf::Vector2f direction, float duration)
{
	body.setPosition(from);
	body.setRotation(Math::DirectionToAngle(direction));
	lifetimeTimer = duration;
	std::array<sf::Vector2f, 4> intersectionRectangle{};
	Math::LocalRectangleToGlobal(body.getLocalBounds(), body, intersectionRectangle);
	return Math::AreRectanglesIntersecting(intersectionRectangle, otherIntersectionRectangle);
}

void Laser::Update(float deltaTime)
{
	lifetimeTimer = std::max(lifetimeTimer - deltaTime, 0.f);
}

void Laser::Draw(sf::RenderWindow& on) const
{
	if (lifetimeTimer != 0)
		on.draw(body);
}
