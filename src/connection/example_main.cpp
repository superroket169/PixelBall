#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>

#include "Server.h"
#include "Client.h"

using namespace connection;

void runServer(unsigned short port)
{
    Server server(port);
    server.start();

    std::string line;
    std::cout << "Server listening on port " << port << ". Commands: update <id> <x> <y> <vx> <vy>, broadcast, quit\n";
    while (std::getline(std::cin, line))
    {
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        if (cmd == "quit")
        {
            break;
        }
        else if (cmd == "update")
        {
            BallState s;
            iss >> s.id >> s.x >> s.y >> s.vx >> s.vy;
            server.updateState(s);
        }
        else if (cmd == "broadcast")
        {
            server.broadcastStates();
        }
    }

    server.stop();
}

void runClient(const std::string &host, unsigned short port)
{
    Client client;
    if (!client.connect(sf::IpAddress(host), port))
    {
        std::cerr << "Failed to connect to " << host << ":" << port << "\n";
        return;
    }

    std::atomic<bool> running(true);
    std::thread printer([&]()
    {
        while (running)
        {
            auto states = client.getStates();
            if (!states.empty())
            {
                std::cout << "Received states:\n";
                for (auto &s : states)
                {
                    std::cout << " id=" << s.id << " x=" << s.x << " y=" << s.y << " vx=" << s.vx << " vy=" << s.vy << "\n";
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    });

    std::string line;
    std::cout << "Client connected to " << host << ":" << port << ". Commands: send <id> <x> <y> <vx> <vy>, quit\n";
    while (std::getline(std::cin, line))
    {
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        if (cmd == "quit")
        {
            break;
        }
        else if (cmd == "send")
        {
            BallState s;
            iss >> s.id >> s.x >> s.y >> s.vx >> s.vy;
            client.sendState(s);
        }
    }

    running = false;
    if (printer.joinable())
    {
        printer.join();
    }

    client.disconnect();
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Usage:\n  " << argv[0] << " server [port]\n  " << argv[0] << " client <host> [port]\n";
        return 1;
    }

    std::string mode = argv[1];
    if (mode == "autotest")
    {
        // Simple automated test: start server and two clients in-process.
        unsigned short port = 50000;
        Server server(port);
        server.start();

        // client A: listener
        std::thread clientA([&]()
        {
            Client c;
            if (!c.connect(sf::IpAddress::LocalHost, port))
            {
                std::cerr << "clientA failed to connect\n";
                return;
            }

            // Wait and print received states for a short while
            for (int i = 0; i < 10; ++i)
            {
                auto states = c.getStates();
                if (!states.empty())
                {
                    std::cout << "[clientA] got " << states.size() << " states\n";
                    for (auto &s : states)
                    {
                        std::cout << " id=" << s.id << " x=" << s.x << " y=" << s.y << " vx=" << s.vx << " vy=" << s.vy << "\n";
                    }
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            c.disconnect();
        });

        // client B: sender
        std::thread clientB([&]()
        {
            Client c;
            if (!c.connect(sf::IpAddress::LocalHost, port))
            {
                std::cerr << "clientB failed to connect\n";
                return;
            }

            // send a state
            BallState s{42, 12.5f, 34.0f, 1.0f, -0.5f};
            c.sendState(s);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            c.disconnect();
        });

        // give clients time to connect/send
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        // trigger server broadcast so listeners get the state
        server.broadcastStates();

        if (clientB.joinable()) clientB.join();
        if (clientA.joinable()) clientA.join();

        server.stop();
        return 0;
    }
    if (mode == "server")
    {
        unsigned short port = 50000;
        if (argc >= 3)
        {
            port = static_cast<unsigned short>(std::stoi(argv[2]));
        }
        runServer(port);
    }
    else if (mode == "client")
    {
        if (argc < 3)
        {
            std::cerr << "client requires a host argument\n";
            return 1;
        }
        std::string host = argv[2];
        unsigned short port = 50000;
        if (argc >= 4)
        {
            port = static_cast<unsigned short>(std::stoi(argv[3]));
        }
        runClient(host, port);
    }
    else
    {
        std::cerr << "Unknown mode: " << mode << "\n";
        return 1;
    }

    return 0;
}
