#ifndef SERVER
#define SERVER
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
class Server
{
public:
	Server(sf::Text& messageBoard);
	sf::Socket::Status Send(sf::Packet& packet);
	sf::Socket::Status Receive(sf::Packet& packet);
private:
	void SocketStatusToString(sf::String& string, sf::Socket::Status status);
	void LogSocketStatus(const std::string& message, sf::Socket::Status status);
	sf::Text& messageBoard;
	sf::UdpSocket socket;
	sf::IpAddress clientAddress;
	sf::Uint16 clientPort;
};
#endif // !SERVER
