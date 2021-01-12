#ifndef NETWORKMESSAGES
#define NETWORKMESSAGES

#include <SFML/System/Vector2.hpp>
#include <SFML/Network/Packet.hpp>

namespace Network
{
	template<typename T>
	concept Message = requires(T&& message, sf::Packet& packet)
	{
		{ message.id };
		{ packet << message } -> std::same_as<sf::Packet&>;
		{ packet >> message } -> std::same_as<sf::Packet&>;
	};

	struct Destination
	{
		sf::Vector2f destination;
		const sf::Uint8 id{};
		friend sf::Packet& operator<< (sf::Packet& packet, const Destination& self)
		{
			return packet << self.id << self.destination.x << self.destination.y;
		}
		friend sf::Packet& operator>> (sf::Packet& packet, Destination& self)
		{
			sf::Uint8 id{};
			if (!packet.endOfPacket() && reinterpret_cast<const sf::Uint8*>(packet.getData())[0] == self.id)
				return packet >> id >> self.destination.x >> self.destination.y;
			return packet;
		}
	};

	struct PositionDestination
	{
		sf::Vector2f position, destination;
		const sf::Uint8 id{ 1 };
		friend sf::Packet& operator<< (sf::Packet& packet, const PositionDestination& self)
		{
			return packet << self.id << self.position.x << self.position.y << 
				self.destination.x << self.destination.y;
		}
		friend sf::Packet& operator>> (sf::Packet& packet, PositionDestination& self)
		{
			sf::Uint8 id{};
			if (!packet.endOfPacket() && reinterpret_cast<const sf::Uint8*>(packet.getData())[0] == self.id)
				return packet >> id >> self.position.x >> self.position.y >>
					self.destination.x >> self.destination.y;
			return packet;
		}
	};
}

#endif // !NETWORKMESSAGES
