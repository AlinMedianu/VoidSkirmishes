#include "Client.h"

Client::Client(const sf::String& address, const sf::String& port, sf::Text& messageBoard) : messageBoard(messageBoard), socket()
{
    socket.setBlocking(false);
    sf::Packet p;
    sf::Socket::Status status = socket.send(p, address.isEmpty() ? 
        sf::IpAddress::LocalHost : sf::IpAddress(address), std::stoi(port.toAnsiString()));
    if (status != sf::Socket::Done)
    {
        LogSocketStatus("Send failed with the status: ", status);
        return;
    }
}

sf::Socket::Status Client::Receive(sf::Packet& packet)
{
    sf::IpAddress server;
    sf::Uint16 port;
    return socket.receive(packet, server, port);
}

void Client::SocketStatusToString(sf::String& string, sf::Socket::Status status)
{
    switch (status)
    {
    case sf::Socket::Done:
        string += "Done";
        break;
    case sf::Socket::NotReady:
        string += "NotReady";
        break;
    case sf::Socket::Partial:
        string += "Partial";
        break;
    case sf::Socket::Disconnected:
        string += "Disconnected";
        break;
    case sf::Socket::Error:
        string += "Error";
        break;
    }
}

void Client::LogSocketStatus(const std::string& message, sf::Socket::Status status)
{
    sf::String string(message);
    SocketStatusToString(string, status);
    messageBoard.setString(string);
}
