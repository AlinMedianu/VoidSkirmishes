#ifndef NETWORKCONNECTION
#define NETWORKCONNECTION

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include "NetworkMessages.h"

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

	template<Message M>
	sf::Socket::Status Connection::Receive(M& message)
	{
		sf::IpAddress newAddress{ sf::IpAddress::None };
		sf::Uint16 newPort{};
		sf::Socket::Status hasReceived{ socket.receive(received, newAddress, newPort) };
		if (hasReceived == sf::Socket::Done)
		{
			otherAddress = newAddress;
			otherPort = newPort;
			received >> message;
			bool correctMessage{ received.endOfPacket() };
			received.clear();
			return correctMessage ? sf::Socket::Done : sf::Socket::Error;
		}
		return hasReceived;
	}
}
#endif // !NETWORKCONNECTION
