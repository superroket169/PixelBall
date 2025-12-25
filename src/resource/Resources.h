#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include <unordered_map>

namespace resource
{
class Resources
{
public:
    Resources();
    ~Resources();

    sf::Texture& loadTexture(const std::string &path);
    sf::Texture& getTexture(const std::string &path);

private:
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> m_textures;
};

} // namespace resource
