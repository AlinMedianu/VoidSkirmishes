#include "Server.h"

Server::Server(sf::Text& messageBoard) : messageBoard(messageBoard), socket(), clientAddress(), clientPort{}
{
    socket.setBlocking(false);
    sf::Socket::Status status = socket.bind(sf::Socket::AnyPort);
    if (status != sf::Socket::Done)
    {
        LogSocketStatus("Bind failed with the status: ", status);
        return;
    }
    messageBoard.setString("Local address: " + sf::IpAddress::getLocalAddress().toString() +
        "\nServer is listening to port " + std::to_string(socket.getLocalPort()) +
        ",\nwaiting for connections... ");
}

sf::Socket::Status Server::Send(sf::Packet& packet)
{    
    return socket.send(packet, clientAddress, clientPort);
}

sf::Socket::Status Server::Receive(sf::Packet& packet)
{
    if (clientPort == 0)
        return socket.receive(packet, clientAddress, clientPort);
    return sf::Socket::NotReady;
}

void Server::SocketStatusToString(sf::String& string, sf::Socket::Status status)
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

void Server::LogSocketStatus(const std::string& message, sf::Socket::Status status)
{
    sf::String string(message);
    SocketStatusToString(string, status);
    messageBoard.setString(string);
}
