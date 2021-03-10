#include "Character.h"

Character::Character(Resources& resources, sf::Vector2f position, float rotation, float scale, const sf::Color& colour)
	: body(), nonRotatedBody(), healthBar{ nullptr }, laser{ nullptr }
{
	auto playerTexturePath(SpriteDirectory"Player.png");
	auto playerTexture = resources.textures.try_emplace(playerTexturePath);
	if (playerTexture.second)
		playerTexture.first->second.loadFromFile(playerTexturePath);
	body.setTexture(playerTexture.first->second);
	body.setPosition(position);
	body.setScale(scale, scale);
	body.setOrigin(body.getLocalBounds().left + body.getLocalBounds().width / 2.f, body.getLocalBounds().top + body.getLocalBounds().height / 2.f);
	body.setColor(colour);
	nonRotatedBody = body;
	body.setRotation(rotation);
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
	{
		nonRotatedBody.setPosition(newPosition);
		healthBar->MoveTo(nonRotatedBody.getTransform().transformRect(body.getLocalBounds()));
	}
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
	if (healthBar != nullptr)
		healthBar->Update(deltaTime);
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

ShootContext Character::Shoot(sf::Vector2f towards, Character& enemy)
{
	std::array<sf::Vector2f, 4> enemyIntersectionRectangle{};
	Math::LocalRectangleToGlobal(enemy.body.getLocalBounds(), enemy.body, enemyIntersectionRectangle);
	if (laser != nullptr && laser->TryHit(enemyIntersectionRectangle, body.getPosition(), towards, 0.5f) &&
		enemy.healthBar != nullptr)
		return ShootContext{ true, enemy.healthBar->FadeIncrementHealth(-1) };
	return ShootContext{ false, enemy.healthBar->GetHealth() };
}

void Character::FakeShoot(sf::Vector2f towards)
{
	if (laser != nullptr)
		laser->Flash(body.getPosition(), towards, 0.5f);
}
