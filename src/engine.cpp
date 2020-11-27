#include "engine.hpp"

int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 720;

SDL_Color COLOR_WHITE = (SDL_Color){ .r = 255, .g = 255, .b = 255, .a = 255 };
SDL_Color COLOR_BLACK = (SDL_Color){ .r = 0, .g = 0, .b = 0, .a = 255 };
SDL_Color COLOR_RED = (SDL_Color){ .r = 255, .g = 0, .b = 0, .a = 255 };
SDL_Color COLOR_YELLOW = (SDL_Color){ .r = 255, .g = 255, .b = 0, .a = 255 };

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

    font_small = TTF_OpenFont("./res/sds.ttf", FONT_SMALL_SIZE);
    if(font_small == nullptr){

        std::cout << "Unable to initialize font_small! SDL Error: " << TTF_GetError() << std::endl;
        return false;
    }

    texture_load_all();

    return true;
}

void Engine::deinit(){

    texture_clean_all();

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

void Engine::render_set_draw_color(SDL_Color color){

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void Engine::render_set_viewport(int x, int y, int width, int height){

    viewport_x = x;
    viewport_y = y;
    viewport_width = width;
    viewport_height = height;
}

void Engine::render_set_offset(int x, int y){

    render_offset_x = x;
    render_offset_y = y;
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
    SDL_Rect dest_rect = (SDL_Rect){ .x = x + render_offset_x, .y = y + render_offset_y, .w = text_surface->w, .h = text_surface->h };
    SDL_RenderCopy(renderer, text_texture, &source_rect, &dest_rect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

void Engine::render_text_multicolor(std::string text, int x, int y, float alpha){

    int active_x = x;

    while(text.length() != 0){

        std::string to_render;
        SDL_Color color;

        std::size_t red_pos = text.find("r(");
        std::size_t yellow_pos = text.find("y(");

        if(red_pos == 0){

            color = COLOR_RED;
            to_render = text.substr(2, text.find(")") - 2);
            text = text.substr(text.find(")") + 1);

        }else if(yellow_pos == 0){

            color = COLOR_YELLOW;
            to_render = text.substr(2, text.find(")") - 2);
            text = text.substr(text.find(")") + 1);

        }else{

            color = COLOR_WHITE;
            std::size_t substr_length = text.length();
            if(red_pos != std::string::npos && red_pos < substr_length){

                substr_length = red_pos;
            }
            if(yellow_pos != std::string::npos && yellow_pos < substr_length){

                substr_length = yellow_pos;
            }

            to_render = text.substr(0, substr_length);
            text = text.substr(substr_length);
        }

        color.a = 255 * alpha;

        SDL_Surface* text_surface = TTF_RenderText_Solid(font_small, to_render.c_str(), color);

        if(text_surface == nullptr){

            std::cout << "Unable to render text to surface! SDL Error: " << TTF_GetError() << std::endl;
            return;
        }

        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

        if(text_texture == nullptr){

            std::cout << "Unable to create texture! SDL Error: " << SDL_GetError() << std::endl;
        }

        SDL_Rect source_rect = (SDL_Rect){ .x = 0, .y = 0, .w = text_surface->w, .h = text_surface->h };
        SDL_Rect dest_rect = (SDL_Rect){ .x = active_x + render_offset_x, .y = y + render_offset_y, .w = text_surface->w, .h = text_surface->h };
        SDL_RenderCopy(renderer, text_texture, &source_rect, &dest_rect);

        active_x += text_surface->w;

        SDL_FreeSurface(text_surface);
        SDL_DestroyTexture(text_texture);
    }
}

void Engine::render_rect(int x, int y, int width, int height){

    SDL_Rect to_draw = (SDL_Rect){ .x = x + render_offset_x, .y = y + render_offset_y, .w = width, .h = height };
    SDL_RenderDrawRect(renderer, &to_draw);
}

void Engine::render_fill_rect(int x, int y, int width, int height){

    SDL_Rect to_draw = (SDL_Rect){ .x = x + render_offset_x, .y = y + render_offset_y, .w = width, .h = height };
    SDL_RenderFillRect(renderer, &to_draw);
}

void Engine::render_sprite(Sprite sprite, int x, int y){

    if(x + 36 <= 0 || x >= viewport_width || y + 36 <= 0 || y >= viewport_height){

        return;
    }

    SpriteInfo sprite_info = spritemap.at(sprite);
    SDL_Rect src_rect = (SDL_Rect){ .x = sprite_info.x * 36, .y = sprite_info.y * 36, .w = 36, .h = 36 };
    SDL_Rect dst_rect = (SDL_Rect){ .x = viewport_x + x, .y = viewport_y + y, .w = 36, .h = 36 };

    SDL_RenderCopy(renderer, sprite_info.texture, &src_rect, &dst_rect);
}

void Engine::render_tile(int index, int x, int y){

    if(index == -1){

        return;
    }

    if(x + 36 <= 0 || x >= viewport_width || y + 36 <= 0 || y >= viewport_height){

        return;
    }

    SDL_Rect src_rect = (SDL_Rect){ .x = (index % tileset_width) * 36, .y = ((int)(index / tileset_width)) * 36, .w = 36, .h = 36 };
    SDL_Rect dst_rect = (SDL_Rect){ .x = viewport_x + x, .y = viewport_y + y,. w = 36, .h = 36 };

    SDL_RenderCopy(renderer, texture_tileset, &src_rect, &dst_rect);
}

// TEXTURES

void Engine::texture_load_all(){

    texture_goblin = texture_load("./res/goblin.png");
    texture_monster = texture_load("./res/monsters.png");

    spritemap = {
        {GOBLIN_BASE, (SpriteInfo){ .texture = texture_goblin, .x = 0, .y = 0 }},
        {GOBLIN_L_UNARMED, (SpriteInfo){ .texture = texture_goblin, .x = 1, .y = 0 }},
        {GOBLIN_R_UNARMED, (SpriteInfo){ .texture = texture_goblin, .x = 1, .y = 1 }},
        {MONSTER_SPIDER, (SpriteInfo){ .texture = texture_monster, .x = 0, .y = 0 }},
    };

    texture_load_tileset();
}

void Engine::texture_clean_all(){

    SDL_DestroyTexture(texture_goblin);

    texture_goblin = nullptr;
}

SDL_Texture* Engine::texture_load(std::string path){

    SDL_Surface* loaded_surface = IMG_Load(path.c_str());
    if(loaded_surface == nullptr){

        std::cout << "Unable to load image! SDL Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* new_texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
    if(new_texture == nullptr){

        std::cout << "Unable to create texture! SDL Error: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    SDL_FreeSurface(loaded_surface);

    return new_texture;
}

void Engine::texture_load_tileset(){

    SDL_Surface* loaded_surface = IMG_Load("./res/tileset.png");
    if(loaded_surface == nullptr){

        std::cout << "Unable to load image! SDL Error: " << IMG_GetError() << std::endl;
        return;
    }

    texture_tileset = SDL_CreateTextureFromSurface(renderer, loaded_surface);
    if(texture_tileset == nullptr){

        std::cout << "Unable to create tileset! SDL Error: " << SDL_GetError() << std::endl;
        return;
    }

    tileset_width = loaded_surface->w / 36;
    tileset_height = loaded_surface->h / 36;

    SDL_FreeSurface(loaded_surface);
}
