#ifndef CLIENT
#define CLIENT
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
class Client
{
public:
	Client(const sf::String& address, const sf::String& port, sf::Text& messageBoard);
	sf::Socket::Status Receive(sf::Packet& packet);
private:
	void SocketStatusToString(sf::String& string, sf::Socket::Status status);
	void LogSocketStatus(const std::string& message, sf::Socket::Status status);
	sf::Text& messageBoard;
	sf::UdpSocket socket;
};
#endif // !UDPCLIENT