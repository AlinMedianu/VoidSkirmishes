#include "Console.h"

Console::Console(const sf::Text& style, sf::Vector2f position) : messageField(style)
{
	messageField.setPosition(position);
	messageField.setString("");
}

void Console::Update(const sf::Event::KeyEvent& keyboardEvent)
{
    if (keyboardEvent.control && keyboardEvent.code == sf::Keyboard::V)
        messageField.setString(messageField.getString() + sf::Clipboard::getString());
    else if (keyboardEvent.code >= sf::Keyboard::A && keyboardEvent.code <= sf::Keyboard::Z)
        messageField.setString(messageField.getString() + static_cast<char>(keyboardEvent.code + 'a'));
    else if (keyboardEvent.code >= sf::Keyboard::Num0 && keyboardEvent.code <= sf::Keyboard::Num9)
        messageField.setString(messageField.getString() + std::to_string(keyboardEvent.code - sf::Keyboard::Num0));
    else if (keyboardEvent.code == sf::Keyboard::Period)
        messageField.setString(messageField.getString() + '.');
    if (keyboardEvent.code == sf::Keyboard::Backspace)
    {
        sf::String result(messageField.getString());
        size_t currentSize = result.getSize();
        if (currentSize > 0)
        {
            result.erase(currentSize - 1);
            messageField.setString(result);
        }
    }
}

const sf::String& Console::GetMessage() const
{
	return messageField.getString();
}

void Console::Clear()
{
	messageField.setString("");
}
