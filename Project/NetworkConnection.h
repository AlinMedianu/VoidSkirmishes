#ifndef NETWORKCONNECTION
#define NETWORKCONNECTION

#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Graphics/Text.hpp>
#include "NetworkMessages.h"
#include <cassert>

namespace Network
{
	class Connection
	{
		sf::Text& messageBoard;
		sf::UdpSocket socket;
		sf::IpAddress otherAddress;
		sf::Uint16 otherPort;
		sf::Packet sent, received;
		void LogSocketStatus(const std::string& message, sf::Socket::Status status);
	public:
		//TODO: replace server bool with RemoveRedundantMessages function
		bool server;
		bool established;
		Connection(sf::Text& messageBoard);
		Connection(const sf::String& address, const sf::String& port, sf::Text& messageBoard);
		template<Message M>
		sf::Socket::Status Send(const M& message);
		template<Message M>
		sf::Socket::Status Receive(M& message);
	};

	template<Message M>
	sf::Socket::Status Connection::Send(const M& message)
	{
		sent.clear();
		sent << message;
		return socket.send(sent, otherAddress, otherPort);
	}

	//TODO: refactor message receiving
	template<Message M>
	sf::Socket::Status Connection::Receive(M& message)
	{
		sf::IpAddress newAddress{ sf::IpAddress::None };
		sf::Uint16 newPort{};
		bool cleared = received.endOfPacket();
		sf::Socket::Status hasReceived{ sf::Socket::Done };
		if (cleared)
			hasReceived = socket.receive(received, newAddress, newPort);
		if (hasReceived == sf::Socket::Done)
		{
			if (cleared)
			{
				otherAddress = newAddress;
				otherPort = newPort;
			}
			received >> message;
			if (received.endOfPacket())
			{
				received.clear();
				hasReceived = sf::Socket::Done;
			}
			else
				hasReceived =  sf::Socket::Error;
		}
		return hasReceived;
	}
}
#endif // !NETWORKCONNECTION
