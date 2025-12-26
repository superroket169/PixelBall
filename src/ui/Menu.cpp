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
    sf::Clock caretClock;
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

            if (ev.type == sf::Event::MouseButtonPressed)
            {
                auto mx = (float)ev.mouseButton.x;
                auto my = (float)ev.mouseButton.y;
                // input box region
                sf::FloatRect inputRect(20.f, 160.f, (float)m_width - 40.f, 30.f);
                if (inputRect.contains(mx, my))
                {
                    m_inputFocused = true;
                }
                else
                {
                    m_inputFocused = false;
                }
                // Host button
                sf::FloatRect hostRect(20.f, 80.f, 160.f, 36.f);
                if (hostRect.contains(mx, my))
                {
                    onHost();
                    return;
                }
                // Join button
                sf::FloatRect joinRect(200.f, 80.f, 160.f, 36.f);
                if (joinRect.contains(mx, my))
                {
                    // if a server is selected, join that else input text
                    if (m_selectedServer < m_serverList.size())
                    {
                        onJoin(m_serverList[m_selectedServer]);
                    }
                    else
                    {
                        onJoin(m_inputText);
                    }
                    return;
                }
                // Refresh button
                sf::FloatRect refRect(380.f, 80.f, 120.f, 36.f);
                if (refRect.contains(mx, my))
                {
                    doRefresh();
                }
                // server list clicks
                float y = 210.f;
                for (size_t i = 0; i < m_serverList.size(); ++i)
                {
                    sf::FloatRect r(20.f, y - 2.f, (float)m_width - 40.f, 20.f);
                    if (r.contains(mx, my))
                    {
                        m_selectedServer = i;
                    }
                    y += 22.f;
                }
            }

            if (ev.type == sf::Event::TextEntered && m_inputFocused)
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
                    onHost();
                    return;
                }
                if (ev.key.code == sf::Keyboard::J)
                {
                    if (m_selectedServer < m_serverList.size())
                    {
                        onJoin(m_serverList[m_selectedServer]);
                    }
                    else
                    {
                        onJoin(m_inputText);
                    }
                    return;
                }
                if (ev.key.code == sf::Keyboard::R)
                {
                    doRefresh();
                }
                if (ev.key.code == sf::Keyboard::Up)
                {
                    if (!m_serverList.empty())
                    {
                        if (m_selectedServer == (size_t)-1) m_selectedServer = 0;
                        else m_selectedServer = (m_selectedServer + m_serverList.size() - 1) % m_serverList.size();
                    }
                }
                if (ev.key.code == sf::Keyboard::Down)
                {
                    if (!m_serverList.empty())
                    {
                        if (m_selectedServer == (size_t)-1) m_selectedServer = 0;
                        else m_selectedServer = (m_selectedServer + 1) % m_serverList.size();
                    }
                }
                if (ev.key.code == sf::Keyboard::Escape)
                {
                    m_inputFocused = false;
                }
            }
        }

        // caret blink
        if (caretClock.getElapsedTime().asMilliseconds() > 500)
        {
            m_showCaret = !m_showCaret;
            caretClock.restart();
        }

        m_window.clear(sf::Color(28, 30, 34));

        sf::Text title("PixelBall", m_font, 36);
        title.setPosition(20.f, 14.f);
        title.setFillColor(sf::Color::White);
        m_window.draw(title);

        // Buttons
        sf::RectangleShape hostBtn(sf::Vector2f(160.f, 36.f));
        hostBtn.setPosition(20.f, 80.f);
        hostBtn.setFillColor(sf::Color(70, 130, 180));
        m_window.draw(hostBtn);
        sf::Text hostLabel("Host (H)", m_font, 18);
        hostLabel.setPosition(32.f, 86.f);
        hostLabel.setFillColor(sf::Color::White);
        m_window.draw(hostLabel);

        sf::RectangleShape joinBtn(sf::Vector2f(160.f, 36.f));
        joinBtn.setPosition(200.f, 80.f);
        joinBtn.setFillColor(sf::Color(80, 180, 130));
        m_window.draw(joinBtn);
        sf::Text joinLabel("Join (J)", m_font, 18);
        joinLabel.setPosition(212.f, 86.f);
        joinLabel.setFillColor(sf::Color::White);
        m_window.draw(joinLabel);

        sf::RectangleShape refBtn(sf::Vector2f(120.f, 36.f));
        refBtn.setPosition(380.f, 80.f);
        refBtn.setFillColor(sf::Color(180, 120, 80));
        m_window.draw(refBtn);
        sf::Text refLabel("Refresh (R)", m_font, 16);
        refLabel.setPosition(392.f, 86.f);
        refLabel.setFillColor(sf::Color::White);
        m_window.draw(refLabel);

        // Input box
        sf::RectangleShape inputBox(sf::Vector2f((float)m_width - 40.f, 30.f));
        inputBox.setPosition(20.f, 160.f);
        inputBox.setFillColor(m_inputFocused ? sf::Color(255, 255, 255, 20) : sf::Color(60, 60, 60));
        inputBox.setOutlineThickness(m_inputFocused ? 2.f : 1.f);
        inputBox.setOutlineColor(m_inputFocused ? sf::Color(200, 200, 255) : sf::Color(80, 80, 80));
        m_window.draw(inputBox);

        std::string inputDisplay = m_inputText.empty() ? std::string("Enter IP or select server...") : m_inputText;
        if (m_inputFocused && m_showCaret) inputDisplay.push_back('|');
        sf::Text inputText(inputDisplay, m_font, 18);
        inputText.setPosition(26.f, 162.f);
        inputText.setFillColor(sf::Color::White);
        m_window.draw(inputText);

        // server list box
        sf::RectangleShape listBox(sf::Vector2f((float)m_width - 40.f, (float)m_height - 210.f));
        listBox.setPosition(20.f, 210.f);
        listBox.setFillColor(sf::Color(18, 20, 24));
        listBox.setOutlineThickness(1.f);
        listBox.setOutlineColor(sf::Color(60, 60, 60));
        m_window.draw(listBox);

        // draw server list
        float y = 210.f + 6.f;
        for (size_t i = 0; i < m_serverList.size(); ++i)
        {
            const auto &s = m_serverList[i];
            sf::Text si(s, m_font, 16);
            si.setPosition(26.f, y);
            if (i == m_selectedServer)
            {
                sf::RectangleShape hl(sf::Vector2f((float)m_width - 52.f, 20.f));
                hl.setPosition(24.f, y - 2.f);
                hl.setFillColor(sf::Color(70, 70, 120));
                m_window.draw(hl);
                si.setFillColor(sf::Color::White);
            }
            else
            {
                si.setFillColor(sf::Color(200, 200, 200));
            }
            m_window.draw(si);
            y += 22.f;
        }

        // footer / help
        sf::Text help("Click a server to select it, double-click Join or press J. Escape to unfocus input.", m_font, 12);
        help.setPosition(20.f, (float)m_height - 28.f);
        help.setFillColor(sf::Color(160, 160, 160));
        m_window.draw(help);

        m_window.display();
    }
}

} // namespace ui
