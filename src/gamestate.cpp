#include "gamestate.hpp"
#include "pathfinding.hpp"

// INIT

Gamestate::Gamestate(){

    for(int i = 0; i < 8; i++){

        log[i] = "";
    }

    sprite_player_base = Sprite::GOBLIN_BASE;
    sprite_player_larm = Sprite::GOBLIN_L_UNARMED;
    sprite_player_rarm = Sprite::GOBLIN_R_UNARMED;

    player_x = 2;
    player_y = 2;

    player_health = 20;
    player_max_health = 100;

    enemy_spawn(4, 2);

    map_width = 26;
    map_height = 16;
    map = new bool*[map_width];
    for(int i = 0; i < map_width; i++){

        map[i] = new bool[map_height];
        for(int j = 0; j < map_height; j++){

            map[i][j] = false;
        }
    }
}

Gamestate::~Gamestate(){

    for(int i = 0; i < map_width; i++){

        delete map[i];
    }

    delete map;
}

// UPDATE

void Gamestate::input_enqueue(Input event){

    if(input_queue.size() < 5){

        input_queue.push(event);
    }
}

void Gamestate::update(float delta){

    if(!input_queue.empty()){

        Input input = input_queue.front();
        input_queue.pop();
        process_turn(input);
    }
}

void Gamestate::process_turn(Input input){

    bool move_player = false;
    int player_move_x = player_x;
    int player_move_y = player_y;
    if(input == Input::UP){

        player_move_y--;
        move_player = true;

    }else if(input == Input::RIGHT){

        player_move_x++;
        move_player = true;

    }else if(input == Input::DOWN){

        player_move_y++;
        move_player = true;

    }else if(input == Input::LEFT){

        player_move_x--;
        move_player = true;
    }

    if(move_player){

        for(unsigned int i = 0; i < enemy.size(); i++){

            if(enemy.at(i).x == player_move_x && enemy.at(i).y == player_move_y){

                log_message("You y(attacked) the spider for r(4 damage).");
                enemy.at(i).health -= 4;
                if(enemy.at(i).health <= 0){

                    log_message("r(You killed the spider!)");
                    enemy.erase(enemy.begin() + i);
                }
                move_player = false;
                break;
            }
        }
    }

    if(move_player){

        player_x = player_move_x;
        player_y = player_move_y;
    }

    for(unsigned int i = 0; i < enemy.size(); i++){

        int direction = pathfind(enemy.at(i).x, enemy.at(i).y, player_x, player_y, map, map_width, map_height);
        int enemy_move_x = enemy.at(i).x + direction_vector[direction][0];
        int enemy_move_y = enemy.at(i).y + direction_vector[direction][1];

        if(enemy_move_x == player_x && enemy_move_y == player_y){

            player_health -= 2;
            if(player_health < 0){

                player_health = 0;
            }

        }else{

            enemy.at(i).x = enemy_move_x;
            enemy.at(i).y = enemy_move_y;
        }
    }
}

// ENEMIES

void Gamestate::enemy_spawn(int x, int y){

    enemy.push_back((Enemy){
            .sprite = Sprite::MONSTER_SPIDER,
            .x = x,
            .y = y,
            .health = 20,
            .max_health = 20
    });
}

// LOGS

void Gamestate::log_message(std::string message){

    if(log_head == 0){

        log_head = 7;

    }else{

        log_head--;
    }

    log[log_head] = message;
}
