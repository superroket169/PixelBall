#include "Renderer.h"
#include "../resource/Resources.h"
#include <iostream>
#include <filesystem>

namespace render
{
Renderer::Renderer(unsigned int width, unsigned int height, resource::Resources &res)
    : m_width(width)
    , m_height(height)
    , m_res(res)
{
    m_window = std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), "PixelBall Local", sf::Style::Close);
    m_window->setFramerateLimit(60);

    // preferred absolute assets directory (user-provided)
    const std::string absAssets = "../assets/";

    // load textures (try absolute first, then relative)
    std::string fieldPath = absAssets + "field.jpeg";
    if (!std::filesystem::exists(fieldPath)) fieldPath = "assets/field.jpeg";
    sf::Texture &fieldTex = m_res.getTexture(fieldPath);
    m_fieldSprite.setTexture(fieldTex);
    // scale field to window
    sf::Vector2f fs = sf::Vector2f((float)width / fieldTex.getSize().x, (float)height / fieldTex.getSize().y);
    m_fieldSprite.setScale(fs);

    std::string ballPath = absAssets + "ball.jpeg";
    if (!std::filesystem::exists(ballPath)) ballPath = "assets/ball.jpeg";
    sf::Texture &ballTex = m_res.getTexture(ballPath);
    m_ballSprite.setTexture(ballTex);
    m_ballSprite.setOrigin(ballTex.getSize().x * 0.5f, ballTex.getSize().y * 0.5f);

    std::string fontPath = absAssets + "DejaVuSans-Bold.ttf";
    if (!std::filesystem::exists(fontPath)) fontPath = "assets/DejaVuSans-Bold.ttf";
    if (!m_font.loadFromFile(fontPath))
    {
        // no font, will use default SFML behavior (may fail).
        std::cerr << "Failed to load font " << fontPath << std::endl;
    }
}

Renderer::~Renderer()
{
    if (m_window && m_window->isOpen())
    {
        m_window->close();
    }
}

void Renderer::beginFrame()
{
    // process events externally
}

void Renderer::draw(const std::vector<std::shared_ptr<entity::Player>> &players, const std::shared_ptr<entity::Ball> &ball, int score0, int score1)
{
    if (!m_window || !m_window->isOpen())
    {
        return;
    }

    m_window->clear(sf::Color::Black);
    m_window->draw(m_fieldSprite);

    // draw ball
    if (ball)
    {
        m_ballSprite.setPosition(ball->position);
        float s = (ball->radius * 2.f) / (float)m_ballSprite.getTexture()->getSize().x;
        m_ballSprite.setScale(s, s);
        m_window->draw(m_ballSprite);
    }

    // draw players
    for (auto &p : players)
    {
        sf::CircleShape cs(p->radius);
        cs.setOrigin(p->radius, p->radius);
        cs.setPosition(p->position);
        if (p->team == 0)
        {
            cs.setFillColor(sf::Color::Blue);
        }
        else
        {
            cs.setFillColor(sf::Color::Red);
        }
        m_window->draw(cs);
    }

    // draw scores
    sf::Text t;
    t.setFont(m_font);
    t.setCharacterSize(24);
    t.setFillColor(sf::Color::Red);
    t.setString(std::to_string(score0) + " - " + std::to_string(score1));
    t.setPosition(10.f, 10.f);
    m_window->draw(t);

    m_window->display();
}

void Renderer::endFrame()
{
}

bool Renderer::isOpen() const
{
    return m_window && m_window->isOpen();
}

bool Renderer::pollEvent(sf::Event &ev)
{
    if (!m_window) return false;
    return m_window->pollEvent(ev);
}

void Renderer::close()
{
    if (m_window && m_window->isOpen())
    {
        m_window->close();
    }
}

} // namespace render
