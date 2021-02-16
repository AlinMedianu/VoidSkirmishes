#include "Debug.h"

Debug::Debug(sf::Text& message) : message(message)
{

}

void Debug::Log(const sf::String& entry)
{
#ifdef _DEBUG
	std::time_t t = std::time(nullptr);
	std::tm tm = *std::localtime(&t);
	message.setString(message.getString() + "\n" + entry + " at " + std::to_string(tm.tm_min) + ":" + std::to_string(tm.tm_sec));
#endif // _DEBUG
}
