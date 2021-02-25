#include "NetworkConnection.h"
namespace Network
{
	Connection::Connection(sf::Text& messageBoard) : messageBoard(messageBoard), socket(), otherAddress(), otherPort{},
		sent(), received(), established{}, host{ true }
	{
		socket.setBlocking(false);
		sf::Socket::Status status = socket.bind(sf::Socket::AnyPort);
		if (status != sf::Socket::Done)
		{
			LogSocketStatus("Bind failed with the status: ", status);
			return;
		}
		messageBoard.setString("Local address: " + sf::IpAddress::getLocalAddress().toString() +
			"\nHost is listening to port " + std::to_string(socket.getLocalPort()) +
			",\nwaiting for connections... ");
	}

	Connection::Connection(const sf::String& address, const sf::String& port, sf::Text& messageBoard)
		: messageBoard(messageBoard), socket(), otherAddress(address.isEmpty() ?
			sf::IpAddress::LocalHost : sf::IpAddress(address)), otherPort{ static_cast<sf::Uint16>(std::stoi(port.toAnsiString())) },
		sent(), received(), established{}, host{ false }
	{
		socket.setBlocking(false);
		messageBoard.setString("Connecting to host... ");
	}

	void Connection::LogSocketStatus(const std::string& message, sf::Socket::Status status)
	{
		sf::String string(message);
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
		messageBoard.setString(string);
	}
}
