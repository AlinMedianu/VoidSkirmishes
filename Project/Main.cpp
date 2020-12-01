#include <string>
#include <cmath>
#include <optional>
#include <LuaBridge/LuaBridge.h>
#include <SFML/Graphics.hpp>
#include "Server.h"
#include "Client.h"

float Length(const sf::Vector2f& vector)
{
    return sqrtf(powf(vector.x, 2) + powf(vector.y, 2));
}

sf::Vector2f Normalize(const sf::Vector2f& vector)
{
    return vector / Length(vector);
}

sf::Vector2f MoveTowards(const sf::Vector2f& start, const sf::Vector2f& end, float step)
{
    sf::Vector2f to = end - start;
    if (to == sf::Vector2f(0, 0))
    {
        return start;
    }
    float length = Length(to);
    return start + to / length * fminf(length, step);
}

void serverMain(sf::RenderWindow& window, sf::Text& message)
{
    Server server(message);
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    luabridge::getGlobalNamespace(L).
        beginClass<sf::Vector2f>("Vector2f").
        addProperty("x", &sf::Vector2f::x).
        addProperty("y", &sf::Vector2f::y).
        addConstructor<void(*)(void)>().
        endClass();
    sf::CircleShape triangle(100.f, 3);
    triangle.setFillColor(sf::Color::Green);
    luaL_dofile(L, InputDirectory"triangle.lua");
    triangle.setPosition(luabridge::getGlobal(L, "destination").cast<sf::Vector2f>());
    sf::Clock clock;
    sf::Packet toSend, toReceive;
    sf::Vector2f previousDestination;
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
        luaL_dofile(L, InputDirectory"triangle.lua");
        sf::Vector2f destination = luabridge::getGlobal(L, "destination").cast<sf::Vector2f>();
        float speed = luabridge::getGlobal(L, "speed").cast<float>();
        triangle.setPosition(MoveTowards(triangle.getPosition(), destination, clock.restart().asSeconds() * speed));
        if (server.Receive(toReceive) == sf::Socket::Done || previousDestination != triangle.getPosition())
        {
            previousDestination = triangle.getPosition();
            toSend << previousDestination.x << previousDestination.y;
            while(server.Send(toSend) == sf::Socket::Partial);
            toSend.clear();
        }
        window.clear();
        window.draw(triangle);
        window.draw(message);
        window.display();
    }
    lua_close(L);
}

void clientMain(sf::RenderWindow& window, sf::String address, sf::String port, sf::Text& message)
{
    Client client(address, port, message);
    sf::CircleShape triangle(100.f, 3);
    sf::Packet packet;
    sf::Clock clock;
    triangle.setFillColor(sf::Color::Green);
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
        if (client.Receive(packet) == sf::Socket::Done)
        {
            float x, y;
            packet >> x >> y;
            message.setString("Received x: " + std::to_string(x) + " and y: " + std::to_string(y) + 
                "\nat frame: " + std::to_string(clock.getElapsedTime().asSeconds()));
            triangle.setPosition(x, y);
        }
        window.clear();
        window.draw(triangle);
        window.draw(message);
        window.display();
    }
}

enum class Role { Undecided, Server, Client };

int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Lerping triangle", sf::Style::Default, settings);
    sf::Font arrial;
    arrial.loadFromFile(FontDirectory"arial.ttf");
    sf::Text message("Do you want to be a server or a client?", arrial, 24);
    message.setStyle(sf::Text::Bold);
    message.setPosition((800 - message.getGlobalBounds().width) / 2, 100);
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
                            serverMain(window, message);
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
                            clientMain(window, address.value(), port, message);
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
