#include "NetworkConnection.h"
namespace Network
{
	Connection::Connection(sf::Text& messageBoard) : messageBoard(messageBoard), socket(), addressAndPort{}, 
		otherAddress(), otherPort{}, sent(), received(), established{}
	{
		socket.setBlocking(false);
		sf::Socket::Status status = socket.bind(sf::Socket::AnyPort);
		if (status != sf::Socket::Done)
		{
			LogSocketStatus("Bind failed with the status: ", status);
			return;
		}
		addressAndPort = "Local address: " + sf::IpAddress::getLocalAddress().toString() +
			"\nPort: " + std::to_string(socket.getLocalPort());
		messageBoard.setString("Local address: " + sf::IpAddress::getLocalAddress().toString() +
			"\nHost is listening to port " + std::to_string(socket.getLocalPort()) +
			"\nPress the adjacent button" + 
			"\nand paste the formatted string with your" +
			"\naddress and port somewhere" + 
			"\nwhere the client can access it."
			",\nwaiting for connections... ");
	}

	Connection::Connection(const std::string& address, sf::Uint16 port, sf::Text& messageBoard)
		: messageBoard(messageBoard), socket(), addressAndPort{}, 
		otherAddress(address), otherPort{ port }, sent(), received(), established{}
	{
		socket.setBlocking(false);
		messageBoard.setString("Connecting to host... ");
	}

	const sf::String& Connection::AddressAndPort() const noexcept
	{
		return addressAndPort;
	}

	void Connection::FlushReceivedMessages()
	{
		received.clear();
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
