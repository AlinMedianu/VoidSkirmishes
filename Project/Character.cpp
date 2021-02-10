#include "Character.h"

Character::Character(float radius, sf::Vector2f position, float rotation, const sf::Color& colour)
	: body(radius, 3), healthBar{ nullptr }, laser{ nullptr }
{
	body.setPosition(position);
	body.setRotation(rotation);
	body.setOrigin(body.getLocalBounds().left + body.getLocalBounds().width / 2.f, body.getLocalBounds().top + body.getLocalBounds().height / 2.f);
	body.setFillColor(colour);
}

void Character::AddHealthBar(HealthBar& healthBar) noexcept
{
	this->healthBar = &healthBar;
}

void Character::AddLaser(Laser& laser) noexcept
{
	this->laser = &laser;
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

void Character::Update(float deltaTime)
{
	if (laser != nullptr)
		laser->Update(deltaTime);
}

void Character::Draw(sf::RenderWindow& on)
{
	on.draw(body);
	if (healthBar != nullptr)
		healthBar->Draw(on);
	if (laser != nullptr)
		laser->Draw(on);
}

int Character::Shoot(sf::Vector2f towards, Character& enemy)
{
	std::array<sf::Vector2f, 4> enemyIntersectionRectangle{};
	Math::LocalRectangleToGlobal(enemy.body.getLocalBounds(), enemy.body, enemyIntersectionRectangle);
	if (laser != nullptr && laser->TryHit(enemyIntersectionRectangle, body.getPosition(), towards, 0.5f) &&
		enemy.healthBar != nullptr)
	{
		std::time_t t = std::time(nullptr);
		std::tm tm = *std::localtime(&t);
		enemy.healthBar->IncrementHealth(-5);
		return -5;
	}
	return 0;
}
