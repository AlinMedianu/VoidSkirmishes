#include "HealthBar.h"

HealthBar::HealthBar(const sf::FloatRect& characterBounds, sf::Vector2f sizeModifier, sf::Int32 health, const sf::Color& colour)
	: width{ characterBounds.width * sizeModifier.x, }, fadeToWidth{ width }, maxHealth{ health }, health{ health },
	image({ width, characterBounds.height * sizeModifier.y }), backgroundImage{}
	 
{
	image.setPosition(characterBounds.left + (characterBounds.width - width) / 2.f,
		characterBounds.top - 2 * image.getSize().y);
	image.setOrigin(image.getLocalBounds().left,
		image.getLocalBounds().top + image.getLocalBounds().height / 2.f);
	backgroundImage = image;
	image.setFillColor(colour);
	backgroundImage.setFillColor(sf::Color::White);
	backgroundImage.setOutlineThickness(2);
	backgroundImage.setOutlineColor(sf::Color::White);
}

void HealthBar::UpdateSize()
{
	image.setSize({ HealthToWidth(health), image.getSize().y });
}

sf::Int32 HealthBar::ClampHealth(sf::Int32 value) const
{
	return std::clamp(value, 0, maxHealth);
}

float HealthBar::ClampImageWidth(float value) const
{
	return std::clamp(value, 0.0f, width);
}

sf::Int32 HealthBar::WidthToHealth(float value) const noexcept
{
	return static_cast<sf::Int32>(maxHealth * value / width);
}

float HealthBar::HealthToWidth(sf::Int32 value) const noexcept
{
	return width * value / maxHealth;
}

sf::Int32 HealthBar::GetHealth() noexcept
{
	return health;
}

void HealthBar::IncrementHealth(sf::Int32 increment)
{
	health = ClampHealth(health + increment);
	UpdateSize();
}

void HealthBar::SetHealth(sf::Int32 newHealth)
{
	health = ClampHealth(newHealth);
	UpdateSize();
}

sf::Int32 HealthBar::FadeIncrementHealth(sf::Int32 increment)
{
	sf::Int32 fadeToHealth = ClampHealth(WidthToHealth(fadeToWidth) + increment);
	fadeToWidth = HealthToWidth(fadeToHealth);
	return fadeToHealth;
}

void HealthBar::FadeHealth(sf::Int32 to)
{
	fadeToWidth = HealthToWidth(to);
}

void HealthBar::MoveTo(const sf::FloatRect& characterBounds)
{
	image.setPosition(characterBounds.left + (characterBounds.width - width) / 2.f,
		characterBounds.top - 2 * image.getSize().y);
	backgroundImage.setPosition(image.getPosition());
}

void HealthBar::Update(float deltaTime)
{
	float currentWidth = image.getSize().x;
	if (currentWidth == fadeToWidth)
		return;
	image.setSize({ Math::ConstantIncrement(currentWidth, fadeToWidth, 
		Math::BiasedSign(fadeToWidth - currentWidth) * 50 * deltaTime), image.getSize().y });
	health = WidthToHealth(image.getSize().x);
}

void HealthBar::Draw(sf::RenderWindow& on) const
{
	if (health > 0)
	{
		on.draw(backgroundImage);
		on.draw(image);
	}
}
