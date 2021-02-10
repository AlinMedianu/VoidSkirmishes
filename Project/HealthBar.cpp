#include "HealthBar.h"

HealthBar::HealthBar(const sf::FloatRect& characterBounds, sf::Vector2f sizeModifier, sf::Int32 health, const sf::Color& colour)
	: width{ characterBounds.width * sizeModifier.x, }, maxHealth{ health }, health{ health }, healthMessage{},
	image({ width, characterBounds.height * sizeModifier.y } )
	 
{
	image.setPosition(characterBounds.left + (characterBounds.width - width) / 2.f,
		characterBounds.top - 2 * image.getSize().y);
	image.setOrigin(image.getLocalBounds().left,
		image.getLocalBounds().top + image.getLocalBounds().height / 2.f);
	image.setFillColor(colour);
}

void HealthBar::UpdateSize()
{
	image.setSize({ width * health / maxHealth, image.getSize().y });
}

void HealthBar::IncrementHealth(sf::Int32 increment)
{
	health = std::clamp(health + increment, 0, maxHealth);
	UpdateSize();
}

const Network::Messages::EnemyHealth& HealthBar::GetHealth() noexcept
{
	healthMessage.health = health;
	return healthMessage;
}

void HealthBar::SetHealth(sf::Int32 newHealth)
{
	health = std::clamp(newHealth, 0, maxHealth);
	UpdateSize();
}

//make it uses the local bounds with applied translation and scale
void HealthBar::MoveTo(const sf::FloatRect& characterBounds)
{
	image.setPosition(characterBounds.left + (characterBounds.width - width) / 2.f,
		characterBounds.top - 2 * image.getSize().y);
}

void HealthBar::Draw(sf::RenderWindow& on) const
{
	if (health > 0)
		on.draw(image);
}
