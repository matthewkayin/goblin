#include "engine.hpp"

int main(){

    Engine engine = Engine();
    bool init_successful = engine.init("roguelike", 1280, 720);

    if(!init_successful){

        std::cout << "Exiting..." << std::endl;
        return 0;
    }

    float delta = 0;
    bool running = true;
    engine.clock_init();
    while(running){

        SDL_Event e;
        while(SDL_PollEvent(&e) != 0){

            if(e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)){

                running = false;
            }
        }
        engine.render_clear();
        engine.render_fps();
        engine.render_flip();
        delta = engine.clock_tick();
    }

    return 0;
}
