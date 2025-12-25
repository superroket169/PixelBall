#pragma once

#include <SFML/Network.hpp>
#include <thread>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <memory>

namespace connection
{

struct BallState
{
    int id = 0;
    float x = 0, y = 0;
    float vx = 0, vy = 0;
};

class Server
{
public:
    explicit Server(unsigned short port = 50000);
    ~Server();

    // Start listening and accepting clients
    void start();
    // Stop server and join threads
    void stop();

    // Broadcast current known states to all clients
    void broadcastStates();

    // Update a ball state on the server (e.g. from local player)
    void updateState(const BallState& s);

private:
    void acceptLoop();
    void clientLoop(std::shared_ptr<sf::TcpSocket> client);

    unsigned short m_port;
    sf::TcpListener m_listener;
    std::thread m_acceptThread;
    std::thread m_broadcastThread;
    // announcer (UDP broadcast)
    std::thread m_announceThread;
    sf::UdpSocket m_udpSocket;
    std::vector<std::shared_ptr<sf::TcpSocket>> m_clients;
    std::mutex m_clientsMutex;

    std::unordered_map<int, BallState> m_states;
    std::mutex m_statesMutex;

    bool m_running = false;
};

} // namespace connection
