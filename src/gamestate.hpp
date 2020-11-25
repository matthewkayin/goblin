#pragma once

#include "sprite.hpp"

#include <string>
#include <queue>
#include <iostream>
#include <vector>

class Gamestate{

    public:
        typedef struct{
            Sprite sprite;
            int x;
            int y;
            int health;
            int max_health;
        } Enemy;
        typedef enum Input{
            NOTHING,
            UP,
            RIGHT,
            DOWN,
            LEFT
        } Input;

        // Init
        Gamestate();

        // Update
        void input_enqueue(Input event);
        void update(float delta);
        void process_turn(Input input);

        // Enemies
        void enemy_spawn(int x, int y);

        // Logs
        std::string log[8];
        int log_head = 0;
        void log_message(std::string message);

        // Rendering
        int interpolate_value(int current_value, int next_value);
        Sprite get_player_base_sprite();
        Sprite get_player_larm_sprite();
        Sprite get_player_rarm_sprite();
        int get_player_render_x();
        int get_player_render_y();
        int get_enemy_count();
        Sprite get_enemy_sprite(int index);
        int get_enemy_render_x(int index);
        int get_enemy_render_y(int index);

        Sprite sprite_player_base;
        Sprite sprite_player_larm;
        Sprite sprite_player_rarm;

        int player_x;
        int player_y;
        std::vector<Enemy> enemy;

    private:
        std::queue<Input> input_queue;
};
