#include "engine.hpp"

// CONSTRUCTOR AND DESTRUCTOR

Engine::Engine(){

    window = nullptr;
    renderer = nullptr;
    is_fullscreen = false;
}

Engine::~Engine(){

    deinit();

    window = nullptr;
    renderer = nullptr;
    is_fullscreen = false;
}

// INITIALIZATION

bool Engine::init(std::string title, int width, int height){

    if(SDL_Init(SDL_INIT_VIDEO) < 0){

        std::cout << "Unable to initialize SDL! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    int img_flags = IMG_INIT_PNG;

    if(!(IMG_Init(img_flags) & img_flags)){

        std::cout << "Unable to initialize SDL_image! SDL Error: " << IMG_GetError() << std::endl;
        return false;
    }

    if(TTF_Init() == -1){

        std::cout << "Unable to initialize SDL_ttf! SDL Error: " << TTF_GetError() << std::endl;
        return false;
    }

    if(!window || !renderer){

        std::cout << "Unable to initialize engine!" << std::endl;
        return false;
    }

    set_resolution(width, height);

    font_small = TTF_OpenFont("./res/sds.ttf", 10);
    if(font_small == nullptr){

        std::cout << "Unable to initialize font_small! SDL Error: " << TTF_GetError() << std::endl;
        return false;
    }

    return true;
}

void Engine::deinit(){

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void Engine::set_resolution(int width, int height){

    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetWindowSize(window, width, height);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void Engine::toggle_fullscreen(){

    if(is_fullscreen){

        SDL_SetWindowFullscreen(window, 0);

    }else{

        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    }

    is_fullscreen = !is_fullscreen;
}

// TIMING

void Engine::clock_init(){

    second_before_time = SDL_GetTicks();
    frame_before_time = second_before_time;
    last_update_time = second_before_time;
}

float Engine::clock_tick(){

    frames++;
    unsigned long current_time = SDL_GetTicks();

    if(current_time - second_before_time >= SECOND){

        fps = frames;
        ups = (int)deltas;
        frames = 0;
        deltas -= ups;
        second_before_time += SECOND;
    }

    float delta = (current_time - last_update_time) / UPDATE_TIME;
    deltas += delta;
    last_update_time = current_time;

    if(current_time - frame_before_time < FRAME_TIME){

        unsigned long delay_time = FRAME_TIME - (current_time - frame_before_time);
        SDL_Delay(delay_time);
    }

    return delta;
}

// RENDERING

void Engine::render_clear(){

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Engine::render_flip(){

    SDL_RenderPresent(renderer);
}

void Engine::render_fps(){

    render_text("FPS " + std::to_string(fps), COLOR_YELLOW, 0, 0);
    render_text("UPS " + std::to_string(ups), COLOR_YELLOW, 0, 10);
}

void Engine::render_text(std::string text, SDL_Color color, int x, int y){

    SDL_Surface* text_surface = TTF_RenderText_Solid(font_small, text.c_str(), color);

    if(text_surface == nullptr){

        std::cout << "Unable to render text to surface! SDL Error: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    if(text_texture == nullptr){

        std::cout << "Unable to create texture! SDL Error: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Rect source_rect = (SDL_Rect){ .x = 0, .y = 0, .w = text_surface->w, .h = text_surface->h };
    SDL_Rect dest_rect = (SDL_Rect){ .x = x, .y = 0, .w = text_surface->w, .h = text_surface->h };
    SDL_RenderCopy(renderer, text_texture, &source_rect, &dest_rect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}
