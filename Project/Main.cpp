#include <optional>
#include "Console.h"
#include "Math.h"
#include "Debug.h"
#include "LuaBrain.h"
#include "NetworkConnection.h"
#include "Character.h"

enum class DuelOutcome { Won, Lost, LostConnection, OtherLostConnection };

[[nodiscard]] DuelOutcome Duel(Network::Connection& connection, Lua::Brain& brain, sf::RenderWindow& window, sf::Text& message, Console& console)
{
    Debug logger(message);
    brain.codeDebug.setFont(*message.getFont());
    brain.codeDebug.setCharacterSize(16);
    brain.codeDebug.setStyle(sf::Text::Bold);
    brain.codeDebug.setPosition(window.getSize().x / 10.f, 3 * window.getSize().y / 4.f);
    brain.codeDebug.setOrigin(brain.codeDebug.getLocalBounds().left + brain.codeDebug.getLocalBounds().width / 2.f, 
        brain.codeDebug.getLocalBounds().top + brain.codeDebug.getLocalBounds().height / 2.f);
    Character player(window.getSize().x / 16.f, brain.GetPosition(), Math::DirectionToAngle(brain.GetFacingDirection()), 0.1f, sf::Color::Green), 
        enemy(window.getSize().x / 16.f, {}, {}, 0.1f, sf::Color::Red);
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
    std::optional<sf::Clock> disconnectionTimer;
    Network::Messages::Initial initialReceive{};
    Network::Messages::Pause sentPauseMessage{}, receivedPauseMessage{};
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
                console.Update(event.key);
                break;
            }
        }
        if (disconnectionTimer.has_value() && disconnectionTimer.value().getElapsedTime().asSeconds() > 3)
            return DuelOutcome::OtherLostConnection;
        Network::Messages::Disconnection disconnectionMessage{};
        if (connection.Receive(disconnectionMessage) == sf::Socket::Done)
        {
            connection.established = false;
            message.setCharacterSize(24);
            message.setString("Other player got disconnected!");
            disconnectionTimer.emplace();
        }
        if (connection.Receive(receivedPauseMessage) == sf::Socket::Done)
            logger.Log(receivedPauseMessage.paused ? "Received pause message" : "Received unpause message");
        float deltaTime = frame.restart().asSeconds();          
        if (console.GetMessage() == "c")
        {
            brain.Compile(map, enemy);
            if (brain.codeDebug.getString().isEmpty())
            {
                if (sentPauseMessage.paused)
                {
                    sentPauseMessage.paused = false;
                    if (connection.Send(sentPauseMessage) == sf::Socket::Done)
                        logger.Log("Sent unpause message");
                }
            }
            else if (!sentPauseMessage.paused)
            {
                sentPauseMessage.paused = true;
                if (connection.Send(sentPauseMessage) == sf::Socket::Done)
                    logger.Log("Sent pause message");
            }
        }
        
        console.Clear();
        if (!sentPauseMessage.paused && !receivedPauseMessage.paused)
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
                if (health == 0)
                    //TODO: add health fade timer
                    return DuelOutcome::Lost;
            }
            else
                connection.FlushReceivedMessages();
            sf::Vector2f nextPosition = Math::Lerp(enemy.GetPosition(), enemyDestination, deltaTime * brain.GetMovementSpeed());
            enemy.SetPosition(nextPosition);
            float nextRotation = Math::LerpNormalizedAngle(Math::NormalizeDegrees(enemy.GetRotation()),
                Math::DirectionToAngle(enemyAimingDirection), deltaTime * brain.GetTurningSpeed());
            enemy.SetRotation(nextRotation);
            enemy.Update(deltaTime);
            if (brain.TrySetNextDestination(map) && connection.Send(brain.GetDestination()) == sf::Socket::Done)
                logger.Log("Sent destination " + std::to_string(brain.GetDestination().destination.x) +
                    " " + std::to_string(brain.GetDestination().destination.y));
            if (brain.TryAim(enemy) && connection.Send(brain.GetAimingDirection()) == sf::Socket::Done)
            {
                logger.Log("Sent aiming direction " + std::to_string(brain.GetAimingDirection().aimingDirection.x) +
                    " " + std::to_string(brain.GetAimingDirection().aimingDirection.y));
                if (player.Shoot(brain.GetFacingDirection(), enemy) && connection.Send(enemy.healthBar->GetHealth()) == sf::Socket::Done)
                {
                    logger.Log("Sent health " + std::to_string(enemy.healthBar->GetHealth().health));
                    if (enemy.healthBar->GetHealth().health == 0)
                        //TODO: add health fade timer
                        return DuelOutcome::Won;
                }
            }
            if (!brain.codeDebug.getString().isEmpty())
            {
                sentPauseMessage.paused = true;
                if (connection.Send(sentPauseMessage) == sf::Socket::Done)
                    logger.Log("Sent pause message");
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
        else if (connection.Receive(initialReceive) == sf::Socket::Done)
        {
            if (!connection.established)
                connection.Send(brain.GetInitialMessage());
            connection.established = true;
            enemy.SetPosition(initialReceive.position);
            enemyDestination = initialReceive.position;
            enemy.SetRotation(Math::DirectionToAngle(initialReceive.facingDirection));
            enemyAimingDirection = initialReceive.facingDirection;
            message.setCharacterSize(15);
            message.setString("");
            const auto& initialMessage = brain.GetInitialMessage();
            logger.Log("Sent initial message " + std::to_string(initialMessage.position.x) +
                " " + std::to_string(initialMessage.position.y) +
                " " + std::to_string(initialMessage.facingDirection.x) +
                " " + std::to_string(initialMessage.facingDirection.y) +
                "\nReceived " + std::to_string(initialReceive.position.x) +
                " " + std::to_string(initialReceive.position.y) +
                " " + std::to_string(initialReceive.facingDirection.x) +
                " " + std::to_string(initialReceive.facingDirection.y));
        }     
        window.clear();
        player.Draw(window);
        if (connection.established)
            enemy.Draw(window);
        window.draw(message);
        window.draw(brain.codeDebug);
        window.display();
    }
    return DuelOutcome::LostConnection;
}

