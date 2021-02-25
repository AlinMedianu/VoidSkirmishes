#include "Character.h"

Character::Character(float radius, sf::Vector2f position, float rotation, float scale, const sf::Color& colour)
	: bodyTexture(), body(), healthBar{ nullptr }, laser{ nullptr }
{
	bodyTexture.loadFromFile(SpriteDirectory"Player.png");
	body.setTexture(bodyTexture);
	body.setPosition(position);
	body.setRotation(rotation);
	body.setScale(scale, scale);
	body.setOrigin(body.getLocalBounds().left + body.getLocalBounds().width / 2.f, body.getLocalBounds().top + body.getLocalBounds().height / 2.f);
	body.setColor(colour);
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

bool Character::Shoot(sf::Vector2f towards, Character& enemy)
{
	std::array<sf::Vector2f, 4> enemyIntersectionRectangle{};
	Math::LocalRectangleToGlobal(enemy.body.getLocalBounds(), enemy.body, enemyIntersectionRectangle);
	if (laser != nullptr && laser->TryHit(enemyIntersectionRectangle, body.getPosition(), towards, 0.5f) &&
		enemy.healthBar != nullptr)
	{
		enemy.healthBar->IncrementHealth(-5);
		return true;
	}
	return false;
}

void Character::FakeShoot(sf::Vector2f towards)
{
	if (laser != nullptr)
		laser->Flash(body.getPosition(), towards, 0.5f);
}
