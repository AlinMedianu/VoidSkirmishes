#include <string>
#include <cmath>
#include <optional>
#include <ctime>
#include "LuaBrain.h"
#include "NetworkConnection.h"

float Length(sf::Vector2f vector)
{
    return sqrtf(powf(vector.x, 2) + powf(vector.y, 2));
}

sf::Vector2f Normalize(sf::Vector2f vector)
{
    return vector / Length(vector);
}

sf::Vector2f Lerp(sf::Vector2f start, sf::Vector2f end, float step)
{
    sf::Vector2f to = end - start;
    if (to == sf::Vector2f(0, 0))
        return start;
    float length = Length(to);
    return start + to / length * fminf(length, step);
}

void game(Network::Connection&& connection, Lua::Brain&& brain, sf::RenderWindow& window, sf::Text& message)
{
    sf::CircleShape player(100.f, 3);
    player.setPosition(brain.GetPosition());
    player.setOrigin(player.getLocalBounds().left + player.getLocalBounds().width / 2.f, player.getLocalBounds().top + player.getLocalBounds().height / 2.f);
    player.setFillColor(sf::Color::Green);
    sf::CircleShape enemy(player);
    enemy.setFillColor(sf::Color::Red);
    sf::Vector2f enemyDestination{};
    const sf::FloatRect map({ 0, 0 }, static_cast<sf::Vector2f>(window.getSize()));
    sf::Clock frame;
    Network::PositionDestination initialReceive{};
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            }
        }
        float deltaTime = frame.restart().asSeconds();
        if (connection.established)
        {
            if (!brain.Move(map))
            {
                if (connection.Send(brain.GetDestination()) == sf::Socket::Done)
                {
                    std::time_t t = std::time(nullptr);
                    std::tm tm = *std::localtime(&t);
                    message.setString(message.getString() + "\nSent destination at " + std::to_string(tm.tm_min) + ":" + std::to_string(tm.tm_sec));
                }
            }
            sf::Vector2f nextPosition = Lerp(brain.GetPosition(), brain.GetDestination().destination, deltaTime * brain.GetSpeed());
            player.setPosition(nextPosition);
            brain.SetPosition(nextPosition);
            Network::Destination destinationMessage;
            if (connection.Receive(destinationMessage) == sf::Socket::Done)
            {
                enemyDestination = destinationMessage.destination;
                std::time_t t = std::time(NULL);
                std::tm tm = *std::localtime(&t);
                message.setString(message.getString() + "\nReceived destination at " + std::to_string(tm.tm_min) + ":" + std::to_string(tm.tm_sec));
            }
            enemy.setPosition(Lerp(enemy.getPosition(), enemyDestination, deltaTime * brain.GetSpeed()));
        }
        else if (connection.Receive(initialReceive) == sf::Socket::Done)
        {
            connection.established = true;
            connection.Send(brain.GetInitialMessage());
            enemy.setPosition(initialReceive.position);
            enemyDestination = initialReceive.destination;
            message.setCharacterSize(15);
            std::time_t t = std::time(nullptr);
            std::tm tm = *std::localtime(&t);
            message.setString("Sent initial message at " + std::to_string(tm.tm_min) + ":" + std::to_string(tm.tm_sec));
        }
        window.clear();
        if (connection.established)
        {
            window.draw(player);
            window.draw(enemy);
        }
        window.draw(message);
        window.display();
    }
}

enum class Role { Undecided, Server, Client };

int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Lerping triangle", sf::Style::Default, settings);
    sf::Font arrial;
    arrial.loadFromFile(FontDirectory"arial.ttf");
    sf::Text message("Do you want to be a server or a client?", arrial, 24);
    message.setStyle(sf::Text::Bold);
    message.setPosition((window.getSize().x - message.getGlobalBounds().width) / 2, 100);
    sf::Text answer(message);
    Role role(Role::Undecided);
    std::optional<sf::String> address;
    answer.setString("");
    answer.setPosition(20, 200);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                if (event.key.code >= sf::Keyboard::A && event.key.code <= sf::Keyboard::Z)
                    answer.setString(answer.getString() + static_cast<char>(event.key.code + 'a'));
                else if (event.key.code >= sf::Keyboard::Num0 && event.key.code <= sf::Keyboard::Num9)
                    answer.setString(answer.getString() + std::to_string(event.key.code - sf::Keyboard::Num0));
                else if (event.key.code == sf::Keyboard::Period)
                    answer.setString(answer.getString() + '.');
                if (event.key.code == sf::Keyboard::Backspace)
                {
                    sf::String result(answer.getString());
                    size_t currentSize = result.getSize();
                    if (currentSize > 0)
                    {
                        result.erase(currentSize - 1);
                        answer.setString(result);
                    }
                }
                if (event.key.code == sf::Keyboard::Enter)
                    switch (role)
                    {
                    case Role::Undecided:
                        if (answer.getString() == "server")
                        {
                            role = Role::Server;
                            message.setString("");
                            answer.setString("");
                            Network::Connection server(message);
                            Lua::Brain brain("move&shoot.lua", { 300, 100 });
                            game(std::move(server), std::move(brain), window, message);
                        }
                        else if (answer.getString() == "client")
                        {
                            role = Role::Client;
                            message.setString("What address do you want to connect to "
                                "\n(leave empty for localhost)?");
                            answer.setString("");                                   
                        }
                        break;
                    case Role::Client:
                        if (!address.has_value())
                        {
                            address = answer.getString();
                            message.setString("What port do you want to connect to?");
                            answer.setString("");
                        }
                        else
                        {
                            sf::String port(answer.getString());
                            message.setString("");
                            answer.setString("");
                            Network::Connection client(address.value(), port, message);
#ifndef _DEBUG
                            Lua::Brain brain("move&shoot.lua", { 980, 500 });
#else
                            Lua::Brain brain("move&shoot - Copy.lua", { 980, 500 });
#endif
                            client.Send(brain.GetInitialMessage());
                            game(std::move(client), std::move(brain), window, message);
                        }
                        break;
                    }                   
                break;
            }
        }
        window.clear();
        window.draw(message);
        window.draw(answer);
        window.display();
    }
    return EXIT_SUCCESS;
}
