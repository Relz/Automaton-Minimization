#ifndef PROJECT_SFMLSPRITE_H
#define PROJECT_SFMLSPRITE_H

#include <SFML/Graphics.hpp>

class SfmlSprite
{
public:
	explicit SfmlSprite(const std::string & imageFileName);

	int GetWidth() const;

	sf::Sprite sprite;

	int GetHeight();

private:
	sf::Texture _texture;
};

#endif //PROJECT_SFMLSPRITE_H
