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
		sf::String addressAndPort;
		sf::IpAddress otherAddress;
		sf::Uint16 otherPort;
		sf::Packet sent, received;
		void LogSocketStatus(const std::string& message, sf::Socket::Status status);
		template<Message M>
		sf::Socket::Status Process(M& message);
	public:
		bool established;
		Connection(sf::Text& messageBoard);
		Connection(const std::string& address, sf::Uint16 port, sf::Text& messageBoard);
		[[nodiscard]] const sf::String& AddressAndPort() const noexcept;
		template<Message M>
		sf::Socket::Status Send(const M& message);
		template<Message M>
		sf::Socket::Status Receive(M& message);
		void FlushReceivedMessages();
	};

	template<Message M>
	sf::Socket::Status Connection::Process(M& message)
	{
		received >> message;
		if (received.endOfPacket())
		{
			received.clear();
			return sf::Socket::Done;
		}
		return sf::Socket::Error;
	}

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
		if (!received.endOfPacket())
			return Process(message);
		sf::IpAddress newAddress{ sf::IpAddress::None };
		sf::Uint16 newPort{};
		sf::Socket::Status hasReceived{};
		if ((hasReceived = socket.receive(received, newAddress, newPort)) == sf::Socket::Done)
		{
			otherAddress = newAddress;
			otherPort = newPort;
			return Process(message);
		}
		return hasReceived;
	}

	[[nodiscard]] inline bool TryGetAddressAndPort(const sf::String& string, std::string& address, sf::Uint16& port)
	{
		size_t newLinePosition = string.find("\nPort: ");
		if (string.find("Local address: ") > 0 ||
			newLinePosition == sf::String::InvalidPos)
			return false;
		address = std::string{ string.begin() + std::string{ "Local address: " }.length(), string.begin() + newLinePosition };
		std::string otherPortString{ string.begin() + newLinePosition + std::string{ "Port: " }.length(), string.end() };
		port = static_cast<sf::Uint16>(std::stoi(otherPortString));
		return true;
	}
}
#endif // !NETWORKCONNECTION
