#include "Server.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <chrono>

namespace connection
{

Server::Server(unsigned short port)
    : m_port(port)
{
}

Server::~Server()
{
    stop();
}

void Server::start()
{
    if (m_running)
    {
        return;
    }

    if (m_listener.listen(m_port) != sf::Socket::Done)
    {
        // TODO: handle listen error
        return;
    }

    m_running = true;
    m_acceptThread = std::thread(&Server::acceptLoop, this);
}

void Server::stop()
{
    if (!m_running)
    {
        return;
    }

    m_running = false;

    // properly close the listener to unblock accept()
    m_listener.close();

    if (m_acceptThread.joinable())
    {
        m_acceptThread.join();
    }

    std::lock_guard<std::mutex> lk(m_clientsMutex);
    for (auto &c : m_clients)
    {
        if (c)
        {
            c->disconnect();
        }
    }

    m_clients.clear();
}

void Server::acceptLoop()
{
    while (m_running)
    {
        auto client = std::make_shared<sf::TcpSocket>();
        if (m_listener.accept(*client) == sf::Socket::Done)
        {
            client->setBlocking(false);
            {
                std::lock_guard<std::mutex> lk(m_clientsMutex);
                m_clients.push_back(client);
            }

            std::thread(&Server::clientLoop, this, client).detach();
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void Server::clientLoop(std::shared_ptr<sf::TcpSocket> client)
{
    sf::Packet packet;
    while (m_running)
    {
        sf::Socket::Status status = client->receive(packet);
        if (status == sf::Socket::Done)
        {
            int type = 0;
            packet >> type;
            if (type == 0)
            {
                BallState s;
                packet >> s.id >> s.x >> s.y >> s.vx >> s.vy;
                updateState(s);
                // optionally immediately broadcast this single update
            }

            packet.clear();
        }
        else if (status == sf::Socket::Disconnected)
        {
            break;
        }
        else
        {
            // not ready or partial
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }

    // remove client
    std::lock_guard<std::mutex> lk(m_clientsMutex);
    auto it = std::find(m_clients.begin(), m_clients.end(), client);
    if (it != m_clients.end())
    {
        m_clients.erase(it);
    }
}

void Server::updateState(const BallState& s)
{
    std::lock_guard<std::mutex> lk(m_statesMutex);
    m_states[s.id] = s;
}

void Server::broadcastStates()
{
    std::vector<std::shared_ptr<sf::TcpSocket>> clientsCopy;
    {
        std::lock_guard<std::mutex> lk(m_clientsMutex);
        clientsCopy = m_clients;
    }

    sf::Packet p;
    p << (int)1; // packet type 1: full state
    {
        std::lock_guard<std::mutex> lk(m_statesMutex);
        p << (int)m_states.size();
        for (auto &kv : m_states)
        {
            const BallState &s = kv.second;
            p << s.id << s.x << s.y << s.vx << s.vy;
        }
    }

    for (auto &c : clientsCopy)
    {
        if (!c)
        {
            continue;
        }

        if (c->send(p) != sf::Socket::Done)
        {
            // TODO: handle failed send (client likely disconnected)
        }
    }
}

} // namespace connection
