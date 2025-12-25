#pragma once

#include <SFML/System.hpp>

namespace entity
{
struct Ball
{
    int id = 0;
    sf::Vector2f position {0.f, 0.f};
    sf::Vector2f velocity {0.f, 0.f};
    float radius = 12.f;
    float friction = 0.98f; // per second multiplier (applied per-step)
};

} // namespace entity
