#pragma once

#include <SFML/Network.hpp>
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>
#include "Server.h" // for BallState

namespace connection
{

class Client
{
public:
    Client();
    ~Client();

    // connect to server (IP or hostname), default port 50000
    bool connect(const sf::IpAddress& host, unsigned short port = 50000);
    void disconnect();

    // send local ball state to server
    void sendState(const BallState& s);

    // get last received states (thread-safe copy)
    std::vector<BallState> getStates();

private:
    void receiveLoop();

    std::unique_ptr<sf::TcpSocket> m_socket;
    std::thread m_receiveThread;
    std::mutex m_statesMutex;
    std::vector<BallState> m_states;
    std::atomic<bool> m_running{false};
};

} // namespace connection
