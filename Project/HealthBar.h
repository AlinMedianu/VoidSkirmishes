#ifndef HEALTHBAR
#define HEALTHBAR

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

class HealthBar
{
	const float width;
	const int maxHealth;
	int health;
	sf::RectangleShape image;
	void UpdateSize();
public:
	explicit HealthBar(const sf::FloatRect& characterBounds, sf::Vector2f sizeModifier, int health, const sf::Color& colour);
	void IncrementHealth(int increment);
	[[nodiscard]] constexpr int GetHealth() const noexcept;
	void SetHealth(int newHealth);
	void MoveTo(const sf::FloatRect& characterBounds);
	void Draw(sf::RenderWindow& on) const;
};

#endif // !HEALTHBAR