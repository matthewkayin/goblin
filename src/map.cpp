#include "map.hpp"

#include <iostream>

bool operator==(SDL_Color lhs, const SDL_Color rhs){

    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b;
}

Map::RoomPrefab Map::load_prefab(std::string path){

    static const RoomPrefab NULL_PREFAB = (RoomPrefab){ .pixels = nullptr, .width = 0, .height = 0 };

    SDL_Surface* loaded_surface = IMG_Load(path.c_str());
    if(loaded_surface == nullptr){

        std::cout << "Unable to load image! SDL Error: " << IMG_GetError() << std::endl;
        return NULL_PREFAB;
    }

    RoomPrefab prefab = (RoomPrefab){
        .pixels = new SDL_Color[loaded_surface->w * loaded_surface->h],
        .width = loaded_surface->w,
        .height = loaded_surface->h
    };
    for(int x = 0; x < prefab.width; x++){

        for(int y = 0; y < prefab.height; y++){

            int index = (y * prefab.width) + x;
            uint32_t pixel = *((uint32_t*)loaded_surface->pixels + index);
            SDL_GetRGB(pixel, loaded_surface->format, &prefab.pixels[index].r, &prefab.pixels[index].g, &prefab.pixels[index].b);
        }
    }

    SDL_FreeSurface(loaded_surface);

    return prefab;
}

void Map::free_prefab(RoomPrefab* prefab){

    delete[] prefab->pixels;
}

Map::Room Map::load_from_prefab(RoomPrefab* prefab){

    static const SDL_Color FLOOR = (SDL_Color){ .r = 255, .g = 255, .b = 255, .a = 255 };
    static const SDL_Color WALL = (SDL_Color){ .r = 0, .g = 0, .b = 0, .a = 255 };
    static const SDL_Color PLAYER = (SDL_Color){ .r = 0, .g = 255, .b = 0, .a = 255 };

    Room room = (Room){
        .x = 0,
        .y = 0,
        .width = prefab->width,
        .height = prefab->height,
        .tiles = new int[prefab->width * prefab->height],
        .collider = new bool[prefab->width * prefab->height],
        .special = new int[prefab->width * prefab->height]
    };

    for(int i = 0; i < room.width * room.height; i++){

        SDL_Color pixel = prefab->pixels[i];
        if(pixel == FLOOR){

            room.tiles[i] = 1;
            room.collider[i] = false;
            room.special[i] = 0;

        }else if(pixel == WALL){

            room.tiles[i] = 0;
            room.collider[i] = true;
            room.special[i] = 0;

        }else if(pixel == PLAYER){

            room.tiles[i] = 1;
            room.collider[i] = false;
            room.special[i] = 1;
        }
    }

    return room;
}

void Map::free_room(Room* room){

    delete[] room->tiles;
    delete[] room->collider;
    delete[] room->special;
}

Map::Map(){

    width = 0;
    height = 0;
    tiles = nullptr;
    collider = nullptr;
    special = nullptr;
}

Map::~Map(){

    delete[] tiles;
    delete[] collider;
    delete[] special;
}

void Map::generate(){

    RoomPrefab prefab = load_prefab("room/base.png");
    Room room = load_from_prefab(&prefab);

    // width = room.width;
    // height = room.height;
    width = 26;
    height = 16;
    tiles = new int[width * height];
    collider = new bool[width * height];
    special = new int[width * height];

    for(int i = 0; i < width * height; i++){

        /*
        tiles[i] = room.tiles[i];
        collider[i] = room.collider[i];
        special[i] = room.special[i];
        */
        tiles[i] = -1;
        collider[i] = false;
        special[i] = 0;
    }

    generate_hallway((Point){ .x = 2, .y = 14 }, (Point){ .x = 2, .y = 2 });
    special[30] = 1;

    /*
    Point player_spawn = get_player_spawn();
    camera_x = player_spawn.x - 13;
    camera_y = player_spawn.y - 8;
    */
    camera_x = 0;
    camera_y = 0;

    free_prefab(&prefab);
    free_room(&room);
}

void Map::generate_hallway(Point start, Point end){

    int xdist = abs(start.x - end.x);
    int ydist = abs(start.y - end.y);

    int xdir = 0;
    int ydir = 0;
    if(end.x > start.x){

        xdir = 1;

    }else if(end.x < start.x){

        xdir = -1;
    }
    if(end.y > start.y){

        ydir = 1;

    }else if(end.y < start.y){

        ydir = -1;
    }

    if(xdir == 0 && ydir == 0){

        std::cout << "Hey this shouldn't happen. xdir and ydir both 0 in generate_hallway()" << std::endl;
        return;
    }

    Point step;
    Point perpendicular_step;
    Point elbow = (Point){ .x = -1, .y = -1 };
    Point other_elbow = (Point){ .x = -1, .y = -1 };
    if(xdist >= ydist){

        step = (Point){ .x = xdir, .y = 0 };
        perpendicular_step = (Point){ .x = 0, .y = ydir };
        if(ydir != 0){

            // 0 to xdist inclusive
            elbow = (Point){ .x = start.x + ((rand() % (xdist + 1)) * xdir), .y = start.y };
            other_elbow = (Point){ .x = elbow.x, .y = end.y };
        }

    }else{

        step = (Point){ .x = 0, .y = ydir };
        perpendicular_step = (Point){ .x = xdir, .y = 0 };
        if(xdir != 0){

            // 0 to ydist inclusive
            elbow = (Point){ .x = start.x, .y = start.y + ((rand() % (ydist + 1)) * ydir) };
            other_elbow = (Point){ .x = end.x, .y = elbow.y };
        }
    }

    Point current = start;
    bool use_perpendicular_step = current.x == elbow.x && current.y == elbow.y;
    while(!(current.x == end.x && current.y == end.y)){

        int index = current.x + (current.y * width);
        tiles[index] = 0;
        collider[index] = false;
        special[index] = 0;

        if(!use_perpendicular_step){

            current.x += step.x;
            current.y += step.y;
            if(current.x == elbow.x && current.y == elbow.y){

                use_perpendicular_step = true;
            }

        }else{

            current.x += perpendicular_step.x;
            current.y += perpendicular_step.y;
            if(current.x == other_elbow.x && current.y == other_elbow.y){

                use_perpendicular_step = false;
            }
        }
    }
}

void Map::camera_update(int player_x, int player_y){

    if(player_x - camera_x < 10){

        camera_x = player_x - 10;

    }else if(player_x - camera_x > 16){

        camera_x = player_x - 16;
    }

    if(player_y - camera_y < 6){

        camera_y = player_y - 6;

    }else if(player_y - camera_y > 10){

        camera_y = player_y - 10;
    }
}

bool Map::is_in_bounds(int x, int y){

    return x >= 0 && x < width && y >= 0 && y < height;
}

int Map::tile_at(int x, int y){

    return tiles[x + (y * width)];
}

bool Map::collider_at(int x, int y){

    return collider[x + (y * width)];
}

int Map::special_at(int x, int y){

    return special[x + (y * width)];
}

Map::Point Map::get_player_spawn(){

    for(int i = 0; i < width * height; i++){

        if(special[i] == 1){

            return (Point){ .x = i % width, .y = (int)(i / width) };
        }
    }

    std::cout << "Error! No player spawn found on map!" << std::endl;
    return (Point){ .x = -1, .y = -1 };
}
