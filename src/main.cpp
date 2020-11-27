#include "engine.hpp"
#include "gamestate.hpp"

#include <SDL2/SDL.h>

#include <unordered_map>
#include <string>

void render_ui(Engine* engine, Gamestate* gamestate);
void render_gamestate(Engine* engine, Gamestate* gamestate);

const int UI_MARGIN = 10;
const int VIEWPORT_WIDTH = 936;
const int VIEWPORT_HEIGHT = 576;

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
        {SDLK_a, Gamestate::Input::LEFT}
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

        render_gamestate(&engine, &gamestate);
        render_ui(&engine, &gamestate);

        // engine.render_fps();
        engine.render_flip();

        delta = engine.clock_tick();
    }

    return 0;
}

void render_ui(Engine* engine, Gamestate* gamestate){

    static const int UI_WIDTH = SCREEN_WIDTH - (UI_MARGIN * 2);
    static const int UI_HEIGHT = SCREEN_HEIGHT - (UI_MARGIN * 2);
    static const int CHATBOX_HEIGHT = UI_HEIGHT - VIEWPORT_HEIGHT;
    static const int CHATBOX_Y = UI_MARGIN + VIEWPORT_HEIGHT;
    static const int TEXT_PADDING_X = 4;
    static const int TEXT_PADDING_Y = 6;
    static const int LINE_HEIGHT = 14;
    static const int PANEL_X = UI_MARGIN + VIEWPORT_WIDTH;
    static const int PANEL_WIDTH = UI_WIDTH - VIEWPORT_WIDTH;

    engine->render_set_draw_color(COLOR_BLACK);
    engine->render_set_offset(0, 0);

    engine->render_fill_rect(0, 0, UI_MARGIN, VIEWPORT_HEIGHT);
    engine->render_fill_rect(UI_MARGIN, 0, VIEWPORT_WIDTH, UI_MARGIN);
    engine->render_fill_rect(UI_MARGIN, CHATBOX_Y, UI_WIDTH, CHATBOX_HEIGHT);
    engine->render_fill_rect(PANEL_X, UI_MARGIN, PANEL_WIDTH, VIEWPORT_HEIGHT + 1);

    engine->render_set_draw_color(COLOR_WHITE);

    engine->render_rect(UI_MARGIN, UI_MARGIN, UI_WIDTH, UI_HEIGHT);
    engine->render_rect(UI_MARGIN, CHATBOX_Y, UI_WIDTH, CHATBOX_HEIGHT);
    engine->render_rect(PANEL_X, UI_MARGIN, PANEL_WIDTH, VIEWPORT_HEIGHT + 1);

    engine->render_set_offset(UI_MARGIN + TEXT_PADDING_X, CHATBOX_Y + TEXT_PADDING_Y);
    for(int i = 0; i < 8; i++){

        int index = (gamestate->log_head + i) % 8;
        if(gamestate->log[index] != ""){

            float alpha = 1.0 - (0.1 * i);
            engine->render_text_multicolor(gamestate->log[index], 0, LINE_HEIGHT * (7 - i), alpha);
        }
    }

    engine->render_set_offset(PANEL_X, UI_MARGIN);
    engine->render_text("Jerry the Goblin", COLOR_WHITE, TEXT_PADDING_X, TEXT_PADDING_Y);

    engine->render_text("Health", COLOR_WHITE, 28, 24);
    engine->render_set_draw_color(COLOR_RED);
    engine->render_fill_rect(112, 24, 200 * ((float)gamestate->player_health / gamestate->player_max_health), LINE_HEIGHT);
    std::string health_string = std::to_string(gamestate->player_health) + " / " + std::to_string(gamestate->player_max_health);
    engine->render_text(health_string, COLOR_WHITE, 152, 25);
}

void render_gamestate(Engine* engine, Gamestate* gamestate){

    static const int ENEMY_HEALTHBAR_WIDTH = 32;
    static const int ENEMY_HEALTHBAR_HEIGHT = 4;

    engine->render_set_offset(UI_MARGIN, UI_MARGIN);

    engine->render_sprite(gamestate->sprite_player_base, gamestate->player_x * 36, gamestate->player_y * 36);
    engine->render_sprite(gamestate->sprite_player_larm, gamestate->player_x * 36, gamestate->player_y * 36);
    engine->render_sprite(gamestate->sprite_player_rarm, gamestate->player_x * 36, gamestate->player_y * 36);

    engine->render_set_draw_color(COLOR_RED);
    for(unsigned int i = 0; i < gamestate->enemy.size(); i++){

        Gamestate::Enemy to_render = gamestate->enemy.at(i);
        engine->render_sprite(to_render.sprite, to_render.x * 36, to_render.y * 36);
        engine->render_fill_rect((to_render.x * 36) + 2, (to_render.y * 36) + 2, (int)(ENEMY_HEALTHBAR_WIDTH * ((float)to_render.health / to_render.max_health)), ENEMY_HEALTHBAR_HEIGHT);
    }
}
