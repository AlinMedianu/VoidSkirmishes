#include <optional>
#include <SFML/Window/Clipboard.hpp>
#include <SFML/Window/Event.hpp>
#include "Math.h"
#include "Debug.h"
#include "LuaBrain.h"
#include "NetworkConnection.h"
#include "Character.h"

void game(Network::Connection&& connection, Lua::Brain&& brain, sf::RenderWindow& window, sf::Text& message)
{
    Debug logger(message);
    Character player(window.getSize().x / 16.f, brain.GetPosition(), Math::DirectionToAngle(brain.GetFacingDirection()), sf::Color::Green), 
        enemy(window.getSize().x / 16.f, {}, {}, sf::Color::Red);
    HealthBar playerHealthBar(player.GetBounds(), { 0.8f, 0.05f }, 100, sf::Color::Green),
        enemyHealthBar(enemy.GetBounds(), { 0.8f, 0.05f }, 100, sf::Color::Red);
    player.healthBar = &playerHealthBar;
    enemy.healthBar = &enemyHealthBar;
    Laser playerLaser({ window.getSize().x / 24.f, window.getSize().y * 2.f }, sf::Color::Green),
        enemyLaser({ window.getSize().x / 24.f, window.getSize().y * 2.f }, sf::Color::Red);
    player.laser = &playerLaser;
    enemy.laser = &enemyLaser;
    sf::Vector2f enemyDestination{}, enemyAimingDirection{};
    sf::Int32 health{ player.healthBar->GetHealth().health };
    sf::Vector2f windowSize = static_cast<sf::Vector2f>(window.getSize());
    const sf::FloatRect map(windowSize * 0.1f, windowSize * 0.8f);
    sf::Clock frame;
    Network::Messages::Initial initialReceive{};
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
            Network::Messages::Destination destinationMessage;
            Network::Messages::AimingDirection aimingDirectionMessage;
            Network::Messages::EnemyHealth enemyHealthMessage;
            if (connection.Receive(destinationMessage) == sf::Socket::Done)
            {
                enemyDestination = destinationMessage.destination;                
                logger.Log("Received destination " + std::to_string(enemyDestination.x) +
                    " " + std::to_string(enemyDestination.y));
            }
            else if (connection.Receive(aimingDirectionMessage) == sf::Socket::Done)
            {
                enemy.FakeShoot(Math::AngleToDirection(Math::NormalizeDegrees(enemy.GetRotation())));
                enemyAimingDirection = aimingDirectionMessage.aimingDirection;
                logger.Log("Received aiming direction " + std::to_string(enemyAimingDirection.x) +
                    " " + std::to_string(enemyAimingDirection.y));
            }
            else if (connection.Receive(enemyHealthMessage) == sf::Socket::Done)
            {
                health = enemyHealthMessage.health;
                logger.Log("Received health " + std::to_string(health));
            }
            sf::Vector2f nextPosition = Math::Lerp(enemy.GetPosition(), enemyDestination, deltaTime * brain.GetMovementSpeed());
            enemy.SetPosition(nextPosition);
            float nextRotation = Math::LerpNormalizedAngle(Math::NormalizeDegrees(enemy.GetRotation()),
                Math::DirectionToAngle(enemyAimingDirection), deltaTime * brain.GetTurningSpeed());
            enemy.SetRotation(nextRotation);
            enemy.Update(deltaTime);
            if (brain.SetNextDestination(map) && connection.Send(brain.GetDestination()) == sf::Socket::Done)
                logger.Log("Sent destination " + std::to_string(brain.GetDestination().destination.x) +
                    " " + std::to_string(brain.GetDestination().destination.y));
            if (brain.Aim(enemy) && connection.Send(brain.GetAimingDirection()) == sf::Socket::Done)
            {
                logger.Log("Sent aiming direction " + std::to_string(brain.GetAimingDirection().aimingDirection.x) +
                    " " + std::to_string(brain.GetAimingDirection().aimingDirection.y));
                if (player.Shoot(brain.GetFacingDirection(), enemy) && connection.Send(enemy.healthBar->GetHealth()) == sf::Socket::Done)
                    logger.Log("Sent health " + std::to_string(enemy.healthBar->GetHealth().health));
            }
            nextPosition = Math::Lerp(brain.GetPosition(), brain.GetDestination().destination, deltaTime * brain.GetMovementSpeed());
            player.SetPosition(nextPosition);
            brain.SetPosition(nextPosition);
            nextRotation = Math::LerpNormalizedAngle(Math::DirectionToAngle(brain.GetFacingDirection()), 
                Math::DirectionToAngle(brain.GetAimingDirection().aimingDirection), deltaTime * brain.GetTurningSpeed());
            player.SetRotation(nextRotation);
            brain.SetFacingDirection(Math::AngleToDirection(nextRotation));  
            player.Update(deltaTime); 
            player.healthBar->SetHealth(health);
        }
        //TODO: make initial not have destination or aiming direction, wait for those messages, show both avatars as on the spot
        else if (connection.Receive(initialReceive) == sf::Socket::Done)
        {
            connection.established = true;
            if (connection.server)
            connection.Send(brain.GetInitialMessage());
            enemy.SetPosition(initialReceive.position);
            enemyDestination = initialReceive.destination;
            enemy.SetRotation(Math::DirectionToAngle(initialReceive.facingDirection));
            enemyAimingDirection = initialReceive.aimingDirection;
            message.setCharacterSize(15);
            message.setString("");
            const auto& initialMessage = brain.GetInitialMessage();
            logger.Log("Sent initial message " + std::to_string(initialMessage.position.x) +
                " " + std::to_string(initialMessage.position.y) + " " + std::to_string(initialMessage.destination.x) +
                " " + std::to_string(initialMessage.destination.y) + " " + std::to_string(initialMessage.facingDirection.x) +
                " " + std::to_string(initialMessage.facingDirection.y) + " " + std::to_string(initialMessage.aimingDirection.x) +
                " " + std::to_string(initialMessage.aimingDirection.y) + "\n" + std::to_string(initialReceive.position.x) +
                " " + std::to_string(initialReceive.position.y) + " " + std::to_string(initialReceive.destination.x) +
                " " + std::to_string(initialReceive.destination.y) + " " + std::to_string(initialReceive.facingDirection.x) +
                " " + std::to_string(initialReceive.facingDirection.y) + " " + std::to_string(initialReceive.aimingDirection.x) +
                " " + std::to_string(initialReceive.aimingDirection.y));
        }
        window.clear();
        player.Draw(window);
        if (connection.established)
        {
            enemy.Draw(window);
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
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Void Skirmishes", sf::Style::Default, settings);
    sf::Font arrial;
    arrial.loadFromFile(FontDirectory"arial.ttf");
    sf::Text message("Do you want to be a server or a client?", arrial, 24);
    message.setStyle(sf::Text::Bold);
    message.setPosition(window.getSize().x / 2.f, window.getSize().y / 10.f);
    sf::Text answer(message);
    message.setOrigin(message.getLocalBounds().left + message.getLocalBounds().width / 2.f, message.getLocalBounds().top + message.getLocalBounds().height / 2.f);
    Role role(Role::Undecided);
    std::optional<sf::String> address;
    answer.setString("");
    answer.setPosition(window.getSize().x / 64.f, window.getSize().y / 2.8f);
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
                if (event.key.control && event.key.code == sf::Keyboard::V)
                    answer.setString(answer.getString() + sf::Clipboard::getString());
                else if (event.key.code >= sf::Keyboard::A && event.key.code <= sf::Keyboard::Z)
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
                            Lua::Brain brain("move&aim.lua", { window.getSize().x * 0.25f, window.getSize().y * 0.25f }, { 0, 1 });
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
                            Lua::Brain brain("move&aim.lua", { window.getSize().x * 0.75f, window.getSize().y * 0.7f }, { 0, -1 });
#else
                            Lua::Brain brain("move&aim - Copy.lua", { window.getSize().x * 0.75f, window.getSize().y * 0.75f }, { 0, -1 });
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
