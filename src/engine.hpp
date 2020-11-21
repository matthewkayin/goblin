#pragma once

#ifdef _WIN32
    #define SDL_MAIN_HANDLED
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <string>

extern const int SCREEN_WIDTH = 640;
extern const int SCREEN_HEIGHT = 360;

extern const SDL_Color COLOR_WHITE = (SDL_Color){ .r = 255, .g = 255, .b = 255, .a = 255 };
extern const SDL_Color COLOR_RED = (SDL_Color){ .r = 255, .g = 0, .b = 0, .a = 255 };
extern const SDL_Color COLOR_YELLOW = (SDL_Color){ .r = 255, .g = 255, .b = 0, .a = 255 };

class Engine{

    public:
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

        // Rendering Base
        void render_clear();
        void render_flip();
        void render_fps();

        void render_text(std::string text, SDL_Color color, int x, int y);
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        bool is_fullscreen;

        TTF_Font* font_small;

        const int SECOND = 1000;
        const float FRAME_TIME = SECOND / 60.0;
        const float UPDATE_TIME = SECOND / 60.0;
        unsigned long second_before_time;
        unsigned long frame_before_time;
        unsigned long last_update_time;
        float deltas = 0;
        int frames = 0;
        int fps = 0;
        int ups = 0;
};
