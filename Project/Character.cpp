#include "Character.h"

Character::Character(float radius, sf::Vector2f position, float rotation, const sf::Color& colour)
	: body(radius, 3), healthBar{ nullptr }
{
	body.setPosition(position);
	body.setRotation(rotation);
	body.setOrigin(body.getLocalBounds().left + body.getLocalBounds().width / 2.f, body.getLocalBounds().top + body.getLocalBounds().height / 2.f);
	body.setFillColor(colour);
}

void Character::AddHealthBar(HealthBar& healthbar) noexcept
{
	healthBar = &healthbar;
}

sf::FloatRect Character::GetBounds() const
{
	return body.getGlobalBounds();
}

sf::Vector2f Character::GetPosition() const
{
	return body.getPosition();
}

void Character::SetPosition(sf::Vector2f newPosition)
{
	body.setPosition(newPosition);
	if (healthBar != nullptr)
		healthBar->MoveTo(body.getGlobalBounds());
}

float Character::GetRotation() const
{
	return body.getRotation();
}

void Character::SetRotation(float newAngle)
{
	body.setRotation(newAngle);
}

void Character::Draw(sf::RenderWindow& on)
{
	on.draw(body);
	if (healthBar != nullptr)
		healthBar->Draw(on);
}
