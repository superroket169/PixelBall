#include <iostream>
#include <thread>

#include "ui/Menu.h"
#include "core/Game.h"
#include "render/Renderer.h"
#include "resource/Resources.h"
#include "input/Input.h"
#include "connection/Server.h"
#include "connection/Client.h"

int main()
{
    using namespace ui;
    using namespace core;
    using namespace resource;
    using namespace render;
    using namespace input;

    Menu menu(800, 240);
    Resources res;

    auto onHost = [&]()
    {
        // start server in background
        static std::unique_ptr<connection::Server> server;
        server = std::make_unique<connection::Server>(50000);
        server->start();
        std::cout << "Hosting server on port 50000" << std::endl;

        // run local game (server authoritative will be implemented later)
        Game game(Mode::Local, 60);
        auto p = std::make_shared<entity::Player>();
        p->id = 1;
        p->team = 0;
        p->position = sf::Vector2f(200.f, 200.f);
        game.players().push_back(p);
        Renderer renderer(1280, 720, res);
        InputHandler input;
        game.start();

        int score0 = 0, score1 = 0;
        game.onGoal = [&](int team) {
            if (team == 0) score0++; else score1++;
            std::cout << "Goal: " << score0 << " - " << score1 << std::endl;
        };

        sf::Event ev;
        while (renderer.isOpen())
        {
            while (renderer.pollEvent(ev))
            {
                input.processEvent(ev);
                if (ev.type == sf::Event::Closed) { renderer.close(); break; }
            }
            input.pollRealTime();
            input.applyTo(*p);
            // push local player state to authoritative server
            connection::BallState ps;
            ps.id = p->id;
            ps.x = p->position.x;
            ps.y = p->position.y;
            ps.vx = p->velocity.x;
            ps.vy = p->velocity.y;
            server->updateState(ps);

            // also push ball state
            if (game.ball())
            {
                connection::BallState bs;
                bs.id = 0; // reserve id 0 for ball
                bs.x = game.ball()->position.x;
                bs.y = game.ball()->position.y;
                bs.vx = game.ball()->velocity.x;
                bs.vy = game.ball()->velocity.y;
                server->updateState(bs);
            }
            renderer.draw(game.players(), game.ball(), score0, score1);
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

        game.stop();
        server->stop();
    };

    auto onJoin = [&](const std::string &ip)
    {
        connection::Client client;
        if (client.connect(sf::IpAddress(ip.empty() ? "127.0.0.1" : ip), 50000))
        {
            std::cout << "Connected to " << (ip.empty() ? "127.0.0.1" : ip) << std::endl;
        }
        else
        {
            std::cout << "Failed to connect to " << (ip.empty() ? "127.0.0.1" : ip) << std::endl;
        }

        // run local client view (network integration later)
        Game game(Mode::Client, 60);
        auto p = std::make_shared<entity::Player>();
        p->id = 2;
        p->team = 1;
        p->position = sf::Vector2f(400.f, 200.f);
        game.players().push_back(p);
        Renderer renderer(1280, 720, res);
        InputHandler input;
        game.start();

        int score0 = 0, score1 = 0;

        sf::Event ev;
        while (renderer.isOpen())
        {
            while (renderer.pollEvent(ev))
            {
                input.processEvent(ev);
                if (ev.type == sf::Event::Closed) { renderer.close(); break; }
            }
            input.pollRealTime();
            input.applyTo(*p);
            // send local player state to server
            connection::BallState ps;
            ps.id = p->id;
            ps.x = p->position.x;
            ps.y = p->position.y;
            ps.vx = p->velocity.x;
            ps.vy = p->velocity.y;
            client.sendState(ps);

            // apply received states from server
            auto states = client.getStates();
            for (auto &s : states)
            {
                if (s.id == 0)
                {
                    // ball
                    if (game.ball())
                    {
                        game.ball()->position = sf::Vector2f(s.x, s.y);
                        game.ball()->velocity = sf::Vector2f(s.vx, s.vy);
                    }
                }
                else
                {
                    // player
                    bool found = false;
                    for (auto &pl : game.players())
                    {
                        if (pl->id == s.id)
                        {
                            pl->position = sf::Vector2f(s.x, s.y);
                            pl->velocity = sf::Vector2f(s.vx, s.vy);
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                    {
                        auto np = std::make_shared<entity::Player>();
                        np->id = s.id;
                        np->position = sf::Vector2f(s.x, s.y);
                        np->velocity = sf::Vector2f(s.vx, s.vy);
                        game.players().push_back(np);
                    }
                }
            }
            renderer.draw(game.players(), game.ball(), score0, score1);
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

        game.stop();
        client.disconnect();
    };

    menu.run(onHost, onJoin);
    return 0;
}
