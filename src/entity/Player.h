#pragma once

#include <SFML/System.hpp>

namespace entity
{
struct Player
{
    int id = -1;
    int team = 0; // 0 = blue, 1 = red
    sf::Vector2f position {0.f, 0.f};
    sf::Vector2f velocity {0.f, 0.f};
    sf::Vector2f accel {0.f, 0.f};
    float radius = 16.f;
    float maxSpeed = 300.f; // units per second
    // linear speed decay (units per second). velocity magnitude will be reduced by
    // this amount each second until zero. Adjust to tune feel.
    float linearFriction = 300.f;
};

} // namespace entity
