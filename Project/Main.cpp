#include <optional>
#include "Console.h"
#include "Math.h"
#include "Debug.h"
#include "LuaBrain.h"
#include "NetworkConnection.h"
#include "Character.h"
#include "UIButton.h"

enum class DuelOutcome { Won, Lost, Tie, LostConnection, OtherLostConnection };

[[nodiscard]] DuelOutcome Duel(Resources& resources, Network::Connection& connection, Lua::Brain& brain, sf::RenderWindow& window, sf::Text& message)
{
    UI::Button compile(resources, { window.getSize().x / 10.f, window.getSize().y / 2.f }, 
        { 0.75f, 0.25f }, "Compile!");
    Debug logger(message);
    brain.codeDebug.setFont(*message.getFont());
    brain.codeDebug.setCharacterSize(16);
    brain.codeDebug.setStyle(sf::Text::Bold);
    brain.codeDebug.setPosition(window.getSize().x / 10.f, 3 * window.getSize().y / 4.f);
    brain.codeDebug.setOrigin(brain.codeDebug.getLocalBounds().left + brain.codeDebug.getLocalBounds().width / 2.f, 
        brain.codeDebug.getLocalBounds().top + brain.codeDebug.getLocalBounds().height / 2.f);
    Character player(resources, brain.GetPosition(), Math::DirectionToAngle(brain.GetFacingDirection()), 0.1f, sf::Color::Green),
        enemy(resources, {}, {}, 0.1f, sf::Color::Red);
    HealthBar playerHealthBar(player.GetBounds(), { 0.8f, 0.05f }, 10, sf::Color::Green),
        enemyHealthBar(enemy.GetBounds(), { 0.8f, 0.05f }, 10, sf::Color::Red);
    player.healthBar = &playerHealthBar;
    enemy.healthBar = &enemyHealthBar;
    Laser playerLaser({ window.getSize().x / 24.f, window.getSize().y * 2.f }, sf::Color::Green),
        enemyLaser({ window.getSize().x / 24.f, window.getSize().y * 2.f }, sf::Color::Red);
    player.laser = &playerLaser;
    enemy.laser = &enemyLaser;
    sf::Vector2f enemyDestination{}, enemyAimingDirection{};
    sf::Vector2f windowSize = static_cast<sf::Vector2f>(window.getSize());
    const sf::FloatRect map(windowSize * 0.1f, windowSize * 0.8f);
    sf::Clock frame;
    std::optional<sf::Clock> disconnectionTimer, laserTimer;
    Network::Messages::Initial initialReceive{};
    Network::Messages::Pause sentPauseMessage{}, receivedPauseMessage{};
    Network::Messages::Destination destinationToReceive{};
    Network::Messages::Aim aimToReceive{};
    Network::Messages::Health healthToReceive{};
    DuelOutcome outcome{ DuelOutcome::Tie };
    while (window.isOpen())
    {
        sf::Event gameEvent;
        while (window.pollEvent(gameEvent))
        {
            switch (gameEvent.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::MouseButtonPressed:
            case sf::Event::MouseButtonReleased:
            case sf::Event::MouseMoved:
                if (connection.established)
                    compile.ReactTo(gameEvent);
                break;
            }
        }
        if (disconnectionTimer.has_value() && disconnectionTimer.value().getElapsedTime().asSeconds() > 3)
            return DuelOutcome::OtherLostConnection;
        Network::Messages::Disconnection disconnectionMessage{};
        float deltaTime = frame.restart().asSeconds();          
        if (connection.established && compile.WasClicked())
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
        if (connection.Receive(initialReceive) == sf::Socket::Done && !connection.established)
        {
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
        else if (connection.Receive(destinationToReceive) == sf::Socket::Done)
        {
            enemyDestination = destinationToReceive.destination;
            logger.Log("Received destination " + std::to_string(enemyDestination.x) +
                " " + std::to_string(enemyDestination.y));
        }
        else if (connection.Receive(aimToReceive) == sf::Socket::Done)
        {
            enemyAimingDirection = aimToReceive.aimingDirection;
            logger.Log("Received aiming direction " + std::to_string(enemyAimingDirection.x) +
                " " + std::to_string(enemyAimingDirection.y));
            if (aimToReceive.willShoot)
                enemy.FakeShoot(Math::AngleToDirection(Math::NormalizeDegrees(enemy.GetRotation())));
        }
        else if (connection.Receive(healthToReceive) == sf::Socket::Done)
        {
            player.healthBar->FadeHealth(healthToReceive.health);
            logger.Log("Received health " + std::to_string(healthToReceive.health));
            if (healthToReceive.health == 0)
            {
                if (outcome != DuelOutcome::Won)
                    outcome = DuelOutcome::Lost;
                else
                    outcome = DuelOutcome::Tie;
            }
        } 
        else if (connection.Receive(disconnectionMessage) == sf::Socket::Done)
        {
            connection.established = false;
            message.setCharacterSize(24);
            message.setString("Other player got disconnected!");
            disconnectionTimer.emplace();
        }
        else if (connection.Receive(receivedPauseMessage) == sf::Socket::Done)
            logger.Log(receivedPauseMessage.paused ? "Received pause message" : "Received unpause message");
        else 
            connection.FlushReceivedMessages();
        if (!sentPauseMessage.paused && !receivedPauseMessage.paused)
        {
            if (!laserTimer.has_value())
                laserTimer.emplace();
            sf::Vector2f nextPosition = Math::ConstantIncrement(enemy.GetPosition(), enemyDestination, deltaTime * brain.GetMovementSpeed());
            enemy.SetPosition(nextPosition);
            float nextRotation = Math::LerpNormalizedAngle(Math::NormalizeDegrees(enemy.GetRotation()),
                Math::DirectionToAngle(enemyAimingDirection), deltaTime * brain.GetTurningSpeed());
            enemy.SetRotation(nextRotation);
            if (brain.TrySetNextDestination(map) && connection.Send(brain.GetDestination()) == sf::Socket::Done)
                logger.Log("Sent destination " + std::to_string(brain.GetDestination().destination.x) +
                    " " + std::to_string(brain.GetDestination().destination.y));
            if (brain.TryAim(enemy))
            {
                Network::Messages::Aim aimToSend
                {
                    .aimingDirection = brain.GetAimingDirection(),
                    .willShoot = laserTimer->getElapsedTime().asSeconds() > 1
                };
                if (connection.Send(aimToSend) == sf::Socket::Done)
                {
                    logger.Log("Sent aiming direction " + std::to_string(aimToSend.aimingDirection.x) +
                        " " + std::to_string(aimToSend.aimingDirection.y));
                    if (aimToSend.willShoot)
                    {
                        laserTimer->restart();
                        ShootContext context(player.Shoot(brain.GetFacingDirection(), enemy));
                        Network::Messages::Health healthToSend
                        {
                            .health = context.health
                        };
                        if (context.hit && connection.Send(healthToSend) == sf::Socket::Done)
                        {
                            logger.Log("Sent health " + std::to_string(context.health));
                            if (context.health == 0)
                            {
                                if (outcome != DuelOutcome::Lost)
                                    outcome = DuelOutcome::Won;
                                else
                                    outcome = DuelOutcome::Tie;
                            }
                        }
                    }
                }
            }
            if (!brain.codeDebug.getString().isEmpty())
            {
                sentPauseMessage.paused = true;
                if (connection.Send(sentPauseMessage) == sf::Socket::Done)
                    logger.Log("Sent pause message");
            }
            nextPosition = Math::ConstantIncrement(brain.GetPosition(), brain.GetDestination().destination, deltaTime * brain.GetMovementSpeed());
            player.SetPosition(nextPosition);
            brain.SetPosition(nextPosition);
            nextRotation = Math::LerpNormalizedAngle(Math::DirectionToAngle(brain.GetFacingDirection()), 
                Math::DirectionToAngle(brain.GetAimingDirection()), deltaTime * brain.GetTurningSpeed());
            player.SetRotation(nextRotation);
            brain.SetFacingDirection(Math::AngleToDirection(nextRotation));  
            player.Update(deltaTime); 
            enemy.Update(deltaTime);
            if (player.healthBar->GetHealth() == 0 || enemy.healthBar->GetHealth() == 0)
                return outcome;
        }       
        window.clear();
        player.Draw(window);
        if (connection.established)
            enemy.Draw(window);
        window.draw(message);
        window.draw(brain.codeDebug);
        if (connection.established)
            compile.Draw(window);
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
    case DuelOutcome::Tie:
        message.setString("It's a tie! Want to play again?(y/n)");
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
    Resources resources;
    auto arrialPath(FontDirectory"arial.ttf");
    auto arrial = resources.fonts.try_emplace(arrialPath);
    arrial.first->second.loadFromFile(arrialPath);
    sf::Text message("Do you want to be a host or a client?", arrial.first->second, 24);
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
                DuelOutcome outcome(Duel(resources, connection.value(), brain.value(), window, message));
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
