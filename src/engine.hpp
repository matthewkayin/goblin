#pragma once

#ifdef _WIN32
    #define SDL_MAIN_HANDLED
#endif

#include "sprite.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <string>
#include <unordered_map>

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

extern SDL_Color COLOR_WHITE;
extern SDL_Color COLOR_BLACK;
extern SDL_Color COLOR_RED;
extern SDL_Color COLOR_YELLOW;

class Engine{

    public:
        typedef struct{
            SDL_Texture* texture;
            int x;
            int y;
        } SpriteInfo;

        // Constructor and destructor
        Engine();
        ~Engine();

        // Initialization
        bool init(std::string title, int width, int height);
        void deinit();
        void set_resolution(int width, int height);
        void toggle_fullscreen();

        // Timing
        void clock_init();
        float clock_tick();

        // Rendering
        void render_clear();
        void render_flip();
        void render_fps();
        void render_set_draw_color(SDL_Color color);
        void render_set_viewport(int x, int y, int width, int height);
        void render_set_offset(int x, int y);

        void render_text(std::string text, SDL_Color color, int x, int y);
        void render_text_multicolor(std::string text, int x, int y, float alpha);
        void render_rect(int x, int y, int width, int height);
        void render_fill_rect(int x, int y, int width, int height);
        void render_sprite(Sprite sprite, int x, int y);

        // Textures
        void texture_load_all();
        void texture_clean_all();
        SDL_Texture* texture_load(std::string path);
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        bool is_fullscreen;

        const int FONT_SMALL_SIZE = 12;
        TTF_Font* font_small;

        SDL_Texture* texture_goblin;
        SDL_Texture* texture_monster;
        std::unordered_map<Sprite, SpriteInfo> spritemap;

        const unsigned long SECOND = 1000;
        const float FRAME_TIME = SECOND / 60.0;
        const float UPDATE_TIME = SECOND / 60.0;
        unsigned long second_before_time;
        unsigned long frame_before_time;
        unsigned long last_update_time;
        float deltas = 0;
        int frames = 0;
        int fps = 0;
        int ups = 0;

        int viewport_x;
        int viewport_y;
        int viewport_width;
        int viewport_height;
        int render_offset_x;
        int render_offset_y;
};
