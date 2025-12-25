#include "Physics.h"
#include "../entity/Player.h"
#include "../entity/Ball.h"
#include <cmath>

namespace physics
{
Physics::Physics(float width, float height)
    : m_width(width)
    , m_height(height)
    , m_lastGoal(-1)
{
    // default small goals centered vertically
    float goalW = 40.f;
    float goalH = 200.f;
    m_leftGoal = sf::FloatRect(0.f - 1.f, (m_height - goalH) * 0.5f, goalW, goalH);
    m_rightGoal = sf::FloatRect(m_width - goalW + 1.f, (m_height - goalH) * 0.5f, goalW, goalH);
}

Physics::~Physics()
{
}

void Physics::setGoals(const sf::FloatRect& leftGoal, const sf::FloatRect& rightGoal)
{
    m_leftGoal = leftGoal;
    m_rightGoal = rightGoal;
}

int Physics::pollGoal()
{
    int g = m_lastGoal;
    m_lastGoal = -1;
    return g;
}

static float length(const sf::Vector2f& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

static sf::Vector2f normalize(const sf::Vector2f& v)
{
    float L = length(v);
    if (L <= 1e-6f)
    {
        return sf::Vector2f(0.f, 0.f);
    }
    return sf::Vector2f(v.x / L, v.y / L);
}

void Physics::step(std::vector<std::shared_ptr<entity::Player>>& players,
                   std::shared_ptr<entity::Ball> ball,
                   float dt)
{
    // integrate player velocities
    for (auto &p : players)
    {
        // apply accel
        p->velocity += p->accel * dt;
        // clamp speed
        float vlen = length(p->velocity);
        if (vlen > p->maxSpeed)
        {
            p->velocity = normalize(p->velocity) * p->maxSpeed;
        }

        // apply player linear friction: reduce speed by constant amount per second
        float speed = length(p->velocity);
        if (speed > 1e-6f)
        {
            float newSpeed = speed - p->linearFriction * dt;
            if (newSpeed < 0.f) newSpeed = 0.f;
            p->velocity = normalize(p->velocity) * newSpeed;
        }

        p->position += p->velocity * dt;

        // clamp to field
        if (p->position.x < p->radius) p->position.x = p->radius;
        if (p->position.y < p->radius) p->position.y = p->radius;
        if (p->position.x > m_width - p->radius) p->position.x = m_width - p->radius;
        if (p->position.y > m_height - p->radius) p->position.y = m_height - p->radius;
    }

    if (!ball)
    {
        return;
    }

    // integrate ball
    ball->velocity *= std::pow(ball->friction, dt);
    ball->position += ball->velocity * dt;

    // ball bounds
    if (ball->position.x < ball->radius)
    {
        ball->position.x = ball->radius;
        ball->velocity.x = -ball->velocity.x * 0.6f;
    }
    if (ball->position.y < ball->radius)
    {
        ball->position.y = ball->radius;
        ball->velocity.y = -ball->velocity.y * 0.6f;
    }
    if (ball->position.x > m_width - ball->radius)
    {
        ball->position.x = m_width - ball->radius;
        ball->velocity.x = -ball->velocity.x * 0.6f;
    }
    if (ball->position.y > m_height - ball->radius)
    {
        ball->position.y = m_height - ball->radius;
        ball->velocity.y = -ball->velocity.y * 0.6f;
    }

    // player-ball collisions (very simple impulse)
    for (auto &p : players)
    {
        sf::Vector2f d = ball->position - p->position;
        float dist = length(d);
        float minDist = ball->radius + p->radius;
        if (dist < 1e-6f)
        {
            // push slightly
            d = sf::Vector2f(1.f, 0.f);
            dist = 1.f;
        }

        if (dist < minDist)
        {
            // simple separation
            float overlap = (minDist - dist);
            sf::Vector2f n = normalize(d);
            ball->position += n * overlap * 0.5f;
            p->position -= n * overlap * 0.5f;

            // relative velocity
            sf::Vector2f rv = ball->velocity - p->velocity;
            float rel = rv.x * n.x + rv.y * n.y;
            if (rel < 0.f)
            {
                float e = 0.8f; // restitution
                float j = -(1.f + e) * rel;
                // mass assumed 1 for both for simplicity
                ball->velocity += n * j * 0.5f;
                p->velocity -= n * j * 0.5f;
            }

            // no kick behavior (space removed)
        }
    }

    // detect goals
    if (m_leftGoal.contains(ball->position))
    {
        // right team scored (team 1)
        m_lastGoal = 1;
        // reset ball center
        ball->position = sf::Vector2f(m_width * 0.5f, m_height * 0.5f);
        ball->velocity = sf::Vector2f(0.f, 0.f);
    }
    else if (m_rightGoal.contains(ball->position))
    {
        // left team scored (team 0)
        m_lastGoal = 0;
        ball->position = sf::Vector2f(m_width * 0.5f, m_height * 0.5f);
        ball->velocity = sf::Vector2f(0.f, 0.f);
    }
}

} // namespace physics
