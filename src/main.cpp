#include "engine.hpp"
#include "gamestate.hpp"

#include <SDL2/SDL.h>

#include <unordered_map>

void render_ui(Engine* engine);
void render_gamestate(Engine* engine, Gamestate::State current_state);

const int UI_MARGIN = 5 * 2;
const int VIEWPORT_WIDTH = 13 * 36 * 2;
const int VIEWPORT_HEIGHT = 288 * 2;

int main(){

    // Init engine
    Engine engine = Engine();
    bool init_successful = engine.init("roguelike", 1280, 720);
    if(!init_successful){

        std::cout << "Exiting..." << std::endl;
        return 0;
    }
    engine.render_set_viewport(UI_MARGIN, UI_MARGIN, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    // Define keymap
    std::unordered_map<SDL_Keycode, Gamestate::Input> keymap = {
        {SDLK_w, Gamestate::Input::UP},
        {SDLK_d, Gamestate::Input::RIGHT},
        {SDLK_s, Gamestate::Input::DOWN},
        {SDLK_a, Gamestate::Input::LEFT},
    };

    // Init gamestate
    Gamestate gamestate;

    // Gameloop
    float delta = 0;
    bool running = true;
    engine.clock_init();
    while(running){

        SDL_Event e;
        while(SDL_PollEvent(&e) != 0){

            if(e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)){

                running = false;

            }else if(e.type == SDL_KEYDOWN){

                int key = e.key.keysym.sym;

                if(keymap.find(key) != keymap.end()){

                    gamestate.input_enqueue(keymap.at(key));
                }
            }
        }

        gamestate.update(delta);

        engine.render_clear();

        render_gamestate(&engine, gamestate.get_state());
        render_ui(&engine);

        // engine.render_fps();
        engine.render_flip();

        delta = engine.clock_tick();
    }

    return 0;
}

void render_ui(Engine* engine){

    static const int UI_WIDTH = SCREEN_WIDTH - (UI_MARGIN * 2);
    static const int UI_HEIGHT = SCREEN_HEIGHT - (UI_MARGIN * 2);
    static const int CHATBOX_HEIGHT = UI_HEIGHT - VIEWPORT_HEIGHT;
    static const int CHATBOX_Y = UI_MARGIN + VIEWPORT_HEIGHT;
    static const int TEXT_PADDING = 2;
    static const int PANEL_X = UI_MARGIN + VIEWPORT_WIDTH;
    static const int PANEL_WIDTH = UI_WIDTH - VIEWPORT_WIDTH;

    engine->render_set_draw_color(COLOR_BLACK);

    engine->render_fill_rect(0, 0, UI_MARGIN, VIEWPORT_HEIGHT);
    engine->render_fill_rect(UI_MARGIN, 0, VIEWPORT_WIDTH, UI_MARGIN);
    engine->render_fill_rect(UI_MARGIN, CHATBOX_Y, UI_WIDTH, CHATBOX_HEIGHT);
    engine->render_fill_rect(PANEL_X, UI_MARGIN, PANEL_WIDTH, VIEWPORT_HEIGHT + 1);

    engine->render_set_draw_color(COLOR_WHITE);

    engine->render_rect(UI_MARGIN, UI_MARGIN, UI_WIDTH, UI_HEIGHT);
    engine->render_rect(UI_MARGIN, CHATBOX_Y, UI_WIDTH, CHATBOX_HEIGHT);
    engine->render_rect(PANEL_X, UI_MARGIN, PANEL_WIDTH, VIEWPORT_HEIGHT + 1);

    engine->render_text("Test log", COLOR_WHITE, UI_MARGIN + TEXT_PADDING, CHATBOX_Y + TEXT_PADDING + 2);
    engine->render_text("Test log", COLOR_WHITE, UI_MARGIN + TEXT_PADDING, CHATBOX_Y + TEXT_PADDING + 10);
    engine->render_text("Test log", COLOR_WHITE, UI_MARGIN + TEXT_PADDING, CHATBOX_Y + TEXT_PADDING + 18);
    engine->render_text("Test log", COLOR_WHITE, UI_MARGIN + TEXT_PADDING, CHATBOX_Y + TEXT_PADDING + 26);
    engine->render_text("Test log", COLOR_WHITE, UI_MARGIN + TEXT_PADDING, CHATBOX_Y + TEXT_PADDING + 34);
    engine->render_text("Test log", COLOR_WHITE, UI_MARGIN + TEXT_PADDING, CHATBOX_Y + TEXT_PADDING + 42);
    engine->render_text("Test log", COLOR_WHITE, UI_MARGIN + TEXT_PADDING, CHATBOX_Y + TEXT_PADDING + 50);

    engine->render_text("Jerry the Goblin", COLOR_WHITE, PANEL_X + TEXT_PADDING, UI_MARGIN + 4);
}

void render_gamestate(Engine* engine, Gamestate::State current_state){

    engine->render_sprite(Engine::Sprite::GOBLIN_BASE, current_state.player_x, current_state.player_y);
    engine->render_sprite(Engine::Sprite::GOBLIN_L_UNARMED, current_state.player_x, current_state.player_y);
    engine->render_sprite(Engine::Sprite::GOBLIN_R_UNARMED, current_state.player_x, current_state.player_y);
}
