#ifndef DEBUG
#define DEBUG

#include <ctime>
#include <SFML/Graphics/Text.hpp>

class Debug
{
	sf::Text& message;
public:
	Debug(sf::Text& message);
	void Log(const sf::String& entry);
};

#endif // !DEBUG