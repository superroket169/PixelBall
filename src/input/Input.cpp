#include "Input.h"
#include "../entity/Player.h"
#include <cmath>

namespace input
{
InputHandler::InputHandler()
    : m_up(false)
    , m_down(false)
    , m_left(false)
    , m_right(false)
    , m_kick(false)
    , m_accelMagnitude(600.f)
{
}

InputHandler::~InputHandler()
{
}

void InputHandler::processEvent(const sf::Event &ev)
{
    if (ev.type == sf::Event::KeyPressed)
    {
        if (ev.key.code == sf::Keyboard::W) m_up = true;
        if (ev.key.code == sf::Keyboard::S) m_down = true;
        if (ev.key.code == sf::Keyboard::A) m_left = true;
        if (ev.key.code == sf::Keyboard::D) m_right = true;
    }
    else if (ev.type == sf::Event::KeyReleased)
    {
        if (ev.key.code == sf::Keyboard::W) m_up = false;
        if (ev.key.code == sf::Keyboard::S) m_down = false;
        if (ev.key.code == sf::Keyboard::A) m_left = false;
        if (ev.key.code == sf::Keyboard::D) m_right = false;
    }
}

void InputHandler::applyTo(entity::Player &p)
{
    sf::Vector2f a {0.f, 0.f};
    if (m_up) a.y -= 1.f;
    if (m_down) a.y += 1.f;
    if (m_left) a.x -= 1.f;
    if (m_right) a.x += 1.f;

    // normalize
    float len = std::sqrt(a.x * a.x + a.y * a.y);
    if (len > 1e-6f)
    {
        a.x /= len;
        a.y /= len;
        a *= m_accelMagnitude;
    }

    p.accel = a;
}

void InputHandler::pollRealTime()
{
    m_up = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
    m_down = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
    m_left = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    m_right = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    // kick remains event-driven; for convenience treat Space as real-time too
    m_kick = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
}

} // namespace input
