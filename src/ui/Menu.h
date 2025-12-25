#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>

namespace ui
{
class Menu
{
public:
    Menu(unsigned int width, unsigned int height);
    ~Menu();

    // run menu loop; returns when user chooses an action
    // onHost: called when user selects Host
    // onJoin(ip): called when user selects Join with entered ip
    void run(std::function<void()> onHost, std::function<void(const std::string&)> onJoin);

private:
    unsigned int m_width;
    unsigned int m_height;
    sf::RenderWindow m_window;
    sf::Font m_font;
    std::string m_inputText;
    std::vector<std::string> m_serverList;
};

} // namespace ui
