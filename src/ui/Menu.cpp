#include "Menu.h"
#include <iostream>
#include <SFML/Network.hpp>
#include <chrono>
#include <thread>
#include <algorithm>
#include <filesystem>

namespace ui
{
Menu::Menu(unsigned int width, unsigned int height)
    : m_width(width)
    , m_height(height)
    , m_window(sf::VideoMode(width, height), "PixelBall - Menu")
{
    const std::string absAssets = "/home/isa/Documents/Codes/PixelBall/assets/";
    std::string fontPath = absAssets + "DejaVuSans-Bold.ttf";
    if (!std::filesystem::exists(fontPath)) fontPath = "assets/DejaVuSans-Bold.ttf";
    if (!m_font.loadFromFile(fontPath))
    {
        std::cerr << "Menu: failed to load font " << fontPath << std::endl;
    }
}

Menu::~Menu()
{
    if (m_window.isOpen()) m_window.close();
}

void Menu::run(std::function<void()> onHost, std::function<void(const std::string&)> onJoin)
{
    auto doRefresh = [&]() {
        m_serverList.clear();
        sf::UdpSocket sock;
        if (sock.bind(sf::Socket::AnyPort) != sf::Socket::Done)
        {
            std::cerr << "Menu: failed to bind UDP socket for discovery" << std::endl;
            return;
        }
        sock.setBlocking(false);
        sf::Packet p;
        auto start = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - start < std::chrono::seconds(1))
        {
            sf::IpAddress sender;
            unsigned short port;
            if (sock.receive(p, sender, port) == sf::Socket::Done)
            {
                std::string msg;
                p >> msg;
                // expected format: PixelBall:<port>
                if (msg.rfind("PixelBall:", 0) == 0)
                {
                    std::string portstr = msg.substr(std::string("PixelBall:").size());
                    std::string display = sender.toString() + ":" + portstr;
                    if (std::find(m_serverList.begin(), m_serverList.end(), display) == m_serverList.end())
                    {
                        m_serverList.push_back(display);
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    };

    while (m_window.isOpen())
    {
        sf::Event ev;
        while (m_window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed)
            {
                m_window.close();
                return;
            }

            if (ev.type == sf::Event::TextEntered)
            {
                if (ev.text.unicode == '\b')
                {
                    if (!m_inputText.empty()) m_inputText.pop_back();
                }
                else if (ev.text.unicode < 128)
                {
                    m_inputText.push_back(static_cast<char>(ev.text.unicode));
                }
            }

            if (ev.type == sf::Event::KeyPressed)
            {
                if (ev.key.code == sf::Keyboard::H)
                {
                    // Host
                    onHost();
                    return;
                }
                if (ev.key.code == sf::Keyboard::J)
                {
                    // Join with current text
                    onJoin(m_inputText);
                    return;
                }
                if (ev.key.code == sf::Keyboard::R)
                {
                    // Refresh server list
                    doRefresh();
                }
            }
        }

        m_window.clear(sf::Color(30, 30, 30));

        sf::Text title("PixelBall Menu", m_font, 28);
        title.setPosition(20.f, 20.f);
        m_window.draw(title);

        sf::Text hostHint("Press H to Host server", m_font, 20);
        hostHint.setPosition(20.f, 80.f);
        m_window.draw(hostHint);

        sf::Text joinHint("Type IP then press J to Join (example: 127.0.0.1)", m_font, 18);
        joinHint.setPosition(20.f, 120.f);
        m_window.draw(joinHint);

        sf::Text refreshHint("Press R to refresh LAN servers", m_font, 18);
        refreshHint.setPosition(20.f, 150.f);
        m_window.draw(refreshHint);

        // draw server list
        float y = 190.f;
        for (auto &s : m_serverList)
        {
            sf::Text si(s, m_font, 16);
            si.setPosition(20.f, y);
            si.setFillColor(sf::Color::White);
            m_window.draw(si);
            y += 20.f;
        }

        sf::RectangleShape inputBox(sf::Vector2f((float)m_width - 40.f, 30.f));
        inputBox.setPosition(20.f, 160.f);
        inputBox.setFillColor(sf::Color(60, 60, 60));
        m_window.draw(inputBox);

        sf::Text inputText(m_inputText.empty() ? std::string("Enter IP...") : m_inputText, m_font, 18);
        inputText.setPosition(26.f, 162.f);
        inputText.setFillColor(sf::Color::White);
        m_window.draw(inputText);

        m_window.display();
    }
}

} // namespace ui
