#include "Client.h"
#include <SFML/Network.hpp>
#include <chrono>

namespace connection
{

Client::Client() = default;

Client::~Client()
{
    disconnect();
}

bool Client::connect(const sf::IpAddress& host, unsigned short port)
{
    disconnect();
    m_socket = std::make_unique<sf::TcpSocket>();
    m_socket->setBlocking(true);
    if (m_socket->connect(host, port, sf::seconds(2.f)) != sf::Socket::Done)
    {
        m_socket.reset();
        return false;
    }
    m_socket->setBlocking(false);
    m_running = true;
    m_receiveThread = std::thread(&Client::receiveLoop, this);
    return true;
}

void Client::disconnect()
{
    if (!m_running)
    {
        if (m_socket) { m_socket->disconnect(); m_socket.reset(); }
        return;
    }
    m_running = false;
    if (m_receiveThread.joinable()) m_receiveThread.join();
    if (m_socket) { m_socket->disconnect(); m_socket.reset(); }
}

void Client::sendState(const BallState& s)
{
    if (!m_socket) return;
    sf::Packet p;
    p << (int)0; // type 0 = single state update
    p << s.id << s.x << s.y << s.vx << s.vy;
    if (m_socket->send(p) != sf::Socket::Done)
    {
        // TODO: handle send error
    }
}

std::vector<BallState> Client::getStates()
{
    std::lock_guard<std::mutex> lk(m_statesMutex);
    return m_states;
}

void Client::receiveLoop()
{
    sf::Packet packet;
    while (m_running && m_socket)
    {
        sf::Socket::Status status = m_socket->receive(packet);
        if (status == sf::Socket::Done)
        {
            int type = 0;
            packet >> type;
            if (type == 1) {
                int count = 0;
                packet >> count;
                std::vector<BallState> tmp;
                for (int i = 0; i < count; ++i)
                {
                    BallState s; packet >> s.id >> s.x >> s.y >> s.vx >> s.vy;
                    tmp.push_back(s);
                }
                {
                    std::lock_guard<std::mutex> lk(m_statesMutex);
                    m_states = std::move(tmp);
                }
            }
            packet.clear();
        }
        else if (status == sf::Socket::Disconnected)
        {
            break;
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
    m_running = false;
}

} // namespace connection
