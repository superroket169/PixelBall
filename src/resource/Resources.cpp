#include "Resources.h"
#include <iostream>

namespace resource
{
Resources::Resources()
{
}

Resources::~Resources()
{
}

sf::Texture& Resources::loadTexture(const std::string &path)
{
    auto it = m_textures.find(path);
    if (it != m_textures.end())
    {
        return *it->second;
    }

    auto tex = std::make_unique<sf::Texture>();
    if (!tex->loadFromFile(path))
    {
        std::cerr << "Failed to load texture: " << path << "\n";
        // create a small placeholder
        tex->create(32, 32);
    }

    sf::Texture& ref = *tex;
    m_textures[path] = std::move(tex);
    return ref;
}

sf::Texture& Resources::getTexture(const std::string &path)
{
    return loadTexture(path);
}

} // namespace resource
