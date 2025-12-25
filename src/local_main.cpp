#include <iostream>
#include <thread>
#include <chrono>

#include "core/Game.h"
#include "input/Input.h"
#include "render/Renderer.h"
#include "resource/Resources.h"
#include "entity/Player.h"

int main()
{
    using namespace core;
    using namespace input;
    using namespace render;
    using namespace resource;

    unsigned int width = 1280;
    unsigned int height = 720;

    Resources res;
    Renderer renderer(width, height, res);

    Game game(Mode::Local, 60);
    // create a local player and add to game
    auto p = std::make_shared<entity::Player>();
    p->id = 1;
    p->team = 0;
    p->position = sf::Vector2f(width * 0.25f, height * 0.5f);
    game.players().push_back(p);

    // start server/game loop
    game.start();

    InputHandler input;

    int score0 = 0;
    int score1 = 0;

    game.onGoal = [&](int team)
    {
        if (team == 0) score0++;
        else score1++;
        std::cout << "Goal: team " << team << "  Score " << score0 << " - " << score1 << "\n";
    };


    // main render loop
    sf::Event ev;
    while (renderer.isOpen())
    {
        // poll events and forward to input handler
        while (renderer.pollEvent(ev))
        {
            input.processEvent(ev);
            if (ev.type == sf::Event::Closed)
            {
                renderer.close();
                break;
            }
        }

        // also sample real-time state so holding keys works
        input.pollRealTime();
        input.applyTo(*p);

        // draw
        renderer.draw(game.players(), game.ball(), score0, score1);

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    game.stop();
    return 0;
}