[[nodiscard]] bool GameOverScreen(DuelOutcome outcome, Console& console, sf::RenderWindow& window, sf::Text& message)
{
    switch (outcome)
    {
    case DuelOutcome::Won:
        message.setString("You won! Want to play again?(y/n)");
        break;
    case DuelOutcome::Lost:
        message.setString("You lost! Want to take your revenge?(y/n)");
        break;
    }
    console.Clear();
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
                console.Update(event.key);
                if (console.GetMessage() == "y")
                    return true;
                if (console.GetMessage() == "n")
                    return false;
            }
        }
        window.clear();
        window.draw(message);
        window.draw(console.messageField);
        window.display();
    }
    return false;
}

enum class Role { Undecided, Host, Client };

int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Void Skirmishes", sf::Style::Default, settings);
    sf::Font arrial;
    arrial.loadFromFile(FontDirectory"arial.ttf");
    sf::Text message("Do you want to be a host or a client?", arrial, 24);
    message.setStyle(sf::Text::Bold);
    message.setPosition(window.getSize().x / 2.f, window.getSize().y / 10.f);
    Console console(message, { window.getSize().x / 64.f, window.getSize().y / 2.8f });
    message.setOrigin(message.getLocalBounds().left + message.getLocalBounds().width / 2.f, message.getLocalBounds().top + message.getLocalBounds().height / 2.f);
    Role role(Role::Undecided);
    std::optional<sf::String> address;
    std::optional<Network::Connection> connection;
    std::optional<Lua::Brain> brain;
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
                console.Update(event.key);
                if (event.key.code == sf::Keyboard::Enter)
                    switch (role)
                    {
                    case Role::Undecided:
                        if (console.GetMessage() == "host")
                        {
                            role = Role::Host;
                            message.setString("");
                            console.Clear();
                            connection.emplace(message);
                            brain.emplace("move&aim.lua", sf::Vector2f{ window.getSize().x * 0.25f, window.getSize().y * 0.25f }, sf::Vector2f{ 0, 1 });
                        }
                        else if (console.GetMessage() == "client")
                        {
                            role = Role::Client;
                            message.setString("What address do you want to connect to "
                                "\n(leave empty for localhost)?");
                            console.messageField.setString("");
                              
                        }
                        break;
                    case Role::Client:
                        if (!address.has_value())
                        {
                            address = console.GetMessage();
                            message.setString("What port do you want to connect to?");
                            console.Clear();
                        }
                        else
                        {
                            sf::String port = console.GetMessage();
                            message.setString("");
                            console.Clear();
                            connection.emplace(address.value(), port, message);
#ifndef _DEBUG
                            brain.emplace("move&aim.lua", sf::Vector2f{ window.getSize().x * 0.75f, window.getSize().y * 0.75f }, sf::Vector2f{ 0, -1 });
#else
                            brain.emplace("move&aim - Copy.lua", sf::Vector2f{ window.getSize().x * 0.75f, window.getSize().y * 0.75f }, sf::Vector2f{ 0, -1 });
#endif
                            connection->Send(brain->GetInitialMessage());
                        }
                        break;
                    }                   
                break;
            }
        }
        if (connection.has_value())
            switch (role)
            {
            case Role::Host:
            case Role::Client:
                console.Clear();
                DuelOutcome outcome = Duel(connection.value(), brain.value(), window, message, console);
                if (outcome == DuelOutcome::OtherLostConnection)
                {
                    role = Role::Undecided;
                    address.reset();
                    connection.reset();
                    brain.reset();
                    console.Clear();
                    message.setString("Do you want to be a host or a client?");
                    break;
                }
                message.setCharacterSize(24);
                if (GameOverScreen(outcome, console, window, message))
                {
                    message.setString("Waiting for other player's response...");
                    brain->Reset();
                    connection->established = false;
                    connection->Send(brain->GetInitialMessage());
                }
                else
                {
                    connection->Send(Network::Messages::Disconnection{});
                    window.close();
                }
                break;
            }
        window.clear();
        window.draw(message);
        window.draw(console.messageField);
        window.display();
    }
    return EXIT_SUCCESS;
}
