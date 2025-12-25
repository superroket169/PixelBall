#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "../entity/Player.h"
#include "../entity/Ball.h"

namespace resource
{
    class Resources;
}

namespace render
{
class Renderer
{
public:
    Renderer(unsigned int width, unsigned int height, resource::Resources &res);
    ~Renderer();

    void beginFrame();
    void draw(const std::vector<std::shared_ptr<entity::Player>> &players, const std::shared_ptr<entity::Ball> &ball, int score0, int score1);
    void endFrame();

    // poll window events
    bool pollEvent(sf::Event &ev);

    // close the window
    void close();

    bool isOpen() const;

private:
    unsigned int m_width;
    unsigned int m_height;
    resource::Resources &m_res;
    std::unique_ptr<sf::RenderWindow> m_window;
    sf::Sprite m_fieldSprite;
    sf::Sprite m_ballSprite;
    sf::Font m_font;
};

} // namespace render
