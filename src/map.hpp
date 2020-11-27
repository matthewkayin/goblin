#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <string>

bool operator==(SDL_Color lhs, const SDL_Color rhs);

class Map{
    public:
        typedef struct{
            SDL_Color* pixels;
            int width;
            int height;
        } RoomPrefab;

        typedef struct{
            int x;
            int y;
            int width;
            int height;
            int* tiles;
            bool* collider;
            int* special;
        } Room;

        typedef struct{
            int x;
            int y;
        } Point;

        Map();
        ~Map();

        void generate();
        void generate_hallway(Point start, Point end);

        int camera_x;
        int camera_y;
        void camera_update(int player_x, int player_y);

        int width;
        int height;
        int* tiles;
        bool* collider;
        int* special;

        bool is_in_bounds(int x, int y);
        int tile_at(int x, int y);
        bool collider_at(int x, int y);
        int special_at(int x, int y);

        Point get_player_spawn();
    private:
        RoomPrefab load_prefab(std::string path);
        void free_prefab(RoomPrefab* prefab);

        Room load_from_prefab(RoomPrefab* prefab);
        void free_room(Room* room);
};
