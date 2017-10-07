#include "SfmlSprite.h"

SfmlSprite::SfmlSprite(const std::string &imageFileName)
{
	_texture.loadFromFile(imageFileName);
	sprite.setTexture(_texture);
}

int SfmlSprite::GetWidth() const
{
	return sprite.getTextureRect().width;
}

int SfmlSprite::GetHeight()
{
	return sprite.getTextureRect().height;
}
