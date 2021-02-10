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

	namespace Messages
	{
		inline sf::Packet& operator<< (sf::Packet& packet, sf::Vector2f self)
		{
			return packet << self.x << self.y;
		}

		inline sf::Packet& operator>> (sf::Packet& packet, sf::Vector2f& self)
		{
			return packet >> self.x >> self.y;
		}

		struct Initial
		{
			const sf::Uint8 id{ 0 };
			sf::Vector2f position, destination, facingDirection, aimingDirection;
			friend sf::Packet& operator<< (sf::Packet& packet, const Initial& self)
			{
				return packet << self.id << self.position << self.destination << self.facingDirection << self.aimingDirection;
			}
			friend sf::Packet& operator>> (sf::Packet& packet, Initial& self)
			{
				sf::Uint8 id{};
				if (!packet.endOfPacket() && reinterpret_cast<const sf::Uint8*>(packet.getData())[0] == self.id)
					return packet >> id >> self.position >> self.destination >> self.facingDirection >> self.aimingDirection;
				return packet;
			}
		};

		struct Destination
		{
			const sf::Uint8 id{ 1 };
			sf::Vector2f destination;
			friend sf::Packet& operator<< (sf::Packet& packet, const Destination& self)
			{
				return packet << self.id << self.destination;
			}
			friend sf::Packet& operator>> (sf::Packet& packet, Destination& self)
			{
				sf::Uint8 id{};
				if (!packet.endOfPacket())
				{
					sf::Uint8 packetID = reinterpret_cast<const sf::Uint8*>(packet.getData())[0];
					if(packetID == self.id)
						return packet >> id >> self.destination;
				}
				return packet;
			}
		};

		struct AimingDirection
		{
			const sf::Uint8 id{ 2 };
			sf::Vector2f aimingDirection;
			friend sf::Packet& operator<< (sf::Packet& packet, const AimingDirection& self)
			{
				return packet << self.id << self.aimingDirection;
			}
			friend sf::Packet& operator>> (sf::Packet& packet, AimingDirection& self)
			{
				sf::Uint8 id{};
				if (!packet.endOfPacket() && reinterpret_cast<const sf::Uint8*>(packet.getData())[0] == self.id)
					return packet >> id >> self.aimingDirection;
				return packet;
			}
		};

		struct EnemyHealth
		{
			const sf::Uint8 id{ 3 };
			sf::Int32 health;
			friend sf::Packet& operator<< (sf::Packet& packet, const EnemyHealth& self)
			{
				return packet << self.id << self.health;
			}
			friend sf::Packet& operator>> (sf::Packet& packet, EnemyHealth& self)
			{
				sf::Uint8 id{};
				if (!packet.endOfPacket() && reinterpret_cast<const sf::Uint8*>(packet.getData())[0] == self.id)
					return packet >> id >> self.health;
				return packet;
			}
		};
	}
}

#endif // !NETWORKMESSAGES
