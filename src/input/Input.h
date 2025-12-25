#pragma once

#include <SFML/Window.hpp>
#include <memory>

namespace entity
{
    struct Player;
}

namespace input
{
class InputHandler
{
public:
    InputHandler();
    ~InputHandler();

    // process an event (key presses/releases)
    void processEvent(const sf::Event &ev);

    // sample real-time keyboard state (for key-hold)
    void pollRealTime();

    // apply current input state to a player (set accel and wantsKick)
    void applyTo(entity::Player &p);

private:
    bool m_up;
    bool m_down;
    bool m_left;
    bool m_right;
    bool m_kick;
    float m_accelMagnitude;
};

} // namespace input
