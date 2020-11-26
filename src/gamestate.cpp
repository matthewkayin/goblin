#include "gamestate.hpp"
#include "pathfinding.hpp"

#include <cstdlib>

// INIT

Gamestate::Gamestate(){

    srand(time(NULL));

    for(int i = 0; i < 8; i++){

        log[i] = "";
    }

    sprite_player_base = Sprite::GOBLIN_BASE;
    sprite_player_larm = Sprite::GOBLIN_L_UNARMED;
    sprite_player_rarm = Sprite::GOBLIN_R_UNARMED;

    player_x = 2;
    player_y = 2;

    player_health = 100;
    player_max_health = 100;

    player_low_attack = 5;
    player_high_attack = 7;
    player_defense = 5;
    player_speed = 5;

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

    // Determine player desired move
    bool move_player = false;
    int player_attacking_index = -1;
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

        player_attacking_index = enemy_occupies(player_move_x, player_move_y);
    }

    // Determine enemy desired move
    typedef struct{
        int x;
        int y;
        bool attacks_player;
    } EnemyTurn;

    if(enemy.size() == 0){

        player_take_turn(player_move_x, player_move_y, player_attacking_index);

    }else{

        // Determine what turn enemies will take
        EnemyTurn enemy_turns[enemy.size()];
        for(unsigned int i = 0; i < enemy.size(); i++){

            int direction = pathfind(enemy.at(i).x, enemy.at(i).y, player_x, player_y, map, map_width, map_height);
            int enemy_move_x = enemy.at(i).x + direction_vector[direction][0];
            int enemy_move_y = enemy.at(i).y + direction_vector[direction][1];
            enemy_turns[i] = (EnemyTurn){
                .x = enemy_move_x,
                .y = enemy_move_y,
                .attacks_player = enemy_move_x == player_x && enemy_move_y == player_y
            };
        }

        // Sort turns by speed
        // Each actor gets a speed score = 10 * speed stat
        // Speed score is then given an extra +5 if attacking so that attackers go first
        // And players get an extra +2.5 so that they go before actors who would otherwise have the same score
        int actor_count = 1 + enemy.size();
        int** turns = new int*[actor_count];
        for(int i = 0; i < actor_count; i++){

            turns[i] = new int[2];
            if(i == 0){

                turns[i][0] = -1;
                turns[i][1] = (player_speed * 10) + (player_attacking_index != -1 ? 5 : 0) + 2;

            }else{

                turns[i][0] = i - 1;
                turns[i][1] = (enemy.at(i - 1).speed * 10) + (enemy_turns[i - 1].attacks_player ? 5 : 0);
            }
        }
        quicksort(turns, 0, actor_count - 1);

        // Take each turn
        for(int i = actor_count - 1; i >= 0; i--){

            if(turns[i][0] == -1){

                player_take_turn(player_move_x, player_move_y, player_attacking_index);

            }else{

                int enemy_index = turns[i][0];
                enemy_take_turn(enemy_index, enemy_turns[enemy_index].x, enemy_turns[enemy_index].y, enemy_turns[enemy_index].attacks_player);
            }
        }

        if(player_attacking_index != -1){

            if(enemy.at(player_attacking_index).health < 0){

                enemy.erase(enemy.begin() + player_attacking_index);
            }
        }
    }
}

void Gamestate::player_take_turn(int move_x, int move_y, int attacking_index){

    // If attacking
    if(attacking_index != -1){

        // Check if enemy still at square
        if(enemy.at(attacking_index).x == move_x && enemy.at(attacking_index).y == move_y){

            int damage = calc_damage(player_low_attack, player_high_attack, enemy.at(attacking_index).defense);
            enemy.at(attacking_index).health -= damage;
            log_message("You hit the y(" + enemy.at(attacking_index).name + ") for r(" + std::to_string(damage) + " damage.)");
            if(enemy.at(attacking_index).health <= 0){

                log_message("You have slain the y(" + enemy.at(attacking_index).name + ".)");
            }

        }else{

            log_message("You attacked the y(" + enemy.at(attacking_index).name + "), but it stepped out of the way first.");
        }

    // If moving
    }else{

        // Check if target square is unoccupied
        int enemy_occupies_index = enemy_occupies(move_x, move_y);
        if(enemy_occupies_index == -1){

            player_x = move_x;
            player_y = move_y;

        // Else stop; path is blocked
        }else{

            log_message("The y(" + enemy.at(enemy_occupies_index).name + ") blocked your path.");
        }
    }
}

void Gamestate::enemy_take_turn(int index, int move_x, int move_y, bool attack_player){

    if(attack_player){

        if(player_x == move_x && player_y == move_y){

            int damage = calc_damage(enemy.at(index).low_attack, enemy.at(index).high_attack, player_defense);
            player_health -= damage;
            if(player_health < 0){

                player_health = 0;
            }
            log_message("The y(" + enemy.at(index).name + ") hit you for r(" + std::to_string(damage) + " damage.)");

        }else{

            log_message("The y(" + enemy.at(index).name + ") attacked, but you stepped out of the way first.");
        }

    }else{

        if(enemy_occupies(move_x, move_y) == -1 && !(player_x == move_x && player_y == move_y)){

            enemy.at(index).x = move_x;
            enemy.at(index).y = move_y;

        }else{

            // log_message("An enemy had its path blocked!");
        }
    }
}

int Gamestate::calc_damage(int low_attack, int high_attack, int target_defense){

    float attack = low_attack + (std::rand() % (high_attack - low_attack + 1));
    return (int)((attack * attack) / (attack + target_defense));
}

// ENEMIES

void Gamestate::enemy_spawn(int x, int y){

    enemy.push_back((Enemy){
            .name = "spider",
            .sprite = Sprite::MONSTER_SPIDER,
            .x = x,
            .y = y,
            .health = 20,
            .max_health = 20,
            .low_attack = 5,
            .high_attack = 7,
            .defense = 5,
            .speed = 5
    });
}

int Gamestate::enemy_occupies(int x, int y){

    for(unsigned int i = 0; i < enemy.size(); i++){

        if(enemy.at(i).x == x && enemy.at(i).y == y){

            return i;
        }
    }

    return -1;
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
