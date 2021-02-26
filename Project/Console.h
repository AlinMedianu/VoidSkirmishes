#ifndef CONSOLE
#define CONSOLE

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Clipboard.hpp>
#include <SFML/Graphics/Text.hpp>

class Console
{
public:
	sf::Text messageField;
	explicit Console(const sf::Text& style, sf::Vector2f position);
	void Update(const sf::Event::KeyEvent& keyboardEvent);
	const sf::String& GetMessage() const;
	void Clear();
};

#endif // !CONSOLE