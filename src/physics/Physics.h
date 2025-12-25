#pragma once

#include <vector>
#include <memory>
#include <SFML/System.hpp>

namespace entity
{
    struct Player;
    struct Ball;
}

namespace physics
{
class Physics
{
public:
    Physics(float width, float height);
    ~Physics();

    // step simulation for dt seconds
    void step(std::vector<std::shared_ptr<entity::Player>>& players,
              std::shared_ptr<entity::Ball> ball,
              float dt);

    // set goal rectangles (left and right)
    void setGoals(const sf::FloatRect& leftGoal, const sf::FloatRect& rightGoal);

    // returns -1 if no goal, otherwise team id that scored
    int pollGoal();

private:
    float m_width;
    float m_height;
    sf::FloatRect m_leftGoal;
    sf::FloatRect m_rightGoal;
    int m_lastGoal;
};

} // namespace physics
