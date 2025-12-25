#pragma once

#include <vector>
#include <memory>
#include <atomic>
#include <functional>
#include <SFML/System.hpp>

namespace entity
{
    struct Player;
    struct Ball;
}

namespace physics
{
    class Physics;
}

namespace core
{
enum class Mode
{
    Server,
    Client,
    Local
};

class Game
{
public:
    explicit Game(Mode mode = Mode::Local, unsigned int ticksPerSecond = 60);
    ~Game();

    void start();
    void stop();

    // single-step update (called by external loop if desired)
    void update(float dt);

    // Accessors for entities
    std::vector<std::shared_ptr<entity::Player>>& players();
    std::shared_ptr<entity::Ball> ball();

    // Callbacks
    // invoked when a goal happens: args (teamIdScored)
    std::function<void(int)> onGoal;

private:
    void runLoop();

    Mode m_mode;
    unsigned int m_tps;
    std::atomic<bool> m_running;
    std::thread m_thread;

    std::unique_ptr<physics::Physics> m_physics;

    std::vector<std::shared_ptr<entity::Player>> m_players;
    std::shared_ptr<entity::Ball> m_ball;

    // field size (world units)
    float m_width = 1280.f;
    float m_height = 720.f;
};

} // namespace core
