#ifndef HEALTHBAR
#define HEALTHBAR

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include "Math.h"

class HealthBar
{
	const float width;
	float fadeToWidth;
	const sf::Int32 maxHealth;
	sf::Int32 health;
	sf::RectangleShape image, backgroundImage;
	void UpdateSize();
	[[nodiscard]] sf::Int32 ClampHealth(sf::Int32 value) const;
	[[nodiscard]] float ClampImageWidth(float value) const;
	[[nodiscard]] sf::Int32 WidthToHealth(float value) const noexcept;
	[[nodiscard]] float HealthToWidth(sf::Int32 value) const noexcept;
public:
	explicit HealthBar(const sf::FloatRect& characterBounds, sf::Vector2f sizeModifier, sf::Int32 health, const sf::Color& colour);
	[[nodiscard]] sf::Int32 GetHealth() noexcept;
	void IncrementHealth(sf::Int32 increment);
	void SetHealth(sf::Int32 newHealth);
	[[nodiscard]] sf::Int32 FadeIncrementHealth(sf::Int32 increment);
	void FadeHealth(sf::Int32 to);
	void MoveTo(const sf::FloatRect& characterBounds);
	void Update(float deltaTime);
	void Draw(sf::RenderWindow& on) const;
};

#endif // !HEALTHBAR