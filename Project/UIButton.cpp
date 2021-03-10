#include "UIButton.h"
namespace UI
{
	Button::Button(Resources& resources, sf::Vector2f position, sf::Vector2f scale, std::string&& message)
		: clicked{ false }, current { State::Default }, textures{}, body(), message()
	{
		std::array<std::string, static_cast<size_t>(State::COUNT)> texturePaths
		{ 
			SpriteDirectory"Button Default.png", 
			SpriteDirectory"Button Down.png", 
			SpriteDirectory"Button Hover.png",
			SpriteDirectory"Button Inactive.png" 
		};
		for (size_t state = 0; state < static_cast<size_t>(State::COUNT); ++state)
		{
			auto texture = resources.textures.try_emplace(texturePaths[state]);
			if (texture.second)
				texture.first->second.loadFromFile(texturePaths[state]);
			textures[state] = &texture.first->second;
		}
		body.setTexture(*textures[static_cast<size_t>(current)]);
		body.setPosition(position);
		body.setOrigin(body.getLocalBounds().left + body.getLocalBounds().width / 2.f,
			body.getLocalBounds().top + body.getLocalBounds().height / 2.f);
		body.setScale(scale);
		auto arrialPath(FontDirectory"arial.ttf");
		auto arrial = resources.fonts.try_emplace(arrialPath);
		if (arrial.second)
			arrial.first->second.loadFromFile(arrialPath);
		this->message.setString(message);
		this->message.setFont(arrial.first->second);
		this->message.setCharacterSize(15);
		this->message.setStyle(sf::Text::Bold);
		this->message.setPosition(position);
		this->message.setOrigin(this->message.getLocalBounds().left + this->message.getLocalBounds().width / 2.f,
			this->message.getLocalBounds().top + this->message.getLocalBounds().height / 2.f);
	}

	void Button::ReactTo(const sf::Event& mouseEvent)
	{
		clicked = false;
		if (current == State::Inactive)
			return;
		switch (mouseEvent.type)
		{
		case sf::Event::MouseButtonPressed:
			if (body.getGlobalBounds().contains(mouseEvent.mouseButton.x, mouseEvent.mouseButton.y))
			{
				current = State::Down;
				UpdateTexture();
			}
			break;
		case sf::Event::MouseButtonReleased:
			if (current == State::Down)
			{
				if (body.getGlobalBounds().contains(mouseEvent.mouseButton.x, mouseEvent.mouseButton.y))
				{
					current = State::Hover;
					clicked = true;
				}
				else
					current = State::Default;
				UpdateTexture();
			}
			break;
		case sf::Event::MouseMoved:
			if (current != State::Down)
			{
				if (body.getGlobalBounds().contains(mouseEvent.mouseMove.x, mouseEvent.mouseMove.y))
					current = State::Hover;
				else
					current = State::Default;
				UpdateTexture();
			}
			break;
		}
	}

	void Button::Activate()
	{
		if (current == State::Inactive)
		{
			current = State::Default;
			UpdateTexture();
		}
	}

	void Button::Deactivate()
	{
		current = State::Inactive;
		UpdateTexture();
	}

	bool Button::WasClicked() const noexcept
	{
		return clicked;
	}

	Button::State Button::GetCurrentState() const noexcept
	{
		return current;
	}

	void Button::Draw(sf::RenderWindow& on)
	{
		on.draw(body);
		on.draw(message);
	}

	void Button::UpdateTexture()
	{
		body.setTexture(*textures[static_cast<size_t>(current)]);
	}
}
