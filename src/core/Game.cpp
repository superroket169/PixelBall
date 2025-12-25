#include "Game.h"
#include "../physics/Physics.h"
#include "../entity/Player.h"
#include "../entity/Ball.h"
#include <thread>
#include <chrono>

namespace core
{
Game::Game(Mode mode, unsigned int ticksPerSecond)
    : m_mode(mode)
    , m_tps(ticksPerSecond)
    , m_running(false)
{
    m_physics = std::make_unique<physics::Physics>(m_width, m_height);
    // create a default ball
    m_ball = std::make_shared<entity::Ball>();
    m_ball->position = sf::Vector2f(m_width * 0.5f, m_height * 0.5f);
}

Game::~Game()
{
    stop();
}

void Game::start()
{
    if (m_running)
    {
        return;
    }

    m_running = true;
    m_thread = std::thread(&Game::runLoop, this);
}

void Game::stop()
{
    if (!m_running)
    {
        return;
    }

    m_running = false;
    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

void Game::runLoop()
{
    using clock = std::chrono::steady_clock;
    auto dt = std::chrono::duration<float>(1.0f / static_cast<float>(m_tps));
    while (m_running)
    {
        auto start = clock::now();

        update(dt.count());

        int goal = m_physics->pollGoal();
        if (goal != -1 && onGoal)
        {
            onGoal(goal);
        }

        auto elapsed = clock::now() - start;
        if (elapsed < dt)
        {
            std::this_thread::sleep_for(dt - elapsed);
        }
    }
}

void Game::update(float dt)
{
    if (m_physics)
    {
        m_physics->step(m_players, m_ball, dt);
    }
}

std::vector<std::shared_ptr<entity::Player>>& Game::players()
{
    return m_players;
}

std::shared_ptr<entity::Ball> Game::ball()
{
    return m_ball;
}

} // namespace core
