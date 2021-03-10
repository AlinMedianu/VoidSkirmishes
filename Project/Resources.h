#ifndef RESOURCES
#define RESOURCES

#include <unordered_map>
#include <filesystem>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>

struct Resources
{
	std::unordered_map<std::filesystem::path, sf::Texture, std::hash<std::wstring>> textures;
	std::unordered_map<std::filesystem::path, sf::Font, std::hash<std::wstring>> fonts;
};

#endif // !RESOURCES